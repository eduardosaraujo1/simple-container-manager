#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*
MainWindow either instantiates or receives the data sources in the constructor (not both though; I'll leave this
decision to the whims of the LLM)
Sources: <data/dockercli.h> <data/dockereventstream.cpp> <data/apppreferences.h>
Sources: <data/data-objects/containerinfo.h>
MainWindow needs to find a way to, cleanly and as soon as it is loaded:
- Hook ui->btnExit into an application exit call
- Start app preferences (call read config file at least once and don't mutate ui->containerList until it's loaded)
- Ensure AppPreferences has not emitted any critical errors, and has a QList<AppPreferences::ContainerSpec>
    - if it errors... idk, the data source already has logging so we should export the logs into a file, show the error message and end the application when the popup is closed
- Call containerList.initialize(QList<AppPrefrences::ContainerSpec>) with appPreferences's data
    - if the list happens to be empty, skip anything related to container refresh (refresh button, docker event initialization) and show a message like "Welcome! Please modify the config file" or something
    - If it's not empty then proceed with the other processes normally
- Ensure DockerEventStream is up (call .start() after initialization)
- Hook ui->btnRefresh click into the container state reload logic (more on that later)
- Hook the signal DockerEventStream::eventReceived into the reload logic (more on that later)
    - DockerEventStream::criticalError exists. If it fails the application isn't exactly broken, it just doesn't refresh automatically, so I'm unsure what to do. Add a little warning at the corner maybe (add a stub and comment whatever you think is best)
- Hook ui->containerList.containerToggle(const QString &containerName, bool isStartCommand) into a DockerCLI startContainer or stopContainer call.
    - To comply with optimistic UI requirements, we should show the container as started before DockerCLI responds with the new state. Currently the ContainerListWidget (ui->containerList) does not have a method to do that. Pretend it does and write a comment above it, signaling that I should implement that.
- Hook ui->containerList.containerAction(const QString &containerName, const QString &action) into a new QProcess to run (mark a TODO that says soon a refactor will abstract this into a domain ActionRunner service/usecase class)
    - Disable the button and reenable it after 1 second, I want to avoid process spamming
- Implement the reload logic in a dedicated domain/reloadcontainersaction.cpp file: 
    - Disable the refresh button
    - Request a container refresh through DockerCLI::requestContainerRefresh
    - Wait on signal DockerCLi::containersUpdated:
        - After 1 second add a loading indicator
        - after 15 seconds, if there is no response then show an error dialog and remove the loading indicator
    - When the signal is received, it will be with the parameter QList<ContainerInfo>
    - Call ui->containerList.refreshContainerInfo() with the container info list
    - Remove the loading indicator
    - After 1 second, enable the button 
Use best practices in separation of concerns to avoid messy code
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
