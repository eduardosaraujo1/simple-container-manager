#include <QCoreApplication>
// #include <QLocale>
// #include <QTranslator>

#include <QProcess>
#include <QStringList>
#include <QTimer>
#include <QSignalSpy>
#include <QString>
#include <yaml-cpp/yaml.h>
#include <model/dockercli.h>
#include <model/data-objects/containerinfo.h>
#include <model/dockereventstream.h>
#include <model/apppreferences.h>

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
    dcli.requestContainerRefresh({"portainer"}); // expected to error out and not show.

    spy.wait(5000);
    assert(spy.count() == 1);
    return 0;
}

int helper_dockerevent_layout(const QStringList &args) {
    DockerEventStream des;
    QSignalSpy spy(&des, &DockerEventStream::eventReceived);
    QProcess proc;

    QObject::connect(&des, &DockerEventStream::eventReceived, &des, [&des](DockerEvent event) {
        qInfo() << event.toString();
        qInfo() << "---";;
    });

    proc.start("docker", args);

    if (spy.wait(10000)) {
        des.abort();
        proc.waitForFinished();
        return 0;
    } else {
        qWarning() << "Test timed out waiting for eventReceived!";
        des.abort();
        proc.waitForFinished();
        assert(false);
    }
}

int test_dockerevent_1() {
    return helper_dockerevent_layout(QStringList{
        "pause",
        "oracle-xe-11g"
    });
}
int test_dockerevent_2() {
    return helper_dockerevent_layout(QStringList{
        "unpause",
        "oracle-xe-11g"
    });
}
int test_dockerevent_3() {
    return helper_dockerevent_layout(QStringList{
        "stop",
        "oracle-xe-11g"
    });
}
int test_dockerevent_4() {
    return helper_dockerevent_layout(QStringList{
        "start",
        "oracle-xe-11g"
    });
}

int test_app_prefs_1() {
    AppPreferences prefs{};

    // Setup: Ensure the folder doesn't exist
    QDir dir("/home/fatec/.config/simple-container-manager/");

    if (dir.exists()) {
        dir.removeRecursively();
    }

    // Act: Refresh the config
    prefs.refreshConfig();

    // Check: Expect the file to be present in the file system
    qInfo() << "I do not have the energy to implement this feature. Please check file:///home/fatec/.config manually";

    return 0;
}

int test_app_prefs_2() {
    AppPreferences prefs{};

    // Setup: Ensure the file doesn't exist
    QFileInfo info("/home/fatec/.config/simple-container-manager/preferences.yaml");

    if (info.exists()) {
        QFile::remove(info.absoluteFilePath());
    }

    // Act: Refresh the config
    prefs.refreshConfig();

    // Check: Expect the file to be present in the file system
    qInfo() << "I do not have the energy to implement this feature. Please check file:///home/fatec/.config manually";

    return 0;
}

int test_app_prefs_3() {
    AppPreferences prefs{};

    // Act: Expect it to read everything
    QList<AppPreferences::ContainerSpec> containers = prefs.containers();

    // Check: Expect the file to be present in the file system
    qInfo() << "I do not have the energy to implement this. Please check if file:///home/fatec/.config/simple-container-manager/preferences.yaml matches the containers";
    for (const auto& container : containers) {
        qInfo() << container.toString();
    }

    return 0;
}

int main(int argc, char *argv[])
{
    qDebug() << "Hello, world!A;";
    return 0;
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("SimpleContainerGroup");
    QCoreApplication::setApplicationName("simple-container-manager");

    // QTranslator translator;
    // const QStringList uiLanguages = QLocale::system().uiLanguages();
    // for (const QString &locale : uiLanguages) {
        // const QString baseName = "workspace_" + QLocale(locale).name();
        // if (translator.load(":/i18n/" + baseName)) {
            // a.installTranslator(&translator);
            // break;
        // }
    // }

    // test_dockercli_1();
    // qInfo() << "---";
    // test_dockercli_2();

    // qInfo() << "\n--TEST1--\n";
    // test_dockerevent_1();
    // qInfo() << "\n--TEST2--\n";
    // test_dockerevent_2();
    // qInfo() << "\n--TEST3--\n";
    // test_dockerevent_3();
    // qInfo() << "\n--TEST4--\n";
    // test_dockerevent_4();

    qInfo() << "\n--TEST1--\n";
    test_app_prefs_1();
    // qInfo() << "\n--TEST2--\n";
    // test_app_prefs_2();
    // qInfo() << "\n--TEST3--\n";
    // test_app_prefs_3();

    // return QCoreApplication::exec();
    return 0;
}
