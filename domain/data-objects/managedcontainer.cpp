#include "managedcontainer.h"
#include <QStringBuilder>
#include <array>

struct StatusMapping {
    ManagedContainer::Status status;
    QStringView stringValue;
};

static constexpr std::array<StatusMapping, 4> s_statusMappings{{
    { ManagedContainer::Status::Started, u"started" },
    { ManagedContainer::Status::Stopped, u"stopped" },
    { ManagedContainer::Status::Missing, u"missing" },
    { ManagedContainer::Status::Unknown, u"unknown" }
}};

ManagedContainer::ManagedContainer(const QString &name,
                                   const QString &runtimeId,
                                   const QString &label,
                                   const QString &iconPath,
                                   const QString &actionString,
                                   Status status)
    : m_name(name)
    , m_runtimeId(runtimeId)
    , m_label(label)
    , m_iconPath(iconPath)
    , m_actionString(actionString)
    , m_status(status)
{
}

ManagedContainer::Status ManagedContainer::statusFromString(QStringView str)
{
    for (const auto& mapping : s_statusMappings) {
        if (str.compare(mapping.stringValue, Qt::CaseInsensitive) == 0) {
            return mapping.status;
        }
    }
    return Status::Unknown;
}

QString ManagedContainer::statusToString(Status status)
{
    for (const auto& mapping : s_statusMappings) {
        if (mapping.status == status) {
            return mapping.stringValue.toString();
        }
    }
    return QStringLiteral("unknown");
}

bool ManagedContainer::isValid() const
{
    return !m_name.isEmpty() && m_status != Status::Unknown;
}

QString ManagedContainer::toString() const
{
    // [SENIOR OBSERVATION] Using QStringBuilder (%) to avoid heap reallocations during assembly
    return QStringLiteral("ManagedContainer[name=\"") % m_name %
           QStringLiteral("\", runtime_id=\"") % m_runtimeId %
           QStringLiteral("\", label=\"") % m_label %
           QStringLiteral("\", icon_path=\"") % m_iconPath %
           QStringLiteral("\", action_string=\"") % m_actionString %
           QStringLiteral("\", status=\"") % statusToString(m_status) %
           QStringLiteral("\"]");
}