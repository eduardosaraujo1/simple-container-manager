#ifndef CONTAINERDEFINITION_H
#define CONTAINERDEFINITION_H

#include <QString>
#include <optional>

class ContainerDefinition
{
public:
    explicit ContainerDefinition(
        const QString &name,
        const QString &label,
        const std::optional<QString> &icon = std::nullopt,
        const std::optional<QString> &action = std::nullopt);

    QString name() const;
    QString label() const;
    std::optional<QString> icon() const;
    std::optional<QString> action() const;

    QString toString() const;

private:
    QString m_name;
    QString m_label;
    std::optional<QString> m_icon;
    std::optional<QString> m_action;
};

#endif // CONTAINERDEFINITION_H
