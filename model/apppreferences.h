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
    };
    explicit AppPreferences(QObject *parent = nullptr);
    ~AppPreferences() = default;
    const QList<ContainerSpec>& containers() const { return m_containers; };
    const QSet<QString>& names() const { return m_container_names; };
public slots:
    void refreshConfig();
    // If YAML file does not exist, writeDefaultConfigs
    // Read YAML file
    // Store container preferences
    // emit preferencesUpdated
private:
    void writeDefaultConfigs(QFile &file) const;
    // Figure out how YamlDocument will represent arrays (`containers:`) and write the template
    std::optional<YAML::Node> readYAMLFile(QFile &file) const;
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
