#ifndef DOCKEREVENTSTREAM_H
#define DOCKEREVENTSTREAM_H

#include <QObject>
#include <QProcess>
#include <model/data-objects/dockerevent.h>

class DockerEventStream : public QObject
{
    Q_OBJECT
public:
    explicit DockerEventStream(QObject *parent = nullptr);
    ~DockerEventStream();
    bool isActive();
    bool waitUntilActive();
public slots:
    void restart();
    void abort();
private:
    //attributes
    QProcess m_proc{this};
    int m_consecutive_errors = 0;
    bool m_attempt_restart = true;
    //assistent methods
    void handleErrors();
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
