#ifndef CONTAINERDEFINITION_H
#define CONTAINERDEFINITION_H

#include <QString>

class ContainerDefinition
{
public:
    explicit ContainerDefinition(
        const QString &name,
        const QString &label,
        const QString &icon = "",
        const QString &action = "");

    [[nodiscard]] QString name() const;
    [[nodiscard]] QString label() const;
    [[nodiscard]] QString icon() const;
    [[nodiscard]] QString action() const;

    QString toString() const;

    [[nodiscard]] bool hasAction() const;
    [[nodiscard]] bool hasIcon() const;

private:
    QString m_name;
    QString m_label;
    QString m_icon;
    QString m_action;
};

#endif // CONTAINERDEFINITION_H
