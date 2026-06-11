#include "containerinfo.h"
#include <QJsonObject>

ContainerInfo ContainerInfo::fromJsonObject(const QJsonObject &obj, bool &error) {
    error = false;

    if (! (obj.contains("id")
          && obj.contains("name")
          && obj.contains("status")
          )) {
        error = true;
        return ContainerInfo{};
    }
    const ContainerInfo::Status status
        = ContainerInfo::statusFromString(obj.value("status").toString());

    return ContainerInfo{
        obj.value("id").toString(),
        obj.value("name").toString(),
        status
    };
}

const QString ContainerInfo::toString() const {
    return "ContainerInfo[" %
           this->id % "," %
           this->name % "," %
           ContainerInfo::statusToString(this->status) % "]";
}


ContainerInfo::Status ContainerInfo::statusFromString(const QString &str) {
    if (str.toLower() == "running")
        return Status::Running;
    if (str.toLower() == "exited")
        return Status::Exited;
    if (str.toLower() == "created")
        return Status::Created;
    if (str.toLower() == "paused")
        return Status::Paused;
    if (str.toLower() == "restarting")
        return Status::Restarting;
    if (str.toLower() == "removing")
        return Status::Removing;
    if (str.toLower() == "dead")
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