#include "dockereventstream.h"

DockerEventStream::DockerEventStream(QObject *parent)
    : QObject{parent}
{
    // Connect onStartError to QProcess errorOccured

    // Connect onUnexpectedError to QProcess finished

    // Connect onErrorMessage to QProcess ready stderr output

    // Connect onEventDetected to QProcess ready stdout output

    // Run restart to initiate process
}

DockerEventStream::~DockerEventStream() {
    m_attempt_restart = false;
    stop();
}

bool DockerEventStream::isActive(){
    // Check QProcess to see if it's active
}

void DockerEventStream::onStartError(QProcess::ProcessError error) {
    // (TODO: move to dedicated function to share with onUnexpectedError)
    // Increment the fail counter (if it's less than 0 then set it to 1)
    // If it's less than or equal to 3 and attempt_restart is true, try calling .restart() and log the attempt
    // Else set attempt_restart to false, log and emit criticalStreamError
}

void DockerEventStream::onUnexpectedError(int exitCode, QProcess::ExitStatus exitStatus) {
    // (TODO: move to dedicated function to share with onStartError)
    // Increment the fail counter (if it's less than 0 then set it to 1)
    // If it's less than or equal to 3 and attempt_restart is true, try calling .restart() and log the attempt
    // Else set attempt_restart to false, log and emit criticalStreamError
}

void DockerEventStream::onErrorMessage() {
    // Read std error

    // Log error

    // Won't attempt to restart anything
}

void DockerEventStream::onEventDetected() {
    // Read standard output

    // Parse JSON

    // If it fails (always log) and consecutive error count is greater than 3, set attempt_restart to false and emit critical error
    // Otherwise, ignore missed event and early return.

    // If success, reset error count (if a parse was successful we're sure it's healthy) and store parsed value

    // Emit eventReceived with parsed value
}

void DockerEventStream::stop() {
    m_attempt_restart = false;

    if (proc.state() == QProcess::Running) {
        proc.terminate();

        if (! proc.waitForFinished(3000)) {
            proc.kill();
        }
    }
}

void DockerEventStream::restart() {
    // Stop process if it's running

    // Set keepAlive to true

    // Run proper docker events command

    // // Triggers auto restart behavior, check onUnexpectedError and onStartError
}