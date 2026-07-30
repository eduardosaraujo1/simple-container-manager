# Test Cases

## 0. Test preparation

- [ ] Docker installed and working.
- [ ] At least 3 configured containers in preferences.yaml.
- [ ] At least one configured container that is initially stopped.
- [ ] At least one configured container that is initially running.
- [ ] At least one configured container that does not exist in Docker, to test error handling.
- [ ] Make sure you know how to stop/start Docker manually on your system.
- [ ] Make sure you know how to make the config directory temporarily unwritable on your system (for test 8.5).

For tests involving Docker being unavailable, don't uninstall Docker. Stop the Docker daemon/service and restore it afterward.

---

## 1. Normal application startup

### 1.1 Start with valid configuration

Precondition: Valid config containing multiple containers.

- [ ] Start the application.
- [ ] MainWindow appears normally.
- [ ] All configured containers appear in ContainerListWidget, initially in Loading state.
- [ ] Their display names are correct.
- [ ] Their states resolve to correct values shortly after startup.
- [ ] No auto-refresh warning is visible.
- [ ] No unexpected error/warning dialogs appear.
- [ ] The application remains responsive.

### 1.2 Initial refresh

- [ ] Start the application with Docker running.
- [ ] Observe the UI immediately after startup.
- [ ] Loading indicator appears while the initial refresh is running.
- [ ] Loading indicator disappears after the container state is received.
- [ ] Container states match `docker ps`.
- [ ] No loading indicator remains indefinitely.

---

## 2. Manual refresh

### 2.1 Successful refresh

- [ ] Start application normally.
- [ ] Click Refresh.
- [ ] Loading indicator appears; ThrottledButton disables itself.
- [ ] Container state is updated.
- [ ] Loading indicator disappears and the button re-enables as soon as the update arrives (should be well before the ~10s cooldown in the normal case).
- [ ] UI remains responsive.

### 2.2 Refresh while nothing changed

- [ ] Click Refresh when all container states are already correct.
- [ ] Loading indicator still behaves correctly.
- [ ] No visual glitches occur.
- [ ] No duplicate rows appear.
- [ ] No rows disappear unexpectedly.

### 2.3 Multiple refreshes

- [ ] Click Refresh repeatedly.
- [ ] Verify ThrottledButton stays disabled/throttled between the click and either (a) ContainerService's response or (b) the internal cooldown elapsing, whichever is first.
- [ ] Verify the button cannot be spammed during the cooldown.
- [ ] Verify the UI doesn't get stuck loading past the cooldown even if Docker never responds (simulate by stopping Docker — see section 7.2).
- [ ] Verify the final displayed state is correct.
- [ ] Verify the button becomes available again afterward.

---

## 3. Start container

Use a container that is initially stopped.

### 3.1 Successful start

- [ ] Confirm the container is stopped.
- [ ] Click Start.
- [ ] UI immediately changes to the expected "Starting..." state.
- [ ] Docker actually starts the container.
- [ ] DockerEventStream receives the corresponding event.
- [ ] Authoritative state arrives and state becomes "Started"/Running.
- [ ] Container remains displayed as running.

### 3.2 Start failure

Use a container that cannot successfully start, if possible.
_PS. Treat this test as a hypothetical error path._

- [ ] Click Start.
- [ ] Optimistic "Starting" state appears.
- [ ] Operation fails (container remains Stopped in Docker).
- [ ] The displayed state is eventually corrected once ContainerService's normal update paths (auto-refresh/DockerEventStream, or the next bulk update) deliver the real state. There is no MainWindow-side timeout for this — don't expect the row to correct itself instantly.
- [ ] If it doesn't seem to correct on its own within a reasonable time, trigger a manual Refresh and confirm the state resolves.

