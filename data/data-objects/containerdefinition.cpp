#include "containerdefinition.h"

ContainerDefinition::ContainerDefinition(
    const QString &name,
    const QString &label,
    const QString &icon,
    const QString &action)
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

QString ContainerDefinition::icon() const
{
    return m_icon;
}

QString ContainerDefinition::action() const
{
    return m_action;
}

bool ContainerDefinition::hasAction() const
{
    return ! m_action.isEmpty();
}
bool ContainerDefinition::hasIcon() const
{
    return ! m_icon.isEmpty();
}

QString ContainerDefinition::toString() const
{
    return QString("ContainerSpec[name=\"%1\", label=\"%2\", icon=%3, action=%4]")
        .arg(m_name,
             m_label,
             m_icon.isEmpty() ? m_icon : "{{empty}}",
             m_action.isEmpty() ? m_action : "{{empty}}");
}