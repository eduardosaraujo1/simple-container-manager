#ifndef CONTAINERSTATE_H
#define CONTAINERSTATE_H

#include <QString>

class ContainerState
{
public:
    enum class Status
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

    static Status statusFromString(QStringView str);

    static QString statusToString(const Status &status);
private:
    QString m_name;
    QString m_id;
    Status m_status = Status::Unknown;
public:
    ContainerState() = default;

    ContainerState(const QString &id, const QString &name, Status status);

    const QString& id() const {
        return this->m_id;
    }
    const QString& name() const {
        return this->m_name;
    }
    Status status() const {
        return this->m_status;
    }

    bool isValid() const;

    QString toString() const;
};

#endif // CONTAINERSTATE_H
