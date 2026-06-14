#include "dockerevent.h"

DockerEvent::Action DockerEvent::containerActionFromString(QStringView str) {
    if (str.compare(u"die", Qt::CaseInsensitive) == 0)
        return Action::Die;
    if (str.compare(u"start", Qt::CaseInsensitive) == 0)
        return Action::Start;
    if (str.compare(u"stop", Qt::CaseInsensitive) == 0)
        return Action::Stop;
    if (str.compare(u"pause", Qt::CaseInsensitive) == 0)
        return Action::Pause;
    if (str.compare(u"unpause", Qt::CaseInsensitive) == 0)
        return Action::Unpause;

    return Action::Unknown;
}

QString DockerEvent::containerActionToString(Action action) {
    if (action == Action::Die)
        return "die";
    if (action == Action::Start)
        return "start";
    if (action == Action::Stop)
        return "stop";
    if (action == Action::Pause)
        return "pause";
    if (action == Action::Unpause)
        return "unpause";

    return "unknown";
}

QString DockerEvent::toString() const {
    return "DockerEvent[action=\"" %
           containerActionToString(this->action()) % "\",container_id=\"" %
           this->containerId() % "\"]";
}

bool DockerEvent::isValid() const {
    return !m_container_id.isEmpty();
}
