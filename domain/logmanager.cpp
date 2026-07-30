#include "logmanager.h"

#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>
#include <QTextStream>
#include <QMutexLocker>
#include <QDebug>
#include <cstdlib>

LogManager *LogManager::s_instance = nullptr;

LogManager::LogManager(QtMsgType minimumLevel, int maxLogFiles, QObject *parent)
    : QObject(parent)
    , m_minimumLevel(minimumLevel)
    , m_maxLogFiles(maxLogFiles > 0 ? maxLogFiles : 1)
{
}

LogManager::~LogManager()
{
    QMutexLocker locker(&m_mutex);
    if (s_instance == this) {
        qInstallMessageHandler(nullptr);
        s_instance = nullptr;
    }
    if (m_logFile.isOpen())
        m_logFile.close();
}

void LogManager::setup()
{
    const QString logDir = ensureLogDirectory();
    if (logDir.isEmpty()) {
        qWarning() << "LogManager: could not determine/create log directory, logging to file disabled";
        return;
    }

    const QString fileName = buildUniqueLogFileName(logDir);
    const QString fullPath = QDir(logDir).filePath(fileName);

    m_logFile.setFileName(fullPath);
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qWarning() << "LogManager: failed to open log file" << fullPath << m_logFile.errorString();
        return;
    }

    // Rotate away old files now that the new one exists.
    enforceMaxLogFiles(logDir);

    s_instance = this;
    qInstallMessageHandler(&LogManager::messageHandler);
}

void LogManager::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    if (type == QtMsgType::QtCriticalMsg) {
        fprintf(stderr, "%s\n", qUtf8Printable(msg));
        fflush(stderr);
    } else {
        fprintf(stdout, "%s\n", qUtf8Printable(msg));
        fflush(stdout);
    }

    if (s_instance)
        s_instance->handleMessage(type, msg);

    if (type == QtFatalMsg) {
        // Preserve Qt's default behavior of aborting on fatal messages.
        std::abort();
    }
}

void LogManager::handleMessage(QtMsgType type, const QString &formattedText)
{
    if (severityRank(type) < severityRank(m_minimumLevel))
        return;

    QMutexLocker locker(&m_mutex);
    if (!m_logFile.isOpen())
        return;

    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    const QString line = QStringLiteral("[%1] [%2] %3\n")
                              .arg(timestamp, levelLabel(type), formattedText);

    QTextStream stream(&m_logFile);
    stream << line;
    stream.flush();
}

int LogManager::severityRank(QtMsgType type)
{
    // QtMsgType's numeric values are not in severity order (QtInfoMsg == 4),
    // so map explicitly instead of comparing the enum values directly.
    switch (type) {
    case QtDebugMsg:    return 0;
    case QtInfoMsg:     return 1;
    case QtWarningMsg:  return 2;
    case QtCriticalMsg: return 3;
    case QtFatalMsg:    return 4;
    }
    return 0;
}

QString LogManager::levelLabel(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg:    return QStringLiteral("DEBUG");
    case QtInfoMsg:     return QStringLiteral("INFO");
    case QtWarningMsg:  return QStringLiteral("WARNING");
    case QtCriticalMsg: return QStringLiteral("CRITICAL");
    case QtFatalMsg:    return QStringLiteral("CRITICAL");
    }
    return QStringLiteral("INFO");
}

QString LogManager::ensureLogDirectory() const
{
    const QString configLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configLocation.isEmpty())
        return QString();

    const QString logDir = QDir(configLocation).filePath("logs");
    QDir dir;
    if (!dir.exists(logDir)) {
        if (!dir.mkpath(logDir))
            return QString();
    }
    return logDir;
}

QString LogManager::buildUniqueLogFileName(const QString &logDir) const
{
    const QString base = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");

    int suffix = 0;
    QString candidate;
    do {
        candidate = QStringLiteral("%1_%2.log").arg(base).arg(suffix);
        ++suffix;
    } while (QFile::exists(QDir(logDir).filePath(candidate)));

    return candidate;
}

void LogManager::enforceMaxLogFiles(const QString &logDir) const
{
    QDir dir(logDir);
    QFileInfoList files = dir.entryInfoList(QStringList() << "*.log", QDir::Files, QDir::Name);

    // Name format "yyyy-MM-dd_hh-mm-ss_X.log" sorts lexicographically in
    // chronological order, so QDir::Name gives us oldest-first ordering.
    while (files.size() > m_maxLogFiles) {
        const QFileInfo oldest = files.takeFirst();
        QFile::remove(oldest.absoluteFilePath());
    }
}