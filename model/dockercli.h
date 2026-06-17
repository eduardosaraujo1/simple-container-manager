#ifndef DOCKERCLI_H
#define DOCKERCLI_H

#include <QObject>
#include <QProcess>
#include <QStringList>
#include <QString>
#include <QStringView>
#include <QByteArray>
#include <optional>
#include <model/data-objects/containerinfo.h>

class DockerCLI : public QObject
{
    Q_OBJECT
public:
    explicit DockerCLI(QObject *parent = nullptr);
    ~DockerCLI();
    void requestContainerRefresh(const QStringList &namesFilter);
private:
    QProcess m_proc{this};
    static std::optional<ContainerInfo> parseContainerInfo(const QByteArray &rawData);
private slots:
    void onProcessDone(int exitCode, QProcess::ExitStatus status);
signals:
    void containersUpdated(const QList<ContainerInfo> &containers);
    void parseErrorOccurred();
};

#endif // DOCKERCLI_H
