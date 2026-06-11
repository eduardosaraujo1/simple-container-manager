#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QProcess>
#include <QStringList>

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

    // I usually put this in a dependency manager class (like Laravel Service Providers, except smaller and without reflection)
    // Since this is a testing suite, let's just keep it here
    // DockerCLI dcli();
    // DockerEventStream devl(); // This uses signals and slots to communicate to DockerService when something changed. Not sure if this is the best decision
    // DockerService ds(dcli,devl);

    QProcess p;
    QObject::connect(&p, &QProcess::finished, &a, [&p](int code, QProcess::ExitStatus status) {
        qInfo() << p.readAllStandardOutput();
        qInfo() << " SEPARATOR ";
        qInfo() << p.readAllStandardError();
    });
    p.start("docker", QStringList{
        "ps",
        "-a",
        "--no-trunc",
        "--filter=name=oracle-xe-11g",
        "--format=\"{\"id\":{{json .ID}},\"name\":{{json .Names}},\"status\":{{json .State}}}\""
    });

    return QCoreApplication::exec();
}
