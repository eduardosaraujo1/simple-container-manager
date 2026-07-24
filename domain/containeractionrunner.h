#ifndef CONTAINERACTIONRUNNER_H
#define CONTAINERACTIONRUNNER_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QProcess>
#include <QSet>

#include <data/apppreferences.h>

/**
 * @brief The ContainerActionRunner class is a dedicated application use
 * case responsible for executing user-defined container actions.
 *
 * Responsibilities:
 * - Validate that the requested action is allowed for the given container.
 * - Launch the configured command in a QProcess.
 * - Prevent action spam by rejecting duplicate requests while one is
 *   already running for a given container.
 * - Notify listeners when execution starts and finishes so the
 *   corresponding UI action button can be enabled/disabled.
 * - Report execution failures.
 *
 * Process management is fully encapsulated here; callers (e.g.
 * MainWindow) should only react to this class's signals.
 */
class ContainerActionRunner : public QObject
{
    Q_OBJECT
public:
    explicit ContainerActionRunner(QObject *parent = nullptr);
    ~ContainerActionRunner();

    /** @brief Configures the runner with the per-container action commands. */
    void initialize(const QList<AppPreferences::ContainerSpec> &containers);

public slots:
    /**
     * @brief Runs the given action for the given container.
     *
     * No-ops (and emits actionFailed()) if the action isn't allowed for
     * this container, or if an action is already running for it.
     */
    void run(const QString &containerName, const QString &action);

signals:
    void actionStarted(const QString &containerName);
    void actionFinished(const QString &containerName, bool success);
    void actionFailed(const QString &containerName, const QString &errorMessage);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessErrorOccurred(QProcess::ProcessError error);

private:
    [[nodiscard]] bool isActionAllowed(const QString &containerName, const QString &action) const;
    [[nodiscard]] QString containerNameForProcess(QProcess *process) const;

    QHash<QString, QString> m_containerActions; // containerName -> configured action/command
    QSet<QString> m_runningContainers;
    QHash<QProcess *, QString> m_processOwners; // running QProcess -> containerName
};

#endif // CONTAINERACTIONRUNNER_H