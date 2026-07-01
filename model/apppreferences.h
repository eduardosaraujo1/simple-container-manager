#ifndef APPPREFERENCES_H
#define APPPREFERENCES_H

#include <QObject>
#include <QList>
#include <QSet>
#include <QFile>
#include <yaml-cpp/yaml.h>

/**
 * @brief The AppPreferences class allows the application to interface with preferences set by the user
 */
class AppPreferences : public QObject
{
    Q_OBJECT
public:
    struct ContainerSpec {
        QString name;   // required
        QString label;  // required
        QString icon;   // nullable
        QString action; // nullable

        QString toString() const {
            return "ContainerSpec[name=\"" % name % "\",label=\"" % label
                   % "\",icon=\"" % icon % "\",action=\"" % action % "\"]";
        }
    };
    explicit AppPreferences(QObject *parent = nullptr);
    ~AppPreferences() = default;
    [[nodiscard]] const QList<ContainerSpec>& containers() const { return m_containers; };
    [[nodiscard]] const QSet<QString>& names() const { return m_container_names; };
    // ui developer: if you see this, remember to give the user a nice message if there are no containers
    [[nodiscard]] const bool hasContainers() { return !m_containers.isEmpty(); }
public slots:
    /**
     * @brief reads the config file and stores it in the application memory
     *
     * This function attempts to read the application config file and store its contents. If the file is not
     * found, the required folders and a template placeholder is placed.
     *
     * Operations are asynchronous, and handled by the Qt Event Loop. Once they are complete, one
     * of the following signals are emitted:
     * - preferencesUpdated()
     * - criticalError()
     */
    void readConfigFile();
private:
    static QString defaultConfig;
    /** @brief parses the YAML node and stores container preferences in memory. */
    void storeContainerPreferences(const YAML::Node &config);
    //attributes
    QList<ContainerSpec> m_containers;
    QSet<QString> m_container_names;
    QString m_config_path;
signals:
    void preferencesUpdated();
    void criticalError();
};

#endif // APPPREFERENCES_H
