#ifndef DOCKEREVENTSTREAM_H
#define DOCKEREVENTSTREAM_H

#include <optional>
#include <QObject>
#include <QString>
#include <QProcess>
#include <QByteArray>
#include <model/data-objects/dockerevent.h>

class DockerEventStream : public QObject
{
    Q_OBJECT
private:
    QProcess proc{this};
    int m_consecutive_errors = 0;
    bool m_attempt_restart = true;
public:
    explicit DockerEventStream(QObject *parent = nullptr);

    ~DockerEventStream();

    bool isActive();

private:
    std::optional<DockerEvent> parseDockerEvent(const QByteArray &rawLine);

    void handleErrors();
public slots:
    void restart();

    void abort();
private slots:
    void onStartError(QProcess::ProcessError error);

    void onUnexpectedError(int exitCode, QProcess::ExitStatus exitStatus);

    void onErrorMessage();

    void onEventDetected();
signals:
    void eventReceived(const DockerEvent &event);

    void criticalError();
};

#endif // DOCKEREVENTSTREAM_H
