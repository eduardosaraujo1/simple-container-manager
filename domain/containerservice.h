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
 * higher-level API for container operations and state synchronization.
 *
 * It owns all container-related workflows, timing, automatic refresh
 * behavior and Docker interactions, so that presentation code (e.g.
 * MainWindow) never has to talk to DockerCLI or DockerEventStream
 * directly.
 */
class ContainerService : public QObject
{
    Q_OBJECT
public:
    explicit ContainerService(QObject *parent = nullptr);
    ~ContainerService();

    /**
     * @brief Configures the service with the containers it should track.
     *
     * Must be called once, before start()/refreshContainers() are used.
     */
    void initialize(const QList<AppPreferences::ContainerSpec> &containers);

    [[nodiscard]] bool isInitialized() const { return m_initialized; }

public slots:
    /** @brief Starts the Docker event stream and performs an initial refresh. */
    void start();
    /** @brief Stops the Docker event stream. */
    void stop();

    /** @brief Requests an up-to-date list of tracked containers from Docker. */
    void refreshContainers();

    /** @brief Starts the given (tracked) container. */
    void startContainer(const QString &containerName);
    /** @brief Stops the given (tracked) container. */
    void stopContainer(const QString &containerName);

signals:
    /** @brief Emitted whenever a fresh snapshot of tracked containers is available. */
    void containersUpdated(const QList<ContainerInfo> &containers);
    /** @brief Emitted when Docker interaction fails (CLI parse error, event stream error, etc.). */
    void errorOccurred(const QString &message);

private slots:
    void onCliContainersUpdated(const QList<ContainerInfo> &containers);
    void onCliParseErrorOccurred();

    void onDockerEvent(const DockerEvent &event);
    void onEventStreamError(DockerEventStream::StreamError error);

private:
    [[nodiscard]] QString containerIdForName(const QString &containerName) const;

    DockerCLI m_cli;
    DockerEventStream m_eventStream;

    bool m_initialized = false;
    QStringList m_containerNames;
    QHash<QString, QString> m_nameToId; // populated as refreshes come in
};

#endif // CONTAINERSERVICE_H