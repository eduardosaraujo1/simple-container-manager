#include "dockercli.h"
#include <QByteArray>
#include <QString>
#include <QJsonObject>
#include <QDebug>
#include <QJsonDocument>
#include <optional>

namespace {
    std::optional<ContainerState> parseContainerInfo(const QByteArray &rawData) {
        QJsonParseError jsonParseError;
        const QJsonDocument doc = QJsonDocument::fromJson(rawData, &jsonParseError);

        if (jsonParseError.error != QJsonParseError::NoError) {
            qWarning() << "[DockerCLI Json Parser] Error when parsing JSON output. Raw Data: \n"
                       << rawData << "\nError:"
                       << jsonParseError.errorString();
            return std::nullopt;
        }

        if (! doc.isObject()) {
            qWarning() << "[DockerCLI Json Parser] JSON parsed succcessfully, but not as valid object. Raw Data: \n"
                       << rawData;
            return std::nullopt;
        }

        const QJsonObject obj = doc.object();
        const QJsonValue id = obj.value("id");
        const QJsonValue name = obj.value("name");
        const QJsonValue status = obj.value("status");

        if (
            id.isUndefined()
            || name.isUndefined()
            || status.isUndefined()
            ) {
            qWarning() << "[DockerCLI Json Parser] JSON parsed succcessfully but does not have the required keys. Raw Data:\n"
                       << rawData;
            return std::nullopt;
        }

        const ContainerState::Status statusEnum
            = ContainerState::statusFromString(status.toString());

        if (statusEnum == ContainerState::Status::Unknown) {
            qWarning() << "[DockerCLI Json Parser] Status \""
                       << status.toString() << "\" could not be identified. Defaulting to Status::Unknown";
        }

        return ContainerState{
            id.toString(),
            name.toString(),
            statusEnum
        };
    }
}

DockerCLI::DockerCLI(QObject *parent) : QObject(parent) {
    QObject::connect(&m_proc, &QProcess::finished, this, &DockerCLI::onProcessDone);
}

DockerCLI::~DockerCLI() {
    if (m_proc.state() == QProcess::Running) {
        m_proc.terminate();

        if (! m_proc.waitForFinished(3000)) {
            m_proc.kill();
        }
    }
}

void DockerCLI::requestContainerRefresh(const QStringList &namesFilter) {
    QStringList arguments{
        "ps",
        "-a",
        "--no-trunc",
        "--format",
        "{\"id\":{{json .ID}},\"name\":{{json .Names}},\"status\":{{json .State}}}"
    };

    if (m_proc.state() != QProcess::NotRunning) {
        qWarning() << "[DockerCLI] previous query is still running. Dropping request.";
        return;
    }

    for (const QString &name : namesFilter) {
        arguments << "--filter";
        arguments << "name="%name;
    }

    m_proc.start("docker", arguments);
}

void DockerCLI::onProcessDone(int exitCode, QProcess::ExitStatus status) {
    if (exitCode != 0 || status == QProcess::ExitStatus::CrashExit) {
        qCritical() << "[DockerCLI] unexpected error occurred when querying containers. Exit code:" << exitCode
                   << ".\nError output:\n" << m_proc.readAllStandardError();
        return;
    }

    const QByteArray output = m_proc.readAllStandardOutput();
    const QList<QByteArray> lines = output.split('\n');

    bool hasErrors = false;
    QList<ContainerState> finalResult;

    for (const QByteArray &line : lines) {
        if (line.isEmpty()) continue;

        auto container = parseContainerInfo(line);

        if (container) {
            finalResult.append(*container);
        } else {
            hasErrors = true;
        }
    }

    if (hasErrors) {
        emit parseErrorOccurred();
    }

    emit this->containersUpdated(finalResult);
}

void DockerCLI::runToggleCommand(const QString &containerId, bool isStartCommand)
{
    QProcess *process = new QProcess(this);

    QObject::connect(process, &QProcess::finished, this,
                     [&](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode != 0 || exitStatus == QProcess::ExitStatus::CrashExit) {
            qCritical() << "[DockerCLI] an unexpected error occurred" <<
                (isStartCommand ? "starting" : "stopping") << "docker process.\nExit code:"
                        << exitCode << ".\n"
                        << "Error output:\n" << process->readAllStandardError() << ".\n"
                        << "Standard output:\n" << process->readAllStandardOutput() << ".\n";
        }
        process->deleteLater();
    });

    QObject::connect(process, &QProcess::errorOccurred, this, [&](QProcess::ProcessError)
    {
        qCritical() << "[DockerCLI] Unexpected error" << (isStartCommand ? "starting" : "stopping")
                    << "Docker process:" << process->errorString();
        process->deleteLater();
    });

    process->start("docker", {
        isStartCommand ? "start" : "stop",
        containerId
    });

    process->waitForFinished();
}

void DockerCLI::startContainer(const QString &containerId) {
    return runToggleCommand(containerId, true);
}

void DockerCLI::stopContainer(const QString &containerId) {
    return runToggleCommand(containerId, false);
}
