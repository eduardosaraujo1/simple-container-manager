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

/**
 * @brief Start container read process; emits containersUpdated signal when read is finished.
 * @param namesFilter Filters which containers should be used by name
 */
void DockerCLI::requestContainerRefresh(const QStringList &namesFilter) {
    QStringList arguments{
        "ps",
        "-a",
        "--no-trunc",
        "--format",
        "{\"id\":{{json .ID}},\"name\":{{json .Names}},\"status\":{{json .State}}}"
    };

    if (proc.state() != QProcess::NotRunning) {
        qWarning() << "requestContainerRefresh: previous query is still running. Dropping request.";
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
        qCritical() << "onProcessDone: unexpected error occurred when querying containers. Exit code:" << exitCode
                   << ".\nError output:\n" << proc.readAllStandardError();
        return;
    }

    // Parse stdout into string in order to remove newline characters and parse each individually
    const QByteArray output = proc.readAllStandardOutput();
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

std::optional<ContainerInfo> DockerCLI::parseContainerInfo(const QByteArray &rawData) {

    QJsonObject obj;
    {
        auto parseResult = DockerCLI::parseJsonObject(rawData);
        if (! parseResult) return std::nullopt;
        obj = std::move(*parseResult);
    }

    if (! (obj.contains("id")
          && obj.contains("name")
          && obj.contains("status")
          )) {
        qWarning() << "parseContainerInfoString: JSON parsed succcessfilly but does not have the required keys. Raw Data:\n"
                   << rawData;
        return std::nullopt;
    }

    const ContainerInfo::Status status
        = ContainerInfo::statusFromString(obj.value("status").toString());

    if (status == ContainerInfo::Status::Unknown) {
        qWarning() << "parseContainerInfoString: Status \""
                   << obj.value("status").toString() << "\" could not be identified. Defaulting to Status::Unknown";
    }

    return ContainerInfo{
        obj.value("id").toString(),
        obj.value("name").toString(),
        status
    };
}

std::optional<QJsonObject> DockerCLI::parseJsonObject(const QByteArray &rawData) {
    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawData, &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError) {
        qWarning() << "parseJsonObject: Error when parsing JSON output. Raw Data: \n"
                   << rawData << "\nError:"
                   << jsonParseError.errorString();
        return std::nullopt;
    }

    if (! doc.isObject()) {
        qWarning() << "parseJsonObject: JSON parsed succcessfilly, but not as valid object. Raw Data: \n"
                   << rawData;
        return std::nullopt;
    }

    return doc.object();
}