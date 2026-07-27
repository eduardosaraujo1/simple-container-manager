#ifndef APPCONFIGREADER_H
#define APPCONFIGREADER_H

#include <QObject>
#include <QString>
#include <yaml-cpp/yaml.h>
#include "data-objects/appprefs/configuredcontainers.h"
#include <optional>

class AppConfigReader : public QObject
{
    Q_OBJECT
public:
    explicit AppConfigReader(QObject *parent = nullptr);
    ~AppConfigReader() = default;

    /**
     * @brief Returns the parsed container configuration.
     *
     * Triggers readConfigFile() first if the config hasn't been loaded yet. Always returns a valid
     * ConfiguredContainers, even if nothing was found or a critical error was emitted (in which case it
     * will simply be empty).
     */
    [[nodiscard]] std::optional<ConfiguredContainers> readContainers();
    [[nodiscard]] bool isCached();

public slots:
    /**
     * @brief reads the config file and stores the raw YAML node in application memory
     *
     * This function attempts to read the application config file and store its contents. If the file is not
     * found, the required folders and a template placeholder is placed.
     *
     * This is a no-op if the config file has already been read (see isLoaded()).
     *
     * Once complete, one of the following signals is emitted:
     * - preferencesUpdated()
     * - criticalError()
     */
    void readConfigFile();

private:
    static QString defaultConfig;
    // attributes
    bool m_isLoaded = false;
    YAML::Node m_configNode;
    QString m_configPath;
signals:
    void configLoaded();
    void criticalError();
};

#endif // APPCONFIGREADER_H
