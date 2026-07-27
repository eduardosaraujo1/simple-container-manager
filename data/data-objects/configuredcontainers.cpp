#include "configuredcontainers.h"
#include <sstream>
#include <QDebug>

namespace
{
    QString stringifyNode(const YAML::Node &node)
    {
        std::stringstream ss;
        ss << node;

        return QString::fromStdString(ss.str());
    }
}

std::optional<ConfiguredContainers> ConfiguredContainers::fromYAML(const YAML::Node &node)
{

    if (!(node.IsDefined() && node["containers"]))
    {
        qWarning().noquote() << "[ContainerConfig YAML Parser] could not find `containers` configuration key in parsed config node.\n"
                                "Raw node format:\n"
                             << stringifyNode(node);
        return std::nullopt;
    }

    const YAML::Node containers = node["containers"];

    if (!containers.IsSequence())
    {
        qWarning() << "[ContainerConfig YAML Parser] `containers` key is defined but is not a sequence.";
        return std::nullopt;
    }

    // [SENIOR OBSERVATION] Pre-allocate space in the QHash to minimize transient heap re-allocations
    ConfiguredContainers config;
    config.m_containers.reserve(static_cast<qsizetype>(containers.size()));

    for (size_t i = 0; i < containers.size(); ++i)
    {
        const YAML::Node item = containers[i];

        if (!item["name"] || !item["label"])
        {
            qWarning() << "[ContainerConfig YAML Parser] Skipping entry at index" << i
                       << "due to missing required 'name' or 'label' keys.";
            continue;
        }

        const QString name = QString::fromStdString(item["name"].as<std::string>());
        const QString label = QString::fromStdString(item["label"].as<std::string>());

        const std::optional<QString> icon = item["icon"]
                                                ? std::make_optional(QString::fromStdString(item["icon"].as<std::string>()))
                                                : std::nullopt;
        const std::optional<QString> action = item["action"]
                                                  ? std::make_optional(QString::fromStdString(item["action"].as<std::string>()))
                                                  : std::nullopt;

        config.m_containers.insert(name, ContainerDefinition(name, label, icon, action));
    }

    return config;
}

const QHash<QString, ContainerDefinition> &ConfiguredContainers::containers() const
{
    return m_containers;
}

QSet<QString> ConfiguredContainers::containerNames() const
{
    return QSet<QString>(m_containers.keyBegin(), m_containers.keyEnd());
}

bool ConfiguredContainers::hasContainers() const
{
    return !m_containers.isEmpty();
}