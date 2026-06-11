#include "dockercli.h"
#include <QDebug>
#include <QJsonDocument>

DockerCLI::DockerCLI(QObject *parent) : QObject(parent) {
    QObject::connect(&proc, &QProcess::finished, this, &DockerCLI::onProcessDone);
}

DockerCLI::~DockerCLI() {
    if (proc.state() == QProcess::Running) {
        proc.terminate();

        if (! proc.waitForFinished(3000)) {
            proc.kill();
        }
    }
}

void DockerCLI::requestContainerRefresh(QStringList namesFilter) {
    QStringList arguments{
        "ps",
        "-a",
        "--no-trunc",
        "--format",
        "{\"id\":{{json .ID}},\"name\":{{json .Names}},\"status\":{{json .State}}}"
    };

    if (proc.state() != QProcess::NotRunning) {
        qInfo() << "Docker Containers query is already running: ignoring last request.";
        return;
    }

    for (const QString &name : namesFilter) {
        arguments << "--filter";
        arguments << "name="%name;
    }

    proc.start("docker", arguments);
}

void DockerCLI::onProcessDone(int exitCode, QProcess::ExitStatus status) {
    if (exitCode != 0 || status == QProcess::ExitStatus::CrashExit) {
        qWarning() << "Ocorreu um erro desconhecido ao listar os containers. Exit code:" << exitCode
                   << ".\nError output:\n" << proc.readAllStandardError();
        return;
    }

    const QString output = QString::fromUtf8(proc.readAllStandardOutput());
    const QStringList lines = output.split("\n", Qt::SkipEmptyParts);
    QList<ContainerInfo> finalResult;

    for (const QString &line : lines) {
        bool modelParseSuccess;
        QJsonParseError jsonParseError;
        QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &jsonParseError);

        if (! doc.isObject() || jsonParseError.error != QJsonParseError::NoError) {
            qWarning() << "Ocorreu um erro ao interpretar o resultado JSON: "
                       << jsonParseError.errorString();
            continue;
        }

        finalResult.append(ContainerInfo::fromJsonObject(doc.object(), modelParseSuccess));
    }

    // Emit signal
    emit this->containersUpdated(finalResult);
}