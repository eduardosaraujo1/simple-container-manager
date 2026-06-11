#ifndef CONTAINERINFO_H
#define CONTAINERINFO_H

#include <QJsonObject>
#include <QString>
#include <QStringBuilder>

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

    static ContainerInfo fromJsonObject(const QJsonObject &obj, bool &error);

    /**
     * @brief Parse Status from string (case insensitive)
     * @param str
     * @return Status
     */
    static Status statusFromString(const QString &str);

    static QString statusToString(const Status &status);

    QString id;
    QString name;
    Status status;

    const QString toString() const;
};

#endif // CONTAINERINFO_H
