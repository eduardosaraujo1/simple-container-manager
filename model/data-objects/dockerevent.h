#ifndef DOCKEREVENT_H
#define DOCKEREVENT_H

#include <QString>
#include <QStringView>

class DockerEvent
{
public:
    enum class Action {
        Die,
        Pause,
        Start,
        Stop,
        Unpause,
        Unknown,
    };

    static Action containerActionFromString(QStringView str);

    static QString containerActionToString(Action action);
private:
    Action m_action = Action::Unknown;
    QString m_container_id;
public:
    DockerEvent() = default;

    DockerEvent(Action action, const QString &container_id)
        : m_action(action), m_container_id(container_id) {}

    Action action() const {
        return this->m_action;
    }

    const QString& containerId() const {
        return this->m_container_id;
    }

    QString toString() const;

    bool isValid() const;
};

#endif // DOCKEREVENT_H
