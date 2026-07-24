#ifndef DOCKEREVENTSTREAM_H
#define DOCKEREVENTSTREAM_H

#include <QObject>
#include <QProcess>

#include <data/data-objects/dockerevent.h>

class DockerEventStream : public QObject
{
    Q_OBJECT

public:
    enum class StreamError
    {
        FailedToStart,
        ProcessCrashed,
        ProcessError,
        ParseError,
    };

public:
    explicit DockerEventStream(QObject *parent = nullptr);
    ~DockerEventStream();

    bool isRunning() const;

public slots:
    void start();
    void stop();

signals:
    void started();
    void stopped();

    void eventReceived(const DockerEvent &event);

    void errorOccurred(DockerEventStream::StreamError error);

private slots:
    void onReadyRead();
    void onProcessError(QProcess::ProcessError error);
    void onProcessFinished(int exitCode,
                           QProcess::ExitStatus exitStatus);

private:
    std::optional<DockerEvent> parseDockerEvent(const QByteArray &rawLine);

    QProcess m_process{this};
};

#endif