#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

/*
MainWindow either instantiates services in the constructor, it is the source of dependency injection (like the context api from React)

Services:
- <domain/containerservice.h>
- <data/apppreferences.h>

Data:
- <data/data-objects/containerinfo.h>
- <data/data-objects/appprefs/containerconfig.h>
- <data/data-objects/appprefs/containerspec.h>

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
  const QString &action) into ContainerService.runAction
    - Add a 2 second debounce timeout on that button

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
