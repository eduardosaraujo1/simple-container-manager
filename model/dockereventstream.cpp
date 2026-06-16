#include <QDebug>
#include <QTimer>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include "dockereventstream.h"

DockerEventStream::DockerEventStream(QObject *parent)
    : QObject{parent}
{
    connect(&proc, &QProcess::errorOccurred, this, &DockerEventStream::onStartError);
    connect(&proc, &QProcess::finished, this, &DockerEventStream::onUnexpectedError);
    connect(&proc, &QProcess::readyReadStandardError, this, &DockerEventStream::onErrorMessage);
    connect(&proc, &QProcess::readyReadStandardOutput, this, &DockerEventStream::onEventDetected);

    restart();
}

DockerEventStream::~DockerEventStream() {
    proc.disconnect(this);
    m_attempt_restart = false;
    if (proc.state() == QProcess::Running) {
        qWarning() << "~DockerEventStream: QProcess was not finished. Forcing kill.";
        proc.kill();
    }
}

bool DockerEventStream::isActive(){
    return proc.state() == QProcess::Running;
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

    const QJsonValue containerIdVal = obj.value("container_id");
    const QJsonValue actionVal = obj.value("action");

    if (containerIdVal.isUndefined() || actionVal.isUndefined()) {
        qWarning() << "parseDockerEvent: Required keys missing from stream JSON context.";
        return std::nullopt;
    }

    const QString actionStr = actionVal.toString();
    const DockerEvent::Action action = DockerEvent::actionFromString(actionStr);

    if (action == DockerEvent::Action::Unknown) {
        qWarning() << "parseDockerEvent: Received safe unhandled action type:" << actionStr;
    }

    return DockerEvent{
        action,
        containerIdVal.toString()
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

        // [SENIOR OBSERVATION:] We must not call restart() directly here! If the Docker daemon is down,
        // calling restart() immediately will instantly fail, triggering handleErrors() again.
        // This causes an infinite recursion loop that will crash the app with a Stack Overflow.
        // Using QTimer::singleShot provides a non-blocking Exponential Backoff delay.
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
        qWarning() << "onStartError: Docker Events process could not be started.";
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
    const QByteArray errorOutput = proc.readAllStandardError();

    qWarning() << "Docker Event Stream Stderr:" << errorOutput.trimmed();
}

void DockerEventStream::onEventDetected() {
    // [SENIOR OBSERVATION:] Because docker events is a continuous stream, readAllStandardOutput()
    // might occasionally grab half of a JSON string if the OS buffer flushes mid-write.
    // Using canReadLine() ensures we only parse fully completed lines ending in '\n'.
    while (proc.canReadLine()) {
        const QByteArray rawLine = proc.readLine();

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

void DockerEventStream::abort() {
    m_attempt_restart = false;

    if (proc.state() == QProcess::Running) {
        qInfo() << "abort: Gracefully terminating Docker Events stream...";
        proc.terminate();

        if (!proc.waitForFinished(3000)) {
            qWarning() << "abort: Stream did not close gracefully within 3s. Forcing kill.";
            proc.kill();
        }
    }
}

/**
 * Triggers auto restart behavior! Check onUnexpectedError and onStartError
 */
void DockerEventStream::restart() {
    if (proc.state() != QProcess::NotRunning) {
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
    proc.start("docker", arguments, QProcess::ReadOnly);
}