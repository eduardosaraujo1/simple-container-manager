#ifndef CONTAINERINFO_H
#define CONTAINERINFO_H

#include <QJsonObject>
#include <QString>

struct ContainerInfo
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

    static Status statusFromString(const QString &str);

    static QString statusToString(const Status &status);
private:
    QString m_id;
    QString m_name;
    Status m_status;
public:
    ContainerInfo() = default;

    ContainerInfo(QString id, QString name, Status status)
        :  m_id(id), m_name(name), m_status(status) {}

    const QString& id() const {
        return this->m_id;
    }
    const QString& name() const {
        return this->m_name;
    }
    const Status& status() const {
        return this->m_status;
    }

    bool isValid() const {
        return m_id.isEmpty();
    }

    const QString toString() const;
};

#endif // CONTAINERINFO_H
