#ifndef DOCKEREVENTSTREAM_H
#define DOCKEREVENTSTREAM_H

#include <QObject>
#include <QProcess>
#include <model/data-objects/dockerevent.h>

class DockerEventStream : public QObject
{
    Q_OBJECT
private:
    int m_consecutive_errors = 0;
    bool m_attempt_restart = true;
    QProcess proc{this};
public:
    explicit DockerEventStream(QObject *parent = nullptr);

    ~DockerEventStream();

    bool isActive();
private slots:
    void onStartError(QProcess::ProcessError error);

    void onUnexpectedError(int exitCode, QProcess::ExitStatus exitStatus);

    void onErrorMessage();

    void onEventDetected();

    void stop();

    void restart();
signals:
    void eventReceived(const DockerEvent event);

    void criticalError();
};

#endif // DOCKEREVENTSTREAM_H
