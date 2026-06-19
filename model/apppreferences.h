#ifndef APPPREFERENCES_H
#define APPPREFERENCES_H

#include <QObject>
#include <QList>
#include <QSet>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFile>
#include <optional>
#include <yaml-cpp/yaml.h>

/**
 * @brief The AppPreferences class allows the application to interface with preferences set by the user
 *
 * The method `refreshConfig` is used to read the file, uppon being called the class should emit either:
 * - preferencesUpdated()
 * - criticalError()
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
    static QString defaultConfig;
public slots:
    void refreshConfig();
    void writeConfigFile(const QString& content) const;
private:
    std::optional<YAML::Node> readYAMLFile(QFile &file) const;
    /** @brief storeContainerPreferences parses the YAML node for container preferences stores it internally through the cache. */
    void storeContainerPreferences(const YAML::Node &rootNode);
    //attributes
    QList<ContainerSpec> m_containers;
    QSet<QString> m_container_names;
    QString m_config_path;
signals:
    void preferencesUpdated();
    void criticalError();
};

#endif // APPPREFERENCES_H
