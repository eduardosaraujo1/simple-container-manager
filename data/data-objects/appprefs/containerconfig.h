#ifndef CONTAINERCONFIG_H
#define CONTAINERCONFIG_H

#include <QHash>
#include <QSet>
#include <QString>
#include <optional>
#include <yaml-cpp/yaml.h>
#include "containerspec.h"

/**
 * @brief The ContainerConfig class represents the fully-parsed set of containers defined
 * in the user's preferences file.
 *
 * Instances are produced via the static fromYAML() factory, which is responsible for all
 * YAML parsing/validation concerns. This keeps that responsibility out of AppPreferences,
 * which now only deals with reading the file and handing off the raw YAML::Node.
 */
class ContainerConfig
{
public:
    /**
     * @brief Builds a ContainerConfig from a parsed YAML root node.
     *
     * Missing or malformed entries are skipped with a warning; a missing/invalid
     * `containers` key results in an empty (but valid) ContainerConfig.
     */
    [[nodiscard]] static std::optional<ContainerConfig> fromYAML(const YAML::Node &node);

    [[nodiscard]] const QHash<QString, ContainerSpec> &containers() const;
    [[nodiscard]] QSet<QString> containerNames() const;
    [[nodiscard]] bool hasContainers() const;

private:
    ContainerConfig() = default;

    QHash<QString, ContainerSpec> m_containers; // name -> spec
};

#endif // CONTAINERCONFIG_H