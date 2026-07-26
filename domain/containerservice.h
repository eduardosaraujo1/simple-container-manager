#ifndef CONTAINERSERVICE_H
#define CONTAINERSERVICE_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QStringList>

#include <data/apppreferences.h>
#include <data/data-objects/containerinfo.h>
#include <data/data-objects/dockerevent.h>
#include <data/dockercli.h>
#include <data/dockereventstream.h>

/**
 * @brief The ContainerService class encapsulates all Docker-related
 * infrastructure (DockerCLI + DockerEventStream) and exposes a
 * higher-level API for container operations, state synchronization,
 * and running user-defined container actions.
 *
 * It owns all container-related workflows, timing, automatic refresh
 * behavior and Docker interactions, so that presentation code (e.g.
 * MainWindow) never has to talk to DockerCLI, DockerEventStream, or
 * QProcess directly.
 */
class ContainerService : public QObject
{
    Q_OBJECT
public:
    explicit ContainerService(
        DockerCLI &cli,
        DockerEventStream &str,
        AppPreferences &prefs,
        QObject *parent = nullptr);
    ~ContainerService();

    /**
     * @brief Ensures the components responsible auto container status refresh
     * are up.
     */
    [[nodiscard]] bool isAutoRefreshEnabled() const;

public slots:
    /** @brief Requests an up-to-date list of tracked containers from Docker. */
    void refreshContainers();

    /** @brief Starts the given container. */
    void startContainer(const QString &containerName);
    /** @brief Stops the given container. */
    void stopContainer(const QString &containerName);

    /**
     * @brief Runs the configured action command for the given container.
     *
     * Looks up containerName's ContainerSpec, and if it has a non-empty
     * action, spawns it as a detached QProcess (disowned from the app so
     * it keeps running independently of MainWindow's lifetime). No-ops if
     * the container is unknown or has no configured action.
     */
    void runAction(const QString &containerName);

signals:
    /** @brief Emitted whenever a fresh snapshot of tracked containers is available. */
    void containersUpdated(const QList<ContainerInfo> &containers);
    /** @brief Emitted when Docker Stream, responsible for automatically updating containers, fails */
    void streamErrorOccurred(DockerEventStream::StreamError error);

private slots:
    void onCliContainersUpdated(const QList<ContainerInfo> &containers);

    void onDockerEvent(const DockerEvent &event);
    void onEventStreamError(DockerEventStream::StreamError error);

private:
    DockerCLI *m_cli;
    DockerEventStream *m_eventStream;
    AppPreferences *m_appPrefs;

    QHash<QString, AppPreferences::ContainerSpec> m_containers; // name -> spec
};

#endif // CONTAINERSERVICE_H