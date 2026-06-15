#include "preferencesmanager.h"

PreferencesManager::PreferencesManager(QObject *parent)
    : QObject{parent}
{
    m_config_relpath = QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation)
        + "/simple-container-manager/preferences.yaml";
}
