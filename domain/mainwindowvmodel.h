#ifndef MAINWINDOWVMODEL_H
#define MAINWINDOWVMODEL_H

#include <QObject>
#include <data/apppreferences.h>
#include <data/dockercli.h>
#include <data/dockereventstream.h>

/*
 * Responsability:
 * - Manage the lifecycle of the data sources
 * - Expose UI state (ContainerTableModel)
 * - Expose the following actions:
 *   - startContainer(containerIndex)
 *   - stopContainer(containerIndex)
 *   - runAction(containerIndex)
 *   - refreshContainers()
 *   - reloadSettings()
 *
 */
class MainWindowVModel : public QObject
{
    Q_OBJECT

public:
    explicit MainWindowVModel(
        QObject *parent,
        AppPreferences *preferences,
        DockerCLI *dockercli,
        DockerEventStream *dockerEventStream
        );
private:
    AppPreferences *m_preferences;
    DockerCLI *m_dockercli;
    DockerEventStream *m_dockerEventStream;
};

#endif // MAINWINDOWVMODEL_H
