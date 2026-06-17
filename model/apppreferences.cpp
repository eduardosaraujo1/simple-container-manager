#include "apppreferences.h"

AppPreferences::AppPreferences(QObject *parent)
    : QObject{parent}
{
    m_config_path = QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation)
        + "/simple-container-manager/preferences.yaml";
}

void AppPreferences::refreshConfig() {
    QFile file(m_config_path);

    if (! file.exists()) {
        writeDefaultConfigs(file);
        if (! file.exists()) {
            qCritical() << "refreshConfig: file was not found even after template was written; emitting crash signal.";
            emit criticalError();
        }
    }

    YAML::Node rootNode;
    {
        auto rootNodeResult = readYAMLFile(file);
        if (! rootNodeResult.has_value()) {
            qCritical() << "refreshConfig: failed to read config yaml file; emitting crash signal";
            emit criticalError();
        }
        rootNode = std::move(*rootNodeResult);
    }

    // Note for future self (remove in production): if this fails, it should still emit "preferencesUpdated".
    // for scalability: if more preferences are added here, then the partial updaate should be accepted
    storeContainerPreferences(rootNode);

    emit preferencesUpdated();
}

void AppPreferences::writeDefaultConfigs(QFile &file) const {
    // Figure out how YamlDocument will represent arrays (`containers:`) and write the template
    /*
    containers:
      - name: mysql
        label: MySQL 8.0
        icon: /path/to/icon.svg
        action: xdg-open http://localhost/phpmyadmin
      - name: oracle-xe-11g
        label: Oracle Express 11g
        icon: /path/to/icon.svg
      - name: php8.2-apache
        label: Oracle Express 11g
    */
}

std::optional<YAML::Node> AppPreferences::readYAMLFile(QFile &file) const {
    // Parse file

    // (somehow) ensure its integrity

    return std::nullopt;
}

void AppPreferences::storeContainerPreferences(const YAML::Node &rootNode) {
    // Ensure container preferences exists

    // Parse root node into QList<ContainerSpec>
    // note: leave logging to the function that attempts to parse the root node

    // If it succeeds, put QList in the app root
}