#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QtGlobal>
#include <QString>
#include <QFile>
#include <QMutex>

// LogManager installs a Qt message handler (qDebug/qInfo/qWarning/qCritical/qFatal)
// and spools filtered log output into timestamped files under
// <AppConfigLocation>/logs, rotating away the oldest files once a maximum
// count is exceeded.
//
// Usage (typically in main.cpp, before creating QApplication or right after):
//
//     LogManager logManager(QtInfoMsg, 10); // capture Info and above, keep 10 files
//     logManager.setup();
//
// The instance must stay alive for the lifetime of the application (it
// uninstalls the Qt message handler in its destructor).
class LogManager : public QObject
{
    Q_OBJECT

public:
    // minimumLevel: lowest severity that will be written to the log file
    //               (QtDebugMsg < QtInfoMsg < QtWarningMsg < QtCriticalMsg < QtFatalMsg)
    // maxLogFiles:  maximum number of log files to keep in the logs folder;
    //               the oldest file(s) are deleted once this is exceeded
    explicit LogManager(QtMsgType minimumLevel, int maxLogFiles = 10, QObject *parent = nullptr);
    ~LogManager() override;

    // Installs the Qt message handler and creates the log file for this
    // session. Must be called once, after construction, for logging to
    // actually be captured.
    void setup();

private:
    // Called by the static Qt message handler trampoline for every log
    // message emitted by the application.
    void handleMessage(QtMsgType type, const QString &formattedText);

    // Actual Qt message handler installed via qInstallMessageHandler(). Qt
    // requires a plain function pointer, so this trampoline forwards to the
    // active LogManager instance.
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    // Returns a severity rank so messages can be compared regardless of the
    // (non-monotonic) numeric values of the QtMsgType enum.
    static int severityRank(QtMsgType type);

    // Maps a QtMsgType to the textual level used in the log entry, e.g. "WARNING".
    static QString levelLabel(QtMsgType type);

    // Builds the logs directory path (creating it if absent) alongside the
    // app's config location.
    QString ensureLogDirectory() const;

    // Picks a free "yyyy-MM-dd_hh-mm-ss_X" file name inside the logs directory.
    QString buildUniqueLogFileName(const QString &logDir) const;

    // Deletes the oldest log file(s) in the logs directory until the number
    // of files is at or below m_maxLogFiles.
    void enforceMaxLogFiles(const QString &logDir) const;

    static LogManager *s_instance;

    QtMsgType m_minimumLevel;
    int m_maxLogFiles;
    QFile m_logFile;
    QMutex m_mutex;
};

#endif // LOGMANAGER_H