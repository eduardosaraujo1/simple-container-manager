#include <QDebug>
#include <QTimer>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include "dockereventstream.h"

DockerEventStream::DockerEventStream(QObject *parent)
    : QObject{parent}
{
    connect(&m_proc, &QProcess::errorOccurred, this, &DockerEventStream::onStartError);
    connect(&m_proc, &QProcess::finished, this, &DockerEventStream::onUnexpectedError);
    connect(&m_proc, &QProcess::readyReadStandardError, this, &DockerEventStream::onErrorMessage);
    connect(&m_proc, &QProcess::readyReadStandardOutput, this, &DockerEventStream::onEventDetected);

    restart();
}

DockerEventStream::~DockerEventStream() {
    m_proc.disconnect(this);
    m_attempt_restart = false;
    if (m_proc.state() == QProcess::Running) {
        qWarning() << "~DockerEventStream: QProcess was not finished. Forcing kill.";
        m_proc.kill();
    }
}

bool DockerEventStream::isActive(){
    return m_proc.state() == QProcess::Running;
}

void DockerEventStream::abort() {
    m_attempt_restart = false;

    if (m_proc.state() == QProcess::Running) {
        qInfo() << "abort: Gracefully terminating Docker Events stream...";
        m_proc.terminate();

        if (!m_proc.waitForFinished(3000)) {
            qWarning() << "abort: Stream did not close gracefully within 3s. Forcing kill.";
            m_proc.kill();
        }
    }
}

void DockerEventStream::restart() {
    if (m_proc.state() != QProcess::NotRunning) {
        abort();
    }
    m_attempt_restart = true;

    qInfo() << "restart: Initializing Docker Events stream...";

    QStringList arguments = {
        "events",
        "--format",
        "{\"action\":{{json .Action}},\"container_id\":{{json .Actor.ID}}}",
        "--filter",
        "Type=container"
    };
    m_proc.start("docker", arguments, QProcess::ReadOnly);
    // if you're reading this, also check out error handling functions
}

std::optional<DockerEvent> DockerEventStream::parseDockerEvent(const QByteArray &rawLine) {
    QJsonParseError jsonParseError;
    const QJsonDocument doc = QJsonDocument::fromJson(rawLine, &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError) {
        qWarning() << "parseDockerEvent: Error parsing JSON stream line.\n"
                   << "Raw Data:" << rawLine << "\n"
                   << "Error:" << jsonParseError.errorString();
        return std::nullopt;
    }

    const QJsonObject obj = doc.object();
    if (obj.isEmpty()) {
        qWarning() << "parseDockerEvent: Parsed successfully but payload is not a JSON Object.";
        return std::nullopt;
    }

    const QJsonValue containerId = obj.value("container_id");
    const QJsonValue action = obj.value("action");

    if (containerId.isUndefined() || action.isUndefined()) {
        qWarning() << "parseDockerEvent: Required keys missing from stream JSON context.";
        return std::nullopt;
    }

    const QString actionStr = action.toString();
    const DockerEvent::Action actionEnum = DockerEvent::actionFromString(actionStr);

    if (actionEnum == DockerEvent::Action::Unknown) {
        qWarning() << "parseDockerEvent: Received safe unhandled action type:" << actionStr;
    }

    return DockerEvent{
        actionEnum,
        containerId.toString()
    };
}

void DockerEventStream::handleErrors() {
    if (m_consecutive_errors < 0) {
        m_consecutive_errors = 1;
    } else {
        m_consecutive_errors++;
    }

    if (m_attempt_restart && m_consecutive_errors <= 3) {
        qWarning() << "handleErrors: Attempting automatic restart" << m_consecutive_errors << "of 3.";

        const int delayMs = 1000 * m_consecutive_errors;
        QTimer::singleShot(delayMs, this, &DockerEventStream::restart);}
    else {
        qCritical() << "handleErrors: Attempted to start event listener too many times. Giving up.";
        abort();
        emit criticalError();
    }
}

void DockerEventStream::onStartError(QProcess::ProcessError error) {
    if (m_attempt_restart) {
        qWarning() << "onStartError: Docker Events m_process could not be started.";
        handleErrors();
    }
}

void DockerEventStream::onUnexpectedError(int exitCode, QProcess::ExitStatus exitStatus) {
    if (m_attempt_restart) {
        qWarning() << "onUnexpectedError: Docker Events exited unexpectedly. Code:" << exitCode << "Status:" << exitStatus;
        handleErrors();
    }
}

void DockerEventStream::onErrorMessage() {
    const QByteArray errorOutput = m_proc.readAllStandardError();

    qWarning() << "Docker Event Stream Stderr:" << errorOutput.trimmed();
}

void DockerEventStream::onEventDetected() {
    // [SENIOR OBSERVATION:] Because docker events is a continuous stream, readAllStandardOutput()
    // might occasionally grab half of a JSON string if the OS buffer flushes mid-write.
    // Using canReadLine() ensures we only parse fully completed lines ending in '\n'.
    // (before this revision we used .readyStandardOutput and .readAllStandardOutput())
    while (m_proc.canReadLine()) {
        const QByteArray rawLine = m_proc.readLine();

        if (auto event = parseDockerEvent(rawLine)) {
            // if a parse was successful, we're sure it's healthy
            qInfo() << "onEventDetected: successsfully captured docker Event";
            m_consecutive_errors = 0;

            emit eventReceived(*event);
            continue;
        }

        qWarning() << "onEventDetected (attempt " % QString::number(m_consecutive_errors+1) %  "): Failed to parse JSON event line:" << rawLine.trimmed();

        if (m_consecutive_errors > 3) {
            qCritical() << "onEventDetected: Too many bad parses in a row. Stream unstable.";
            abort();
            emit criticalError();
            return;
        }

        ++m_consecutive_errors;
    }}

