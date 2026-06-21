#include "dockercli.h"
#include <QByteArray>
#include <QString>
#include <QJsonObject>
#include <QDebug>
#include <QJsonDocument>
#include <optional>

namespace {
    std::optional<ContainerInfo> parseContainerInfo(const QByteArray &rawData) {
        QJsonParseError jsonParseError;
        const QJsonDocument doc = QJsonDocument::fromJson(rawData, &jsonParseError);

        if (jsonParseError.error != QJsonParseError::NoError) {
            qWarning() << "parseJsonObject: Error when parsing JSON output. Raw Data: \n"
                       << rawData << "\nError:"
                       << jsonParseError.errorString();
            return std::nullopt;
        }

        if (! doc.isObject()) {
            qWarning() << "parseJsonObject: JSON parsed succcessfully, but not as valid object. Raw Data: \n"
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
            qWarning() << "parseContainerInfoString: JSON parsed succcessfully but does not have the required keys. Raw Data:\n"
                       << rawData;
            return std::nullopt;
        }

        const ContainerInfo::Status statusEnum
            = ContainerInfo::statusFromString(status.toString());

        if (statusEnum == ContainerInfo::Status::Unknown) {
            qWarning() << "parseContainerInfoString: Status \""
                       << status.toString() << "\" could not be identified. Defaulting to Status::Unknown";
        }

        return ContainerInfo{
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
        qWarning() << "requestContainerRefresh: previous query is still running. Dropping request.";
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
        qCritical() << "onProcessDone: unexpected error occurred when querying containers. Exit code:" << exitCode
                   << ".\nError output:\n" << m_proc.readAllStandardError();
        return;
    }

    const QByteArray output = m_proc.readAllStandardOutput();
    const QList<QByteArray> lines = output.split('\n');

    bool hasErrors = false;
    QList<ContainerInfo> finalResult;

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
        // I have considered emitting the offending string along side this signal
        // However, the logging layer (qWarning()) should already take care of observability
        // and the consumer does not care about how many errors happened
        emit this->parseErrorOccurred();
    }

    emit this->containersUpdated(finalResult);
}
