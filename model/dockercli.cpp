#include "dockercli.h"

DockerCLI::DockerCLI(QObject *parent) {
    QObject::connect(&proc, &QProcess::finished, this, &DockerCLI::onProcessDone);
}

void DockerCLI::requestContainerRefresh(QStringList namesFilter) {
    if (proc.state() == QProcess::Running) {
        // Fetch is (probably) already in progress
        return;
    }

    // Prepare arguments

    // Start process
}

void DockerCLI::onProcessDone(int exitCode, QProcess::ExitStatus status) {
}