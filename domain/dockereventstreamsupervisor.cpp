#include "dockereventstreamsupervisor.h"

#include <QDebug>

DockerEventStreamSupervisor::DockerEventStreamSupervisor(DockerEventStream &stream, QObject *parent)
    : QObject(parent), m_stream(stream)
{
    m_retryTimer.setSingleShot(true);

    connect(&m_retryTimer, &QTimer::timeout,
            this, &DockerEventStreamSupervisor::onRetryTimerTimeout);

    connect(&m_stream, &DockerEventStream::stopped,
            this, &DockerEventStreamSupervisor::onStreamStopped);

    connect(&m_stream, &DockerEventStream::errorOccurred,
            this, &DockerEventStreamSupervisor::onStreamStopped);


    connect(&m_stream, &DockerEventStream::eventReceived,
            this, &DockerEventStreamSupervisor::onStreamEventReceived);
}

DockerEventStreamSupervisor::~DockerEventStreamSupervisor() = default;

void DockerEventStreamSupervisor::keepAlive()
{
    if (m_active)
    {
        return;
    }
    qInfo() << "[DockerEventStreamSupervisor] Supervisor configured to keep DockerEventStream alive.";

    m_active = true;

    if (!m_stream.isRunning())
    {
        m_stream.start();
    }
}

void DockerEventStreamSupervisor::detatch() {
    if (m_active) {
        m_active = false;
        m_consecutiveFailures = 0;
    }
}

void DockerEventStreamSupervisor::onStreamStopped()
{
    if (!m_active || !m_stream.isNotRunning())
    {
        return;
    }

    qInfo() << "[DockerEventStreamSupervisor] Identified " << m_consecutiveFailures+1 << " consecutive stream failure.";

    ++m_consecutiveFailures;

    if (m_consecutiveFailures < kMaxConsecutiveFailures)
    {
        qDebug() << "[DockerEventStreamSupervisor] Scheduling stream restart.";
        scheduleRestart(kBackoffMs);
        return;
    }

    qWarning() << "[DockerEventStreamSupervisor] Could not keep the Docker events stream up after"
               << m_consecutiveFailures << "consecutive attempts. Detatching auto restart and emitting signal.";

    emit maxConsecutiveFailuresReached();
    detatch();
}

void DockerEventStreamSupervisor::onStreamEventReceived()
{
    m_consecutiveFailures = 0;
}

void DockerEventStreamSupervisor::onRetryTimerTimeout()
{
    if (!m_active || m_stream.isRunning())
    {
        return;
    }

    qInfo() << "[DockerEventStreamSupervisor] Starting stream after retry timer is up";
    m_stream.start();
}

void DockerEventStreamSupervisor::scheduleRestart(int delayMs)
{
    m_retryTimer.start(delayMs);
}

bool DockerEventStreamSupervisor::isActive() {
    return m_active;
}
