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
MainWindow is responsible for coordinating the UI with the already-constructed
application services. It does not instantiate or own application services or
their transitive dependencies.

MainWindow should contain presentation and interaction logic only. Container
workflows, Docker communication, automatic refresh, and operation timing are
owned by ContainerService.

Container states include (at least): Loading, Running, Stopped, Starting,
Stopping, NotFound. Loading is the placeholder state a container is shown in
between being populated into the list and its first authoritative state
update arriving.

Feature requirements:

- Hook ui->btnExit to application termination.

  - MainWindow does not perform explicit application-wide cleanup. (See
    main()'s cleanup responsibility below.)

- Populate ui->containerList (ContainerListWidget) with the configured
  containers received during application startup, each initially shown in
  State::Loading.

- If the configuration contains no containers — whether because the
  configuration explicitly defines none, or because no configuration file
  existed and AppPreferencesReader generated an empty default one — treat
  this as a normal, non-error state:

  - Keep the container list empty.
  - Show an appropriate welcoming/empty-state message.
  - Disable functionality that requires configured containers, such as
    refresh and container actions.

- Trigger the initial container refresh after the configured containers have
  been populated (skipped naturally if there are none).

- Hook ui->btnRefresh, a ThrottledButton, to ContainerService's container
  refresh operation.

  - ThrottledButton disables itself and starts its own internal cooldown
    (~10s) the moment it's pressed, and re-enables itself automatically when
    that cooldown elapses. It exposes a mutator to reset/re-enable it early.
  - MainWindow shows a loading indicator when the button is pressed.
  - If ContainerService reports the container state has been updated before
    the cooldown elapses, MainWindow hides the loading indicator and calls
    the button's reset mutator to re-enable it immediately.
  - If no response arrives, the button re-enables itself on its own once the
    cooldown elapses; MainWindow treats that auto re-enable as the signal to
    hide the loading indicator, so the UI never remains in a loading state
    indefinitely.
  - MainWindow does not run its own timer for this — the cooldown lives
    entirely inside ThrottledButton.

- Hook ContainerListWidget container actions to the corresponding
  ContainerService operations:

  - Start container
  - Stop container
  - Open/administer container

- When a start or stop operation is requested:

  - Perform a UI update through ContainerListWidget's setContainerState()
    method, setting the state to the state expected (Starting or Stopping).
  - This UI update is not authoritative. The actual container state is
    ultimately determined by ContainerService's subsequent state update.
  - MainWindow does not attempt to independently detect that a start/stop
    operation failed (no local timeout, no "did this ever get a response"
    tracking). It relies entirely on ContainerService's normal update paths
    (automatic refresh / DockerEventStream / manual refresh) to eventually
    deliver the authoritative state and correct the row.
  - Known limitation: if a bulk authoritative update that was already
    in flight before the start/stop click lands afterward, it may
    momentarily clobber the optimistic state with stale data before the
    real update arrives. This is not addressed by design; manual Refresh
    is the user's recourse if a row looks wrong.

- React to ContainerService's authoritative container state updates.

  - Pass the complete state collection to ContainerListWidget's bulk state
    update method.
  - ContainerListWidget is responsible for locating the corresponding rows
    and applying the individual states.

- React to ContainerService reporting that automatic refresh has failed, or
  that DockerEventStream is unavailable (including at startup):

  - Display a non-blocking warning in the UI using
    ui->lblAutoRefreshWarning. This is the single widget used for both
    "automatic refresh failed while running" and "Docker/DockerEventStream
    unavailable," since they represent the same underlying condition from
    the user's point of view.
  - The warning should not prevent normal interaction with the application.
  - If automatic refresh/DockerEventStream becomes available again, remove
    or hide the warning.

- MainWindow should not:

  - Instantiate ContainerService, DockerCLI, DockerEventStream, or any
    other application dependency.
  - Communicate directly with Docker.
  - Implement container start/stop workflows.
  - Implement automatic refresh behavior.
  - Decide how Docker events are interpreted.
  - Manage service-level timers, debounce/throttling behavior, or
    operation-completion tracking of any kind. (ThrottledButton is a
    presentation-level widget, not a service-level timer, and is exempt
    from this restriction.)

The main() function is the application's composition root.

main() is responsible for:

- Constructing QApplication.

- Constructing application dependencies in the correct dependency order,
  including:

  - AppPreferencesReader
  - ConfiguredContainers
  - DockerCLI
  - DockerEventStream
  - ContainerService
  - MainWindow

- Loading the application configuration before constructing MainWindow, via
  AppPreferencesReader, with three possible outcomes:

  1. No configuration file exists: AppPreferencesReader generates a default,
     empty configuration file and returns an empty ConfiguredContainers.
     This is NOT an error. Proceed to construct MainWindow normally; it
     will show its empty-state/welcome message per its own spec above.
  2. A configuration file exists and loads successfully (empty or not):
     proceed to construct MainWindow normally, providing it the resulting
     ConfiguredContainers.
  3. Configuration loading fails — either an existing file is invalid
     (e.g. malformed syntax), or no file existed and the attempt to write
     the new default file itself failed (e.g. permissions/filesystem
     error): this is a real error.

     - Do not construct MainWindow.
     - Do not start ContainerService, DockerCLI, or DockerEventStream —
       these are Docker-related services and this is a configuration
       failure unrelated to Docker.
     - Construct and display an error-state screen widget explaining that
       the application could not load its configuration and providing
       guidance on how to correct it.
     - Keep this as a non-aggressive, dismissible screen (not a blocking
       modal dialog as the primary experience) that the user can close via
       the normal window/exit affordances once acknowledged.

- If configuration loads successfully (cases 1 or 2 above), provide the
  resulting ConfiguredContainers to ContainerService and MainWindow as
  required.

- Display MainWindow and start the Qt event loop.

- Own the destruction order of application services on shutdown.
  DockerEventStream's destructor sends SIGTERM to its underlying Docker
  events subprocess, so no dedicated app-wide cleanup routine is required
  as long as:

  - Application services (DockerCLI, DockerEventStream, ContainerService)
    are held with a lifetime scoped to main() itself (e.g. stack
    allocation, or a holder that outlives app.exec()), and
  - That holder is destroyed immediately after app.exec() returns, before
    main() exits.

  Ordinary C++ destruction order at the end of main() is sufficient;
  MainWindow itself performs no cleanup (per its own spec above).

main() owns the composition and lifetime of application services. MainWindow
consumes those services but does not construct them.
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
