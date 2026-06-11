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

    QString id;
    QString name;
    Status status;

    static ContainerInfo fromJsonObject(const QJsonObject &obj, bool &error);

    /**
     * @brief Parse Status from string (case insensitive)
     * @param str
     * @return Status
     */
    static Status statusFromString(const QString &str);
};

#endif // CONTAINERINFO_H
