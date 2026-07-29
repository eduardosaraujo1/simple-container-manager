#ifndef CONFIGUREDCONTAINERS_H
#define CONFIGUREDCONTAINERS_H

#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>
#include <optional>
#include <yaml-cpp/yaml.h>
#include "containerdefinition.h"

/**
 * @brief The ConfiguredContainers class represents the fully-parsed set of containers defined
 * in the user's preferences file.
 *
 * Instances are produced via the static fromYAML() factory, which is responsible for all
 * YAML parsing/validation concerns. This keeps that responsibility out of AppPreferences,
 * which now only deals with reading the file and handing off the raw YAML::Node.
 */
class ConfiguredContainers
{
public:
    /**
     * @brief Builds a ConfiguredContainers from a parsed YAML root node.
     *
     * Missing or malformed entries are skipped with a warning; a missing/invalid
     * `containers` key results in an empty (but valid) ConfiguredContainers.
     */
    [[nodiscard]] static std::optional<ConfiguredContainers> fromYAML(const YAML::Node &node);

    [[nodiscard]] const QList<ContainerDefinition> &asList() const;
    [[nodiscard]] QStringList containerNames() const;
    [[nodiscard]] const ContainerDefinition *findByName(const QString& name) const;
    [[nodiscard]] bool hasContainers() const;

private:
    ConfiguredContainers() = default;

    QList<ContainerDefinition> m_containers;
};

#endif // CONFIGUREDCONTAINERS_H
