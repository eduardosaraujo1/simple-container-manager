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

    bool hasErrors = false;
    // Parse stdout into string in order to remove newline characters and parse each individually
    const QString output = QString::fromUtf8(proc.readAllStandardOutput());
    const QStringList lines = output.split("\n", Qt::SkipEmptyParts);
    QList<ContainerInfo> finalResult;

    for (const QString &line : lines) {
        bool parseSuccess;
        ContainerInfo container = parseContainerInfoString(line, parseSuccess);

        if (parseSuccess) {
            finalResult.append(container);
        } else {
            hasErrors = true;
        }
    }

    if (hasErrors) {
        // I have considered emitting the offending string along side this signal
        // However, the logging layer (qWarning()) should already take care of observability
        // and the consumer does not care about how many errors haappened
        emit this->parseErrorOccurred();
    }

    emit this->containersUpdated(finalResult);
}

const ContainerInfo DockerCLI::parseContainerInfoString(const QString &str, bool &success) {
    success = true;

    // Parse using Qt's String parser
    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &jsonParseError);
    QJsonObject obj;

    if (jsonParseError.error != QJsonParseError::NoError) {
        success = false;
        qWarning() << "parseContainerInfoString: Error when parsing JSON output. String: \n" %
                   str % "\nError:" %
                   jsonParseError.errorString();
        return ContainerInfo{};
    }

    // Ensure parsed JSON is an object
    obj = doc.object();
    if (obj.isEmpty()) {
        success = false;
        qWarning() << "parseContainerInfoString: JSON parsed succcessfilly but not as valid object. \nString: " %
                   str;
        return ContainerInfo{};
    }

    // Ensure object has required fields
    if (! (obj.contains("id")
          && obj.contains("name")
          && obj.contains("status")
          )) {
        success = false;
        qWarning() << "parseContainerInfoString: JSON parsed succcessfilly but does not have the required keys. \nString: " %
                   str;
        return ContainerInfo{};
    }

    const ContainerInfo::Status status
        = ContainerInfo::statusFromString(obj.value("status").toString());

    if (status == ContainerInfo::Status::Unknown) {
        qWarning() << "parseContainerInfoString: Status could not be correctly identified. Received '"
                          % obj.value("status").toString()
                          % "'";
    }

    return ContainerInfo{
        obj.value("id").toString(),
        obj.value("name").toString(),
        status
    };
}