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
        ProcessError
    };

public:
    explicit DockerEventStream(QObject *parent = nullptr);
    ~DockerEventStream();

    [[nodiscard]] bool isRunning() const;
    [[nodiscard]] bool isStarting() const;
    [[nodiscard]] bool isNotRunning() const;
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