#ifndef APPCONFIGREADER_H
#define APPCONFIGREADER_H

#include <QObject>
#include <QString>
#include <yaml-cpp/yaml.h>
#include "data-objects/configuredcontainers.h"
#include <optional>

/**
 * @brief Reads and caches the application's YAML configuration file.
 *
 * AppConfigReader is responsible for locating, reading, and parsing the
 * application's `preferences.yaml` file. The parsed YAML document is cached
 * in memory and can later be converted into a ConfiguredContainers object.
 *
 * If the configuration file does not exist, a default template is written to
 * the expected location before loading it into memory.
 *
 * Configuration is loaded only once during the lifetime of the object.
 * Subsequent calls to readConfigFile() are ignored unless a new
 * AppConfigReader instance is created.
 *
 * Results are communicated through Qt signals:
 * - configLoaded() when the configuration has been successfully loaded.
 * - errorOccurred(AppConfigReader::Error) when the configuration could not be read or parsed.
 */
class AppConfigReader : public QObject
{
    Q_OBJECT

public:
    enum class Error {
        FileReadError,
        TemplateWriteError
    };
    /**
     * @brief Creates a new configuration reader.
     *
     * Determines the platform-specific location of the application's
     * configuration file but does not read it automatically.
     *
     * @param parent Parent QObject.
     */
    explicit AppConfigReader(QObject *parent = nullptr);

    ~AppConfigReader() = default;

    /**
     * @brief Returns the configured containers from the cached YAML document.
     *
     * The configuration must have previously been loaded by calling
     * readConfigFile(). If no configuration has been loaded yet,
     * std::nullopt is returned.
     *
     * If the cached YAML exists but contains no configured containers,
     * a valid (possibly empty) ConfiguredContainers object is returned.
     *
     * @return The parsed container configuration, or std::nullopt if no
     * configuration has been loaded.
     */
    [[nodiscard]] std::optional<ConfiguredContainers> readContainers();

    /**
     * @brief Returns whether the configuration has already been loaded.
     *
     * Once this function returns true, subsequent calls to readConfigFile()
     * become no-ops and readContainers() can safely be used.
     *
     * @return true if the configuration is cached in memory; false otherwise.
     */
    [[nodiscard]] bool isCached();

public slots:
    /**
     * @brief Loads the application's configuration file into memory.
     *
     * Attempts to read and parse the application's `preferences.yaml` file.
     * If the file does not exist, the required directory structure is created
     * and a default configuration template is written before loading it.
     *
     * This function performs no work if the configuration has already been
     * loaded.
     *
     * On completion exactly one of the following signals is emitted:
     * - configLoaded() if the configuration was successfully loaded.
     * - criticalError() if the configuration could not be read or parsed.
     */
    void readConfigFile();

signals:
    /**
     * @brief Emitted after the configuration has been successfully loaded.
     */
    void configLoaded();

    /**
     * @brief Emitted when the configuration could not be loaded or parsed.
     */
    void errorOccurred(AppConfigReader::Error errorType);

private:
    static QString defaultConfig;

    bool m_isLoaded = false;
    YAML::Node m_configNode;
    QString m_configPath;
};
#endif // APPCONFIGREADER_H
