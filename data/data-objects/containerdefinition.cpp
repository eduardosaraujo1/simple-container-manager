#include "containerdefinition.h"

ContainerDefinition::ContainerDefinition(
    const QString &name,
    const QString &label,
    const std::optional<QString> &icon,
    const std::optional<QString> &action)
    : m_name(name),
      m_label(label),
      m_icon(icon),
      m_action(action)
{
}

QString ContainerDefinition::name() const
{
    return m_name;
}

QString ContainerDefinition::label() const
{
    return m_label;
}

std::optional<QString> ContainerDefinition::icon() const
{
    return m_icon;
}

std::optional<QString> ContainerDefinition::action() const
{
    return m_action;
}

QString ContainerDefinition::toString() const
{
    return QString("ContainerSpec[name=\"%1\", label=\"%2\", icon=%3, action=%4]")
        .arg(m_name)
        .arg(m_label)
        .arg(m_icon ? *m_icon : "null")
        .arg(m_action ? *m_action : "null");
}