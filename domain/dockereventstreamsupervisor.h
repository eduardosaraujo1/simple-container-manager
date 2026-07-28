#ifndef DOCKEREVENTSTREAMSUPERVISOR_H
#define DOCKEREVENTSTREAMSUPERVISOR_H

#include <QObject>
#include <QTimer>

#include <data/dockereventstream.h>

/**
 * @brief The DockerEventStreamSupervisor class keeps a DockerEventStream
 * alive: it starts the stream if it isn't running, and restarts it with
 * an exponential-ish backoff whenever it unexpectedly stops.
 *
 * Retry rules:
 * - On the stream's `stopped` signal, wait 500 ms and try to start it again.
 * - Consecutive failed restarts are tracked. Once 3 have happened in a row,
 *   a warning is logged and the backoff increases to 2 minutes for the
 *   following attempts, so a persistently broken Docker connection doesn't
 *   spin the CPU or spam retries.
 * - Any event received on the stream (`eventReceived`) means the stream is
 *   healthy again, so the consecutive-failure count is reset to 0.
 */
class DockerEventStreamSupervisor : public QObject
{
    Q_OBJECT

public:
    explicit DockerEventStreamSupervisor(DockerEventStream &stream, QObject *parent = nullptr);
    ~DockerEventStreamSupervisor() override;

    [[nodiscard]] bool isActive();
    [[nodiscard]] bool onLongBackoff();

public slots:
    /**
     * @brief Starts supervising the stream: starts it immediately if it's
     * down, and from then on transparently restarts it (with backoff) any
     * time it stops on its own. Safe to call more than once; subsequent
     * calls are no-ops.
     */
    void keepAlive();

    /**
     * @brief Stops supervising the stream: does not interrupt the stream if it's
     * already running, but will not try to start it if it shuts down.
     */
    void detatch();

private slots:
    void onStreamStopped();
    void onStreamEventReceived();
    void onRetryTimerTimeout();

private:
    void scheduleRestart(int delayMs);

    static constexpr int kShortBackoffMs = 500;
    static constexpr int kLongBackoffMs = 2 * 60 * 1000; // 2 minutes
    static constexpr int kMaxConsecutiveFailures = 3;

    DockerEventStream *m_stream;
    QTimer m_retryTimer;
    int m_consecutiveFailures = 0;
    bool m_active = false;
};

#endif // DOCKEREVENTSTREAMSUPERVISOR_H