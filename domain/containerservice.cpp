#include "containerservice.h"

#include <QDebug>
#include <QProcess>
#include <QSet>

#include <data/data-objects/containerdefinition.h>

ContainerService::ContainerService(
    DockerCLI &cli,
    DockerEventStream &stream,
    ConfiguredContainers &containers,
    QObject *parent)
    : QObject(parent), m_cli(&cli), m_eventStream(&stream), m_containerConfig(&containers)
{
    connect(m_cli, &DockerCLI::containersUpdated,
            this, &ContainerService::onCliContainersUpdated);

    connect(m_eventStream, &DockerEventStream::eventReceived,
            this, &ContainerService::onDockerEvent);
    connect(m_eventStream, &DockerEventStream::started,
            this, &ContainerService::onEventStreamStart);
    connect(m_eventStream, &DockerEventStream::stopped,
            this, &ContainerService::onEventStreamStop);

    if (!m_eventStream->isRunning())
    {
        // TODO: add a dedicated class to retry if the eventStream unexpectedly dies
        m_eventStream->start();
    }
}

ContainerService::~ContainerService() = default;

bool ContainerService::isEventStreamRunning() const
{
    return m_eventStream->isRunning();
}

void ContainerService::refreshContainers()
{
    // Only ask Docker about containers the user actually configured;
    // anything else running on the host is none of our business.
    const QSet<QString> names = m_containerConfig->containerNames();
    const QStringList namesFilter(names.begin(), names.end());

    m_cli->requestContainerRefresh(namesFilter);
}

void ContainerService::startContainer(const QString &containerName)
{
    m_cli->startContainer(containerName);
}

void ContainerService::stopContainer(const QString &containerName)
{
    m_cli->stopContainer(containerName);
}

void ContainerService::runAction(const QString &containerName)
{
    const QHash<QString, ContainerDefinition> &containers = m_containerConfig->containers();
    const auto it = containers.constFind(containerName);

    if (it == containers.constEnd())
    {
        qWarning() << "[ContainerService Action Runner] no configured container named" << containerName;
        return;
    }

    const std::optional<QString> action = it->action();

    if (!action || action->isEmpty())
    {
        return;
    }

    // The action is a single command-line string (e.g. "xterm -e bash"),
    // so it needs to be split into a program and its arguments before
    // QProcess can launch it.
    const QStringList parts = QProcess::splitCommand(*action);

    if (parts.isEmpty())
    {
        qWarning() << "[ContainerService Action Runner] could not parse action command for" << containerName
                   << ":" << *action;
        return;
    }

    const QString program = parts.first();
    const QStringList arguments = parts.mid(1);

    // startDetached() launches the process fully independent of this
    // application: no QProcess object to keep alive, no parent-child
    // relationship, so it survives the app closing.
    if (!QProcess::startDetached(program, arguments))
    {
        qWarning() << "[ContainerService Action Runner] failed to start action for" << containerName;
    }
}

void ContainerService::onCliContainersUpdated(const QList<ContainerState> &containers)
{
    emit containersUpdated(containers);
}

void ContainerService::onDockerEvent(const DockerEvent &event)
{
    Q_UNUSED(event);

    refreshContainers();
}

void ContainerService::onEventStreamStart()
{
    emit streamStatusUpdated(true);
}

void ContainerService::onEventStreamStop()
{
    emit streamStatusUpdated(false);
}