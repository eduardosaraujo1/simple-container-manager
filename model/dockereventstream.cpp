#include <QByteArray>
#include <QDebug>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <optional>
#include "dockereventstream.h"

DockerEventStream::DockerEventStream(QObject *parent)
    : QObject{parent}
{
    connect(&m_proc, &QProcess::errorOccurred, this, &DockerEventStream::onProcessError);
    connect(&m_proc, &QProcess::finished, this, &DockerEventStream::onProcessFinished);
    connect(&m_proc, &QProcess::readyReadStandardOutput, this, &DockerEventStream::onReadyRead);
    connect(&m_proc, &QProcess::readyReadStandardError, [&m_proc]()
            { qDebug() << "[DockerEventStream] Standard error captured: " << m_proc->readAllStandardError(); });
}

DockerEventStream::~DockerEventStream()
{
    m_proc.disconnect(this);
    m_attempt_restart = false;
    if (m_proc.state() == QProcess::Running)
    {
        qWarning() << "~DockerEventStream: QProcess was not finished. Forcing kill.";
        m_proc.kill();
    }
}

bool DockerEventStream::isRunning() const
{
    return m_proc.state() == QProcess::Running;
}

void DockerEventStream::stop()
{
    if (m_proc.state() != QProcess::Running)
    {
        return;
    }

    qInfo() << "[DockerEventStream] Gracefully stopping Docker Events stream...";
    m_proc.terminate();

    if (!m_proc.waitForFinished(3000))
    {
        qWarning() << "[DockerEventStream] Could not stop stream gracefully within 3s. Forcing kill...";
        m_proc.kill();
    }
}

void DockerEventStream::start()
{
    if (isRunning())
    {
        qInfo() << "[DockerEventStream] Docker Stream is already running. Ignoring 'start' request.";
        return;
    }

    qInfo() << "[DockerEventStream] Initializing Docker Events stream...";

    QStringList arguments = {
        "events",
        "--format",
        "{\"action\":{{json .Action}},\"container_id\":{{json .Actor.ID}}}",
        "--filter",
        "Type=container"};

    m_proc.start("docker", arguments, QProcess::ReadOnly);
}

void DockerEventStream::onReadyRead()
{
    // docker events is a continuous stream. Read complete lines only so
    // we never attempt to parse half of a JSON object.
    while (m_process.canReadLine())
    {
        const QByteArray rawLine = m_process.readLine();

        if (auto event = parseDockerEvent(rawLine))
        {
            qDebug() << "[DockerEventStream] Event received.";

            emit eventReceived(*event);
            continue;
        }

        qWarning()
            << "[DockerEventStream] Failed to parse Docker event:"
            << rawLine.trimmed();
    }
}

void DockerEventStream::onProcessError(QProcess::ProcessError error)
{
    qWarning()
        << "[DockerEventStream] Process error:"
        << error;

    switch (error)
    {
    case QProcess::FailedToStart:
        emit errorOccurred(StreamError::FailedToStart);
        break;

    case QProcess::Crashed:
        emit errorOccurred(StreamError::ProcessCrashed);
        break;

    default:
        emit errorOccurred(StreamError::ProcessError);
        break;
    }
}

void DockerEventStream::onProcessFinished(int exitCode,
                                          QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit)
    {
        qWarning()
            << "[DockerEventStream] Process terminated unexpectedly."
            << "Exit code:" << exitCode;

        emit errorOccurred(StreamError::ProcessCrashed);
    }
    else
    {
        qDebug()
            << "[DockerEventStream] Stream stopped."
            << "Exit code:" << exitCode;
    }

    emit stopped();
}

std::optional<DockerEvent> DockerEventStream::parseDockerEvent(const QByteArray &rawLine)
{
    QJsonParseError jsonParseError;
    const QJsonDocument doc = QJsonDocument::fromJson(rawLine, &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
    {
        qWarning() << "[DockerEventStream] Could not parse docker event data received from terminal.\n"
                   << "Error:" << jsonParseError.errorString();
        << "Raw Data:" << rawLine << "\n" return std::nullopt;
    }

    const QJsonObject obj = doc.object();
    if (obj.isEmpty())
    {
        qWarning() << "[DockerEventStream] Parsed JSON from docker events listener interpreted as empty object.";
        return std::nullopt;
    }

    const QJsonValue containerId = obj.value("container_id");
    const QJsonValue action = obj.value("action");

    if (containerId.isUndefined() || action.isUndefined())
    {
        qWarning() << "[DockerEventStream] Required keys missing from parsed event JSON data.";
        return std::nullopt;
    }

    const QString actionStr = action.toString();
    const DockerEvent::Action actionEnum = DockerEvent::actionFromString(actionStr);

    if (actionEnum == DockerEvent::Action::Unknown)
    {
        qDebug() << "parseDockerEvent: Received safe unhandled action type:" << actionStr;
    }

    return DockerEvent{
        actionEnum,
        containerId.toString()};
}