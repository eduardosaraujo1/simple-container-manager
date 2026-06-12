#include "containerinfo.h"
#include <QJsonObject>


const QString ContainerInfo::toString() const {
    return "ContainerInfo[" %
           this->id() % "," %
           this->name() % "," %
           ContainerInfo::statusToString(this->status()) % "]";
}

ContainerInfo::Status ContainerInfo::statusFromString(const QString &str) {
    const QString lower = str.toLower();
    if (lower == "running")
        return Status::Running;
    if (lower == "exited")
        return Status::Exited;
    if (lower == "created")
        return Status::Created;
    if (lower == "paused")
        return Status::Paused;
    if (lower == "restarting")
        return Status::Restarting;
    if (lower == "removing")
        return Status::Removing;
    if (lower == "dead")
        return Status::Dead;

    qWarning() << "Container '" << str << "' does not have a known status";
    return Status::Unknown;
}

QString ContainerInfo::statusToString(const ContainerInfo::Status &status) {
    if (status == Status::Running)
        return "running";
    if (status == Status::Exited)
        return "exited";
    if (status == Status::Created)
        return "created";
    if (status == Status::Paused)
        return "paused";
    if (status == Status::Restarting)
        return "restarting";
    if (status == Status::Removing)
        return "removing";
    if (status == Status::Dead)
        return "dead";

    return "unknown";
}