> Failure of this test case (i.e. even manual Refresh doesn't fix it) suggests an issue in the Docker event stream/refresh logic, not in MainWindow.

---

## 4. Stop container

Use a running container.

### 4.1 Successful stop

- [ ] Confirm the container is running.
- [ ] Click Stop.
- [ ] UI immediately changes to the expected "Stopping..." state.
- [ ] Docker stops the container.
- [ ] DockerEventStream receives the event.
- [ ] Authoritative state is received.
- [ ] Container remains displayed as stopped.

### 4.2 Stop failure

- [ ] Trigger a situation where stop cannot succeed.
- [ ] Click Stop.
- [ ] Optimistic "Stopping" state appears.
- [ ] Operation fails.
- [ ] UI eventually reflects the real Docker state via ContainerService's normal update paths (same as 3.2 — no dedicated MainWindow timeout). Use manual Refresh if it doesn't seem to resolve.

---

## 5. Container admin action

- [ ] Click Admin for a valid container.
- [ ] Correct ContainerService operation is invoked.
- [ ] Correct container is targeted.
- [ ] UI remains responsive.

If Admin opens a terminal/browser/other application:

- [ ] The expected external application opens.
- [ ] Closing Simple Container Manager doesn't affect that application.

---

## 6. Automatic refresh / DockerEventStream

This is probably the most important integration section.

### 6.1 Automatic state change

- [ ] Start the application.
- [ ] Listen to Docker events separately.
- [ ] Ensure automatic refresh is working (no auto-refresh warning visible).
- [ ] Change a container's state outside Simple Container Manager.
- [ ] Don't click Refresh.
- [ ] Wait for the Docker event.
- [ ] UI updates automatically.
- [ ] Correct row changes.
- [ ] No manual refresh is necessary.

### 6.2 Restart externally

- [ ] Restart a configured container using the Docker CLI.
- [ ] Observe the UI during the state transitions.
- [ ] Ensure the final state is correct.
- [ ] Ensure the UI doesn't become stuck in an unconfirmed state.

---

## 7. Docker unavailable

### 7.1 Application starts while Docker is unavailable

Precondition: Stop the Docker daemon.

- [ ] Start Simple Container Manager.
- [ ] Application still starts.
- [ ] Configured containers still appear (initially Loading, then resolving to NotFound/unavailable as appropriate).
- [ ] Application does not crash.
- [ ] Application does not hang waiting for Docker.
- [ ] `lblAutoRefreshWarning` is shown to indicate DockerEventStream is unavailable — this is the same widget used later for "automatic refresh failed," not a separate indicator.

This is particularly important because configuration availability and Docker availability are separate concerns.

### 7.2 Manual refresh while Docker is unavailable

- [ ] Docker is stopped.
- [ ] Start application.
- [ ] Click Refresh.
- [ ] Loading indicator appears; button throttles.
- [ ] Docker operation fails.
- [ ] Loading indicator disappears once ThrottledButton's cooldown elapses (since no successful response will ever arrive) — confirm this happens automatically without user intervention.
- [ ] Application remains responsive.
- [ ] Error/failure is communicated appropriately.
- [ ] UI doesn't pretend that a Docker state was successfully confirmed.

### 7.3 Docker disappears while application is running

Precondition: Application is running normally.

- [ ] Start application.
- [ ] Confirm automatic refresh is working.
- [ ] Stop Docker externally.
- [ ] Wait for DockerEventStream to detect failure.
- [ ] `lblAutoRefreshWarning` becomes visible.
- [ ] Warning doesn't block interaction.
- [ ] Application remains responsive.
- [ ] Existing container information doesn't suddenly get destroyed unnecessarily.

---

## 8. Configuration tests

### 8.1 Empty configuration (explicit)

Use:

```
containers: []
```

- [ ] Start application.
- [ ] Application opens normally.
- [ ] Welcoming/empty-state screen is shown.
- [ ] No error screen or dialog appears.
- [ ] Container list is empty.
- [ ] Refresh is disabled.
- [ ] Container actions are unavailable.
- [ ] Application remains usable.
- [ ] Exit works normally.

### 8.2 Invalid configuration (malformed existing file)

Break the YAML syntax of an existing config file.

- [ ] Start application.
- [ ] AppPreferencesReader detects the failure.
- [ ] MainWindow is not constructed.
- [ ] The error-state screen (not a blocking modal dialog) is shown, explaining the problem and how to fix it.
- [ ] Docker-related services (DockerCLI, DockerEventStream, ContainerService) are not started.
- [ ] The user can close the application normally (window close/Exit affordance on the error screen) once acknowledged — the app does not force-quit on its own.

### 8.3 Missing configuration (no file present)

Temporarily rename/remove the configuration file.

- [ ] Start application.
- [ ] Verify AppPreferencesReader generated a new default (empty) configuration file on disk.
- [ ] This is treated as success, not an error — MainWindow is constructed normally.
- [ ] Welcoming/empty-state screen appears (same as 8.1), not the error screen.
- [ ] No crash.
- [ ] No misleading "Docker unavailable" message tied to this — the config path and Docker path are independent.

### 8.4 Invalid container definition

Configure a container that doesn't exist, e.g.:

```
containers:
  - name: definitely-not-a-real-container
```

- [ ] Application starts.
- [ ] The configured container appears (Loading, then resolving).
- [ ] Refresh attempts to resolve its state.
- [ ] Status is shown as "Not Found."
- [ ] Other valid containers continue working.
- [ ] One invalid configuration entry doesn't crash the entire UI.

### 8.5 Default configuration cannot be written

Simulate this by making the configuration directory temporarily unwritable (permissions), then removing/renaming the config file so a default must be generated.

- [ ] Start application.
- [ ] AppPreferencesReader attempts to write a default config and fails.
- [ ] This is treated as a real error (same path as 8.2).
- [ ] MainWindow is not constructed.
- [ ] Error-state screen is shown followed by the welcome screen.
- [ ] Docker-related services are not started.
- [ ] Restore directory permissions afterward.

---

## 9. UI consistency tests

### 9.1 State correctness

For every configured container:

- [ ] UI state matches Docker state.
- [ ] Display name remains correct.
- [ ] Ports remain correct.
- [ ] PID remains correct where applicable.
- [ ] No unrelated row changes when another container changes.

### 9.2 Repeated operations

Try Stop → Start → Stop → Start quickly.

- [ ] No crash.
- [ ] No duplicate rows.
- [ ] No stale pending indicators.
- [ ] Final UI matches Docker.
- [ ] Note any instance where a stale in-flight bulk update will clobber an optimistic state. This is intentional.

---

## 10. Exit / shutdown

### 10.1 Normal exit

- [ ] Click Exit.
- [ ] Application closes cleanly.
- [ ] No dialog appears unexpectedly.
- [ ] No process remains running afterward.
- [ ] Application terminates cleanly.
- [ ] No lingering Docker event process remains (check with `ps aux | grep docker` — verify you're not accidentally catching unrelated Docker processes).

### 10.2 Exit while refreshing

- [ ] Click Refresh.
- [ ] Immediately click Exit.
- [ ] Application closes cleanly.
- [ ] No crash.
- [ ] No zombie docker processes remain (`ps aux | grep docker` after close).

---

## 11. Startup dependency failure

If possible, simulate failures in the configuration-loading stage — e.g. make the configuration file unreadable (permissions), or malform it (covered by 8.2), or block writing a new default (covered by 8.5).

- [ ] Application doesn't crash.
- [ ] AppPreferencesReader reports failure.
- [ ] main() handles the failure per the spec above.
- [ ] MainWindow isn't created.
- [ ] User gets the error-state screen (not a blocking dialog).
- [ ] No Docker services are unnecessarily started.
- [ ] Application can be closed normally.
