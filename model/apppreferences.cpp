#include "apppreferences.h"

QString AppPreferences::defaultConfig = "containers:\n"
           "#  - name: php8.2-apache\n"
           "#    label: Apache com PHP\n"
           "#  - name: oracle-xe-11g\n"
           "#    label: Oracle Express 11g\n"
           "#    action: $HOME/Scripts/oracle-11g-launch.sh\n"
           "#  - name: postgresql\n"
           "#    label: PostgreSQL\n"
           "#    icon: /path/to/icon.svg\n"
           "#  - name: mysql\n"
           "#    label: MySQL 8.0\n"
           "#    icon: /path/to/icon.svg\n"
           "#    action: xdg-open http://localhost/phpmyadmin\n";

AppPreferences::AppPreferences(QObject *parent)
    : QObject{parent}
{
    m_config_path = QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation)
        + "/simple-container-manager/preferences.yaml";
}

void AppPreferences::refreshConfig() {
    QFile file(m_config_path);
    YAML::Node rootNode;

    if (file.exists()) {
        if (auto result = readYAMLFile(file)) {
            rootNode = result.value();
        } else {
            qCritical() << "refreshConfig: failed to read config yaml file; emitting crash signal";
            emit criticalError();
            return;
        }
    } else {
        qWarning() << "refreshConfig: preferences.yaml was not found; attempting to write default file";
        writeConfigFile(AppPreferences::defaultConfig); // writeConfigFile is a public slot, so is refreshConfig. Will it block the main event loop?
        rootNode = YAML::LoadAll(AppPreferences::defaultConfig.toStdString());
    }

    // Note for future self (remove in production): if this fails, it should still emit "preferencesUpdated".
    // for scalability: if more preferences are added here, then the partial updaate should be accepted
    storeContainerPreferences(rootNode);

    emit preferencesUpdated();
}

void AppPreferences::writeConfigFile(const QString& content) const {
    // Please review: this is definitively incomplete, didn't handle
    // if the folder doesn't exist, or log anything, or if this is the proepr syntax
    // for overriding file contents
    QFile file(m_config_path);

    if (! file.open(QFile::OpenModeFlag::ReadWrite))
        return;

    file.write(content.toUtf8());
    file.close();
}

std::optional<YAML::Node> AppPreferences::readYAMLFile(QFile &file) const {
    // Parse file into YAML Node

    // (somehow) ensure its integrity

    return std::nullopt;
}

void AppPreferences::storeContainerPreferences(const YAML::Node &rootNode) {
    // Ensure container preferences exists

    // Parse root node into QList<ContainerSpec>
    // note: leave logging to the function that attempts to parse the root node

    // If it succeeds, put QList in the app root
}
