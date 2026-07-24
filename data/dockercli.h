#ifndef DOCKERCLI_H
#define DOCKERCLI_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include "data-objects/containerinfo.h"

class DockerCLI : public QObject
{
    Q_OBJECT
public:
    explicit DockerCLI(QObject *parent = nullptr);
    ~DockerCLI();
    /**
     * @brief Starts the container read process and notifies the result
     *
     * Once the result is available, the signal `containersUpdated` is emitted.
     */
    void requestContainerRefresh(const QStringList &namesFilter);
public slots:
    void startContainer(const QString &containerId);
    void stopContainer(const QString &containerId);
private:
    QProcess m_proc{this};
private slots:
    void onProcessDone(int exitCode, QProcess::ExitStatus status);
signals:
    void containersUpdated(const QList<ContainerInfo> &containers);
    void parseErrorOccurred();
};

#endif // DOCKERCLI_H
