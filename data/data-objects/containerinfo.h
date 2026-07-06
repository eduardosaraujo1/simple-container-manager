#ifndef CONTAINERINFO_H
#define CONTAINERINFO_H

#include <QString>

class ContainerInfo
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
    QString m_id;
    QString m_name;
    Status m_status = Status::Unknown;
public:
    ContainerInfo() = default;

    ContainerInfo(const QString &id, const QString &name, Status status);

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

#endif // CONTAINERINFO_H
