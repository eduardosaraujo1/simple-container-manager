#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QProcess>

#include <QStringList>
#include <QTimer>
#include <QSignalSpy>
#include <model/dockercli.h>
#include <model/data-objects/containerinfo.h>

int test_dockercli_1() {
    DockerCLI dcli;
    QSignalSpy spy(&dcli, &DockerCLI::containersUpdated);

    QObject::connect(&dcli, &DockerCLI::containersUpdated, &dcli, [&dcli](QList<ContainerInfo> containers) {
        for (const ContainerInfo &container : containers) {
            qInfo() << container.toString();
        }
    });

    dcli.requestContainerRefresh({"portainer", "oracle-xe-11g"});

    spy.wait(5000);

    assert(spy.count() == 1);
    return 0;
}

int test_dockercli_2() {
    DockerCLI dcli;
    QSignalSpy spy(&dcli, &DockerCLI::containersUpdated);

    QObject::connect(&dcli, &DockerCLI::containersUpdated, &dcli, [&dcli](QList<ContainerInfo> containers) {
        for (const ContainerInfo &container : containers) {
            qInfo() << container.toString();
        }
    });

    dcli.requestContainerRefresh({"oracle-xe-21c"});
    dcli.requestContainerRefresh({"portainer"}); // should not show (unless system is fast)

    spy.wait(5000);
    assert(spy.count() == 1);
    return 0;
}

int e2e() {
    // Work in Progress
    return 0;
}

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

    test_dockercli_1();
    qInfo() << "---";
    test_dockercli_2();

    // return QCoreApplication::exec();
    return 0;
}
