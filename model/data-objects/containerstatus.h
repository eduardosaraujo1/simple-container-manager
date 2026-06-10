#ifndef CONTAINERSTATUS_H
#define CONTAINERSTATUS_H

#include <QDebug>

enum ContainerStatus
{
    Created,
    Running,
    Paused,
    Restarting,
    Exited,
    Removing,
    Dead,
    Unknown
};

ContainerStatus containerStatusFromString(const QString &str) {
    if (str.toLower() == "running")
        return ContainerStatus::Running;
    if (str.toLower() == "exited")
        return ContainerStatus::Exited;
    if (str.toLower() == "created")
        return ContainerStatus::Created;
    if (str.toLower() == "paused")
        return ContainerStatus::Paused;
    if (str.toLower() == "restarting")
        return ContainerStatus::Restarting;
    if (str.toLower() == "removing")
        return ContainerStatus::Removing;
    if (str.toLower() == "dead")
        return ContainerStatus::Dead;

    qWarning() << "Container '" << str << "' does not have a known status";
    return ContainerStatus::Unknown;
}

#endif // CONTAINERSTATUS_H
