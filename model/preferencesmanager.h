#ifndef PREFERENCESMANAGER_H
#define PREFERENCESMANAGER_H

#include <QObject>

/*
 * Needs attribute to specify config file path on Windows, Linux and MacOS
 * Needs method to read/reload configs
 * Needs ContainerPreferences List to store config for each container
 */
class PreferencesManager : public QObject
{
    Q_OBJECT
public:
    explicit PreferencesManager(QObject *parent = nullptr);

signals:
};

#endif // PREFERENCESMANAGER_H
