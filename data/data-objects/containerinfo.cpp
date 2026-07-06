#include "containerinfo.h"
#include <QJsonObject>

ContainerInfo::Status ContainerInfo::statusFromString(QStringView str) {
    if (str.compare(u"running", Qt::CaseInsensitive) == 0)
        return Status::Running;
    if (str.compare(u"exited", Qt::CaseInsensitive) == 0)
        return Status::Exited;
    if (str.compare(u"created", Qt::CaseInsensitive) == 0)
        return Status::Created;
    if (str.compare(u"paused", Qt::CaseInsensitive) == 0)
        return Status::Paused;
    if (str.compare(u"restarting", Qt::CaseInsensitive) == 0)
        return Status::Restarting;
    if (str.compare(u"removing", Qt::CaseInsensitive) == 0)
        return Status::Removing;
    if (str.compare(u"dead", Qt::CaseInsensitive) == 0)
        return Status::Dead;

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

QString ContainerInfo::toString() const {
    return "ContainerInfo[id=\"" %
           this->id() % "\",name=\"" %
           this->name() % "\",status=\"" %
           statusToString(this->status()) % "\"]";
}

bool ContainerInfo::isValid() const {
    return m_id.isEmpty();
}