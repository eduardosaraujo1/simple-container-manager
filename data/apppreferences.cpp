#include "apppreferences.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

QString AppPreferences::defaultConfig =
    "containers:\n"
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

std::optional<ContainerConfig> AppPreferences::containers()
{
    if (!m_isLoaded)
    {
        qWarning() << "[AppPreferences] Attempted to list containers before loading config file into memory.";
        return std::nullopt;
    }

    return ContainerConfig::fromYAML(m_configNode);
}

bool AppPreferences::isLoaded()
{
    return m_isLoaded;
}

namespace
{
    bool setupConfigDirectory(const QString &configFilePath)
    {
        const QFileInfo info(configFilePath);
        const QDir dir = info.dir();

        if (!dir.exists())
        {
            if (!dir.mkpath("."))
            {
                qWarning() << "[AppPreferences Directory Setup] \"make directory\" operation failed\n"
                           << "Config path: " << configFilePath << "\n";
                return false;
            }
        }

        return true;
    }

    void writeConfigFile(const QString &content, const QString &filePath)
    {
        if (!setupConfigDirectory(filePath))
        {
            qWarning() << "[AppPreferences Config File Writer] Failed to create directory containing the config."
                       << "Config path: " << filePath << "\n"
                       << "Content: " << content;
            return;
        }

        QFile file(filePath);

        if (!file.open(QFile::ReadWrite | QFile::Truncate))
        {
            qWarning() << "[AppPreferences Config File Writer] failed to open file resource for writing; interrupting config file write process\n"
                       << "Config path: " << filePath << "\n"
                       << "Content: " << content;
            return;
        }

        if (file.write(content.toUtf8()) == -1)
        {
            qWarning() << "[AppPreferences Config File Writer] failed to write config content to config file.\n"
                       << "Config path: " << filePath;
        }

        file.close();
    }

    std::optional<YAML::Node> readYAMLFile(QFile &file)
    {
        if (!file.exists())
        {
            qWarning() << "[AppPreferences YAML File Reader] file does not exist; interrupting YAML parsing";
            return std::nullopt;
        }
        if (!file.open(QFile::ReadOnly))
        {
            qWarning() << "[AppPreferences YAML File Reader]: failed to open file; interrupting YAML parsing";
            return std::nullopt;
        }

        const QByteArray fileContents = file.readAll();
        file.close();

        try
        {
            YAML::Node node = YAML::Load(fileContents.toStdString());
            return node;
        }
        catch (const YAML::ParserException &e)
        {
            qWarning() << "[AppPreferences YAML File Reader] YAML parser could not read the file.\nError description:" << e.what();
        }
        catch (const std::exception &e)
        {
            qWarning() << "[AppPreferences YAML File Reader] An unexpected error has occurred.\nError description:" << e.what();
        }

        return std::nullopt;
    }
}

AppPreferences::AppPreferences(QObject *parent)
    : QObject{parent}
{
    m_config_path = QStandardPaths::writableLocation(
                        QStandardPaths::AppConfigLocation) %
                    "/preferences.yaml";
}

void AppPreferences::readConfigFile()
{
    if (m_isLoaded)
    {
        return;
    }

    QFile file(m_config_path);
    YAML::Node rootNode;

    if (file.exists())
    {
        if (auto result = readYAMLFile(file))
        {
            rootNode = result.value();
        }
        else
        {
            qCritical() << "[AppPreferences] Failed to read config yaml file. Emitting crash signal";
            emit criticalError();
            return;
        }
    }
    else
    {
        qWarning() << "[AppPreferences] Preferences.yaml was not found. Attempting to write default file";
        writeConfigFile(AppPreferences::defaultConfig, m_config_path);
        rootNode = YAML::Load(AppPreferences::defaultConfig.toStdString());
    }

    m_configNode = rootNode;
    m_isLoaded = true;
    emit configLoaded();
}