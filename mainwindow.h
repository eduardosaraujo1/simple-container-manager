#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*
MainWindow either instantiates services in the constructor, it is the source of dependency injection (like the context api from React)

Services:
- <domain/containerservice.h>
- <domain/containeractionrunner.h>
- <data/apppreferences.h>

Data:
- <data/data-objects/containerinfo.h>

ContainerService encapsulates all Docker-related infrastructure
(DockerCLI + DockerEventStream) and exposes a higher-level API for
container operations and state synchronization.

MainWindow responsibilities:

- Hook ui->btnExit into an application exit call.

- Start AppPreferences (call readConfigFile() at least once and don't
  mutate ui->containerList until it's loaded).

- Ensure AppPreferences has not emitted any critical errors and has a
  QList<AppPreferences::ContainerSpec>.
    - If it errors:
        - The data source already performs its own logging.
        - Show an error dialog.
        - Exit the application once the dialog is dismissed.

- Call ui->containerList.initialize(QList<AppPreferences::ContainerSpec>)
  using AppPreferences' data.

    - If the configuration contains no containers:
        - Skip any ContainerService initialization.
        - Disable refresh-related functionality.
        - Show a welcome message such as
          "Welcome! Please edit the configuration file to add containers."

    - Otherwise:
        - Initialize ContainerService.
        - Trigger the initial container refresh.

- Hook ui->btnRefresh into ContainerService::refreshContainers().

- Hook ui->containerList.containerAction(const QString &containerName,
  const QString &action) into a new QProcess.

    - Disable the corresponding action button.
    - Re-enable it after one second to prevent process spam.

    // TODO:
    // Replace this with a dedicated ActionRunner service/use case so
    // MainWindow no longer executes external processes directly.

- Hook ui->containerList.containerAction(const QString &containerName,
  const QString &action) into ContainerActionRunner::run().

    ContainerActionRunner is a dedicated application use case responsible
    for executing user-defined container actions.

    Responsibilities:
        - Validate that the requested action is allowed.
        - Launch the configured command in a QProcess.
        - Prevent action spam by rejecting duplicate requests while one
          is already running (or temporarily disabling execution).
        - Notify the presenter when execution starts and finishes so the
          corresponding action button can be enabled/disabled.
        - Report execution failures.

    MainWindow should only react to its signals by updating the UI.

    Although the initial implementation may internally use QProcess,
    process management should remain encapsulated within
    ContainerActionRunner.

- Hook ui->containerList.containerAction(const QString &containerName,
  const QString &action) into a new QProcess.

    - Disable the corresponding action button.
    - Re-enable it after one second to prevent process spam.

    // TODO:
    // Replace this with a dedicated ActionRunner service/use case so
    // MainWindow no longer executes external processes directly.

Keep MainWindow focused on presentation logic only.
ContainerService owns all container-related workflows, timing,
automatic refresh behavior and Docker interactions.
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
