#include "apppreferences.h"
#include <QFileInfo>
#include <QDir>

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
        writeConfigFile(AppPreferences::defaultConfig); // writeConfigFile is a public slot, so is refreshConfig. Will it block the UI event loop?
        rootNode = YAML::Load(AppPreferences::defaultConfig.toStdString());
    }

    // Note for future self (remove in production): if this fails, it should still emit "preferencesUpdated".
    // for scalability: if more preferences are added here, then the partial updaate should be accepted
    storeContainerPreferences(rootNode);

    emit preferencesUpdated();
}

void AppPreferences::writeConfigFile(const QString& content) const {
    QFileInfo info(m_config_path);
    QDir dir = info.dir();

    if (! dir.exists()) {
        bool success = dir.mkpath(".");
        if (! success) {
            qWarning() << "writeConfigFile: folder create operation (mkpath) failed; interrupting config file write process\n"
                       << "Config path: " << m_config_path << "\n"
                   << "Content: " << content;
            return;
        }
    }

    QFile file(m_config_path);

    if (! file.open(QFile::ReadWrite | QFile::Truncate)) {
        qWarning() << "writeConfigFile: failed to open file resource for writing; interrupting config file write process\n"
                   << "Config path: " << m_config_path << "\n"
                   << "Content: " << content;
        return;
    }

    if (file.write(content.toUtf8()) == -1) {
        qWarning() << "writeConfigFile: failed to write config content to config file.\n"
                   << "Config path: " << m_config_path;
    }

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
