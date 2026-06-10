#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QObject>
#include <QProcess>
#include "signalreceiver.h"
#include "signaltransmitter.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "workspace_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // Set up code that uses the Qt event loop here.
    // Call QCoreApplication::quit() or QCoreApplication::exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    SignalTransmitter st;
    SignalReceiver sr;

    QProcess p;
    // p.start("docker", QStringList{
        // "ps",
        // "-a",
        // "--format='{\"ID\":{{json .ID}},\"Names\":{{json .Names}},\"State\":{{json .State}}}'"
    // });
    p.start("docker", QStringList{
        "events",
        "--format",
        "'{\"Action\":{{json .Action}},\"ContainerID\":{{json .Actor.ID}}'"
    });

    QObject::connect(&p,&QProcess::readyReadStandardOutput, &p, [&p]() {
        qInfo() << p.readAllStandardOutput();
    });

    // If you do not need a running Qt event loop, remove the call
    // to QCoreApplication::exec() or use the Non-Qt Plain C++ Application template.

    return QCoreApplication::exec();
}
