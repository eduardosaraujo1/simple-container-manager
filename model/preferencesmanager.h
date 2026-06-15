#ifndef PREFERENCESMANAGER_H
#define PREFERENCESMANAGER_H

#include <QObject>
#include <QList>
#include <QSet>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFile>

/*
 * Needs attribute to specify config file path on Windows, Linux and MacOS
 * Needs method to read/reload configs
 * Needs ContainerPreferences List to store config for each container
 */
class PreferencesManager : public QObject
{
    Q_OBJECT
public:
    struct ContainerSpec {
        QString name; // required
        QString label; // required
        QString icon; // nullable
        QString action; // nullable
    };

private:
    QList<ContainerSpec> m_containers;
    QSet<QString> m_container_names;
    QString m_config_relpath; // Don't forget to set this in the constructor
public:
    explicit PreferencesManager(QObject *parent = nullptr);

    ~PreferencesManager();

    const QList<ContainerSpec>& containers() const {
        return this->m_containers;
    };

    const QSet<QString>& names() const {
        return this->m_container_names;
    };
public slots:
    void refreshConfig();
private:
    void writeDefaultConfigs(QFile &file) const;

    QByteArray readFileData(QFile &file) const;

    // std::optional<YamlDocument> parseConfig(const QByteArray) const;

    // Figure out how YamlDocument will represent arrays (`containers:`) and write the spec
signals:
    void preferencesUpdated();

    void criticalError();
};

#endif // PREFERENCESMANAGER_H
