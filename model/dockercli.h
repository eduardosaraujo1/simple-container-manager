#ifndef DOCKERCLI_H
#define DOCKERCLI_H

#include <QObject>
#include <QProcess>
#include <QStringList>
#include <model/data-objects/containerinfo.h>

class DockerCLI : public QObject
{
    Q_OBJECT
public:
private:
    QProcess proc;

    // I considered making `success` an enum or a proper `Exception` here. However,
    // decided against it because if an error happens here it should go to the
    // logger. After that, the application does not care what went wrong, it should just display
    // error message to the user guiding him to send the logs.
    static const ContainerInfo parseContainerInfoString(const QString &str, bool &success);
public:
    explicit DockerCLI(QObject *parent = nullptr);

    ~DockerCLI();

    void requestContainerRefresh(QStringList namesFilter);
signals:
    void containersUpdated(QList<ContainerInfo> containers);

    void containerParseError(const QString &offendingJsonLine);
private slots:
    // Runs when the
    void onProcessDone(int exitCode, QProcess::ExitStatus status);
};

#endif // DOCKERCLI_H
