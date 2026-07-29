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

        const QString name = QString::fromStdString(item["name"].as<std::string>(""));
        const QString label = QString::fromStdString(item["label"].as<std::string>(""));
        const QString icon = QString::fromStdString(item["icon"].as<std::string>(""));
        const QString action = QString::fromStdString(item["action"].as<std::string>(""));

        config.m_containers.append(ContainerDefinition(name, label, icon, action));
    }

    return config;
}

const QList<ContainerDefinition> &ConfiguredContainers::asList() const
{
    return m_containers;
}

QStringList ConfiguredContainers::containerNames() const
{
    QStringList result;

    for (const auto &container : m_containers)
        result.append(container.name());

    return result;
}

const ContainerDefinition *ConfiguredContainers::findByName(const QString& name) const {
     for (const auto &container : m_containers) {
        if (container.name() == name)
            return &container;
    }

    return nullptr;
}

bool ConfiguredContainers::hasContainers() const
{
    return !m_containers.isEmpty();
}