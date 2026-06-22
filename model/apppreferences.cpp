#include "apppreferences.h"
#include <optional>
#include <sstream>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

namespace {
    bool setupConfigDirectory(const QString &configFilePath) {
        const QFileInfo info(configFilePath);
        const QDir dir = info.dir();

        if (! dir.exists()) {
            if (! dir.mkpath(".")) {
                qWarning() << "setupConfigDirectory: the \"make directory\" operation has failed; interrupting process\n"
                           << "Config path: " << configFilePath << "\n";
                return false;
            }
        }

        return true;
    }

    void writeConfigFile(const QString &content, const QString &filePath) {
        if (! setupConfigDirectory(filePath)) {
            qWarning() << "writeConfigFile: config folder setup failed; interrupting config file write process\n"
                       << "Config path: " << filePath << "\n"
                   << "Content: " << content;
            return;
        }

        QFile file(filePath);

        if (! file.open(QFile::ReadWrite | QFile::Truncate)) {
            qWarning() << "writeConfigFile: failed to open file resource for writing; interrupting config file write process\n"
                       << "Config path: " << filePath << "\n"
                       << "Content: " << content;
            return;
        }

        if (file.write(content.toUtf8()) == -1) {
            qWarning() << "writeConfigFile: failed to write config content to config file.\n"
                       << "Config path: " << filePath;
        }

        file.close();
    }

    std::optional<YAML::Node> readYAMLFile(QFile &file) {
        if (! file.exists()) {
            qWarning() << "readYAMLFile: file does not exist; interrupting YAML parsing";
            return std::nullopt;
        }
        if (! file.open(QFile::ReadOnly)) {
            qWarning() << "readYAMLFile: failed to open file; interrupting YAML parsing";
            return std::nullopt;
        }

        const QByteArray fileContents = file.readAll();
        file.close();

        try {
            YAML::Node node = YAML::Load(fileContents.toStdString());
            return node;
        } catch (const YAML::ParserException &e) {
            qWarning() << "readYAMLFile: YAML parser could not read the file.\nError description:"  << e.what();
        } catch (const std::exception &e) {
            qWarning() << "readYAMLFile: An unexpected error has occurred.\nError description:"  << e.what();
        }

        return std::nullopt;
    }

    QString stringifyNode(const YAML::Node &node) {
        std::stringstream ss;
        ss << node;

        return QString::fromStdString(ss.str());
    }

    std::optional<QList<AppPreferences::ContainerSpec>> parseContainerPreferences(const YAML::Node &config)
    {
        if (!(config.IsDefined() && config["containers"])) {
            qWarning().noquote() << "parseContainerPreferences: could not find `containers` configuration key in parsed config node.\n"
                                    "Raw node format:\n" << stringifyNode(config);
            return std::nullopt;
        }

        const YAML::Node containers = config["containers"];

        if (!containers.IsSequence()) {
            qWarning() << "parseContainerPreferences: `containers` key is defined but is not a sequence.";
            return std::nullopt;
        }

        QList<AppPreferences::ContainerSpec> list;
        // [SENIOR OBSERVATION] Pre-allocate space in the QList to minimize transient heap re-allocations
        list.reserve(static_cast<qsizetype>(containers.size()));

        for (size_t i = 0; i < containers.size(); ++i) {
            const YAML::Node item = containers[i];

            if (!item["name"] || !item["label"]) {
                qWarning() << "parseContainerPreferences: Skipping entry at index" << i
                           << "due to missing required 'name' or 'label' keys.";
                continue;
            }

            const QString containerName = QString::fromStdString(item["name"].as<std::string>());
            const QString containerLabel = QString::fromStdString(item["label"].as<std::string>());
            const QString containerIcon = item["icon"] ? QString::fromStdString(item["icon"].as<std::string>()) : QString();
            const QString containerAction = item["action"] ? QString::fromStdString(item["action"].as<std::string>()) : QString();

            AppPreferences::ContainerSpec spec{
                .name   = containerName,
                .label  = containerLabel,
                .icon   = containerIcon,
                .action = containerAction
            };

            list.append(spec);
        }

        return list;
    }

    QSet<QString> createContainerNameIndex(const QList<AppPreferences::ContainerSpec> &list)
    {
        QSet<QString> indexSet;
        // [SENIOR OBSERVATION] Pre-allocate space in the QSet to minimize transient heap re-allocations
        indexSet.reserve(list.size());

        for (const auto &spec : list) {
            indexSet.insert(spec.name);
        }

        return indexSet;
    }
}

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

AppPreferences::AppPreferences(QObject *parent)
    : QObject{parent}
{
    m_config_path = QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation)
        % "/preferences.yaml";
    qDebug() << m_config_path;
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
        writeConfigFile(AppPreferences::defaultConfig, m_config_path);
        rootNode = YAML::Load(AppPreferences::defaultConfig.toStdString());
    }

    // Note for future self (remove in production): if this fails, it should still emit "preferencesUpdated".
    // for scalability: if more preferences are added here, then the partial updaate should be accepted
    if (auto result = parseContainerPreferences(rootNode)) {
        m_containers = result.value();
        m_container_names = createContainerNameIndex(m_containers);
    }

    emit preferencesUpdated();
}
