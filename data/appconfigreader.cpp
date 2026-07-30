#include "appconfigreader.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

QString AppConfigReader::defaultConfig =
    "containers:\n"
    "#  - name: php8.2-apache\n"
    "#    label: Apache com PHP\n"
    "#  - name: oracle-xe-11g\n"
    "#    label: Oracle Express 11g\n"
    "#    executable: calc"
    "#  - name: postgresql\n"
    "#    label: PostgreSQL\n"
    "#    icon: /path/to/icon.svg\n"
    "#  - name: mysql\n"
    "#    label: MySQL 8.0\n"
    "#    icon: /path/to/icon.svg\n"
    "#    executable: xdg-open http://localhost/phpmyadmin\n";

std::optional<ConfiguredContainers> AppConfigReader::readContainers()
{
    if (!m_isLoaded)
    {
        qWarning() << "[AppConfigReader] Attempted to list containers before loading config file into memory.";
        return std::nullopt;
    }

    return ConfiguredContainers::fromYAML(m_configNode);
}

bool AppConfigReader::isCached()
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
                qWarning() << "[AppConfigReader Directory Setup] \"make directory\" operation failed\n"
                           << "Config path:" << configFilePath << "\n";
                return false;
            }
        }

        return true;
    }

    bool writeConfigFile(const QString &content, const QString &filePath)
    {
        if (!setupConfigDirectory(filePath))
        {
            qWarning() << "[AppConfigReader Config File Writer] Failed to create directory containing the config."
                       << "Config path:" << filePath << "\n"
                       << "Content:" << content;
            return false;
        }

        QFile file(filePath);

        if (!file.open(QFile::ReadWrite | QFile::Truncate))
        {
            qWarning() << "[AppConfigReader Config File Writer] failed to open file resource for writing; interrupting config file write process\n"
                       << "Config path:" << filePath << "\n"
                       << "Content:" << content;
            return false;
        }

        if (file.write(content.toUtf8()) == -1)
        {
            qWarning() << "[AppConfigReader Config File Writer] failed to write config content to config file.\n"
                       << "Config path:" << filePath;
            return false;
        }

        file.close();
        return true;
    }

    std::optional<YAML::Node> readYAMLFile(QFile &file)
    {
        if (!file.exists())
        {
            qWarning() << "[AppConfigReader YAML File Reader] file does not exist; interrupting YAML parsing";
            return std::nullopt;
        }
        if (!file.open(QFile::ReadOnly))
        {
            qWarning() << "[AppConfigReader YAML File Reader]: failed to open file; interrupting YAML parsing";
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
            qWarning() << "[AppConfigReader YAML File Reader] YAML parser could not read the file.\nError description:" << e.what();
        }
        catch (const std::exception &e)
        {
            qWarning() << "[AppConfigReader YAML File Reader] An unexpected error has occurred.\nError description:" << e.what();
        }

        return std::nullopt;
    }
}

AppConfigReader::AppConfigReader(QObject *parent)
    : QObject{parent}
{
    m_configPath = QStandardPaths::writableLocation(
                        QStandardPaths::AppConfigLocation) %
                    "/preferences.yaml";
}

void AppConfigReader::readConfigFile()
{
    if (m_isLoaded)
    {
        return;
    }

    QFile file(m_configPath);
    YAML::Node rootNode;

    if (file.exists())
    {
        if (auto result = readYAMLFile(file))
        {
            rootNode = result.value();
        }
        else
        {
            qCritical() << "[AppConfigReader] Failed to read config yaml file. Emitting crash signal";
            emit errorOccurred(Error::FileReadError);
            return;
        }
    }
    else
    {
        qWarning() << "[AppConfigReader] preferences.yaml was not found. Attempting to write default file";
        if (!writeConfigFile(AppConfigReader::defaultConfig, m_configPath)) {
            emit errorOccurred(Error::TemplateWriteError);
        }

        // Avoid reading the file that was just created to prevent a potential error
        rootNode = YAML::Load(AppConfigReader::defaultConfig.toStdString());
    }

    m_configNode = rootNode;
    m_isLoaded = true;
    emit configLoaded();
}