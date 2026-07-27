#include "containerstate.h"
#include <QJsonObject>

ContainerState::ContainerState(const QString &id, const QString &name, Status status)
    : m_id(id), m_name(name), m_status(status) {}

ContainerState::Status ContainerState::statusFromString(QStringView str) {
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

QString ContainerState::statusToString(const ContainerState::Status &status) {
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

QString ContainerState::toString() const {
    return "ContainerInfo[id=\"" %
           this->id() % "\",name=\"" %
           this->name() % "\",status=\"" %
           statusToString(this->status()) % "\"]";
}

bool ContainerState::isValid() const {
    return !(m_id.isEmpty() || m_name.isEmpty());
}