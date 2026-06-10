#ifndef DOCKERCLI_H
#define DOCKERCLI_H

#include <QProcess>
#include <QStringList>
#include <QObject>
#include "data-objects/containerinfo.h"

class DockerCLI : public QObject
{
    Q_OBJECT

public:
    explicit DockerCLI(QObject *parent = nullptr);

    void updateContainers(QStringList names);
private:
    QProcess proc;
    QList<ContainerInfo> containers;
signals:
    void containersUpdated(QList<ContainerInfo> containers);
};

#endif // DOCKERCLI_H
