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
    : QObject(parent), m_cli(cli), m_eventStream(stream), m_containerConfig(containers),
    m_streamSupervisor(stream, this)
{
    connect(&m_cli, &DockerCLI::containersUpdated,
            this, &ContainerService::onCliContainersUpdated);

    connect(&m_eventStream, &DockerEventStream::eventReceived,
            this, &ContainerService::onDockerEvent);
    connect(&m_streamSupervisor, &DockerEventStreamSupervisor::maxConsecutiveFailuresReached,
            this, &ContainerService::onCriticalStreamFailure);

    m_streamSupervisor.keepAlive();
}

ContainerService::~ContainerService() = default;

bool ContainerService::isAutoRefreshUp() const
{
    return m_eventStream.isRunning();
}

const ConfiguredContainers &ContainerService::containers() const
{
    return m_containerConfig;
}

void ContainerService::refreshContainers()
{
    // Only ask Docker about containers the user actually configured;
    // anything else running on the host is none of our business.
    const QStringList namesFilter = m_containerConfig.containerNames();

    m_cli.requestContainerRefresh(namesFilter);
}

void ContainerService::startContainer(const QString &containerName)
{
    m_cli.startContainer(containerName);
}

void ContainerService::stopContainer(const QString &containerName)
{
    m_cli.stopContainer(containerName);
}

void ContainerService::runAction(const QString &containerName)
{
    const ContainerDefinition *container = m_containerConfig.findByName(containerName);

    if (!container)
    {
        qWarning() << "[ContainerService Action Runner] no configured container named" << containerName;
        return;
    }

    const QString action = container->action();

    if (action.isEmpty())
    {
        qWarning() << "[ContainerService Action Runner] Attempt to run action for "<< containerName <<". This suggests a UX/UI problem.";
        return;
    }

    // The action is a single command-line string (e.g. "xterm -e bash"),
    // so it needs to be split into a program and its arguments before
    // QProcess can launch it.
    const QStringList parts = QProcess::splitCommand(action);

    if (parts.isEmpty())
    {
        qWarning() << "[ContainerService Action Runner] could not parse action command for" << containerName
                   << ":" << action;
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

void ContainerService::onCriticalStreamFailure()
{
    emit autoRefreshDown();
}