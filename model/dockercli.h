#ifndef DOCKERCLI_H
#define DOCKERCLI_H

#include <QObject>
#include <QProcess>
#include <QStringList>
#include <model/data-objects/containerinfo.h>

class DockerCLI : public QObject
{
    Q_OBJECT

public:
    explicit DockerCLI(QObject *parent = nullptr);

    void requestContainerRefresh(QStringList namesFilter);
signals:
    // Callback-based response (to avoid blocking Qt Event Loop)
    void containersUpdated(QList<ContainerInfo> containers);
private slots:
    // Runs when the
    void onProcessDone(int exitCode, QProcess::ExitStatus status);
private:
    QProcess proc;
};

#endif // DOCKERCLI_H
