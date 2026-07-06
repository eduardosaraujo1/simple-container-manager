#ifndef MANAGEDCONTAINER_H
#define MANAGEDCONTAINER_H

#include <QString>
#include <QStringView>

class ManagedContainer
{
public:
    enum class Status
    {
        Started,
        Stopped,
        Missing,
        Unknown
    };
    // Static conversion utilities
    static Status statusFromString(QStringView str);
    static QString statusToString(Status status);
    // Constructors
    ManagedContainer() = default;
    ManagedContainer(const QString &name,
                     const QString &runtimeId,
                     const QString &label,
                     const QString &iconPath,
                     const QString &actionString,
                     Status status);
    // Accessors
    const QString& name() const { return m_name; }
    const QString& runtimeId() const { return m_runtimeId; }
    const QString& label() const { return m_label; }
    const QString& iconPath() const { return m_iconPath; }
    const QString& actionString() const { return m_actionString; }
    Status status() const { return m_status; }
    // Methods
    bool isValid() const;
    QString toString() const;

private:
    QString m_name;
    QString m_runtimeId;
    QString m_label;
    QString m_iconPath;
    QString m_actionString;
    Status m_status = Status::Unknown;
};

#endif // MANAGEDCONTAINER_H