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
    ContainerInfo::Status status = ContainerInfo::statusFromString(obj.value("status").toString());

    return ContainerInfo{
        obj.value("id").toString(),
        obj.value("name").toString(),
        status
    };
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