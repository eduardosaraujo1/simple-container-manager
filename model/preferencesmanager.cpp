#include "preferencesmanager.h"

PreferencesManager::PreferencesManager(QObject *parent)
    : QObject{parent}
{
    m_config_relpath = QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation)
        + "/simple-container-manager/preferences.yaml";
}

void PreferencesManager::refreshConfig() {
    //
}

void PreferencesManager::writeDefaultConfigs(QFile &file) const {
    //
}

QByteArray PreferencesManager::readFileData(QFile &file) const {
    //
}

// std::optional<YamlDocument> PreferencesManager::parseConfig(const QByteArray) const;

// Figure out how YamlDocument will represent arrays (`containers:`) and write the spec