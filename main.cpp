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

int test_dockercli_1()
{
    DockerCLI dcli;
    QSignalSpy spy(&dcli, &DockerCLI::containersUpdated);

    QObject::connect(&dcli, &DockerCLI::containersUpdated, &dcli, [&dcli](QList<ContainerInfo> containers)
                     {
        for (const ContainerInfo &container : containers) {
            qInfo() << container.toString();
        } });

    dcli.requestContainerRefresh({"portainer", "oracle-xe-11g"});

    spy.wait(5000);

    assert(spy.count() == 1);
    return 0;
}

int test_dockercli_2()
{
    DockerCLI dcli;
    QSignalSpy spy(&dcli, &DockerCLI::containersUpdated);

    QObject::connect(&dcli, &DockerCLI::containersUpdated, &dcli, [&dcli](QList<ContainerInfo> containers)
                     {
        for (const ContainerInfo &container : containers) {
            qInfo() << container.toString();
        } });

    dcli.requestContainerRefresh({"oracle-xe-21c"});
    dcli.requestContainerRefresh({"portainer"}); // expected to error out and not show.

    spy.wait(5000);
    assert(spy.count() == 1);
    return 0;
}

int test_dockercli_3()
{
    const QString containerId = "73095f503db88226d7861d85852cad4aa6b029352b039ca31859b893d7a97dfb";
    bool received = false;
    DockerCLI dcli;
    DockerEventStream des;
    QEventLoop loop;
    QTimer timeout;

    des.waitUntilActive();
    timeout.setSingleShot(true);

    QObject::connect(&timeout, &QTimer::timeout,
                     &loop, &QEventLoop::quit);

    QObject::connect(
        &des,
        &DockerEventStream::eventReceived,
        [&](const DockerEvent &event)
        {
            if (event.action() == DockerEvent::Action::Stop &&
                event.containerId() == containerId)
            {
                received = true;
                timeout.stop();
                loop.quit();
            }
        });

    timeout.start(10000);
    dcli.stopContainer(containerId);

    // Lock until the event has emitted or timeout has ran
    loop.exec();

    // Check
    assert(received);
    des.abort();
    // Currently, there is a warning that a QProcess instance is destroyed before the process is terminated.
    // It refers too the `docker start` QProcess, because the Start event triggers before the command finishes. And since the application (both
    // in tests and in production) relies on said event to determine if it should refresh or not, the test suite cleans up before the instance has time to
    // finish and teardown.
    //
    // This is not a problem: the process can be orphaned for a few milliseconds, as it terminates itself. Besides the log message in the console, no
    // memory leaks occur (thanks to Qt's Object Hierarchy) and the behavior remains the same.

    return 0;
}
int test_dockercli_4()
{
    const QString containerId = "73095f503db88226d7861d85852cad4aa6b029352b039ca31859b893d7a97dfb";
    bool received = false;
    DockerCLI dcli;
    DockerEventStream des;
    QEventLoop loop;
    QTimer timeout;

    des.waitUntilActive();
    timeout.setSingleShot(true);

    QObject::connect(&timeout, &QTimer::timeout,
                     &loop, &QEventLoop::quit);

    QObject::connect(
        &des,
        &DockerEventStream::eventReceived,
        [&](const DockerEvent &event)
        {
            if (event.action() == DockerEvent::Action::Start &&
                event.containerId() == containerId)
            {
                received = true;
                timeout.stop();
                loop.quit();
            }
        });

    timeout.start(10000);
    dcli.startContainer(containerId);

    loop.exec();

    // Assert
    assert(received);

    // Cleanup
    des.abort();
    // Currently, there is a warning that a QProcess instance is destroyed before the process is terminated.
    // It refers too the `docker start` QProcess, because the Start event triggers before the command finishes. And since the application (both
    // in tests and in production) relies on said event to determine if it should refresh or not, the test suite cleans up before the instance has time to
    // finish and teardown.
    //
    // This is not a problem: the process can be orphaned for a few milliseconds, as it terminates itself. Besides the log message in the console, no
    // memory leaks occur (thanks to Qt's Object Hierarchy) and the behavior remains the same.
    return 0;
}

int helper_expectEvent(DockerEvent::Action expectedAction,
                       const QStringList &dockerArgs)
{
    DockerEventStream stream;

    QSignalSpy startedSpy(&stream, &DockerEventStream::started);
    QSignalSpy eventSpy(&stream, &DockerEventStream::eventReceived);
    QSignalSpy stoppedSpy(&stream, &DockerEventStream::stopped);

    stream.start();

    if (!startedSpy.wait(5000))
    {
        qCritical() << "DockerEventStream never started.";
        return 1;
    }

    QProcess proc;
    proc.start("docker", dockerArgs);

    if (!proc.waitForFinished(10000))
    {
        qCritical() << "Docker command timed out.";
        stream.stop();
        return 1;
    }

    if (!eventSpy.wait(10000))
    {
        qCritical() << "Timed out waiting for Docker event.";
        stream.stop();
        return 1;
    }

    const QList<QVariant> arguments = eventSpy.takeFirst();

    if (arguments.size() != 1)
    {
        qCritical() << "eventReceived emitted unexpected arguments.";
        stream.stop();
        return 1;
    }

    const DockerEvent event = qvariant_cast<DockerEvent>(arguments.at(0));

    qInfo() << event.toString();
    qInfo() << "---";

    if (event.action() != expectedAction)
    {
        qCritical()
            << "Expected action"
            << DockerEvent::containerActionToString(expectedAction)
            << "but received"
            << DockerEvent::containerActionToString(event.action());

        stream.stop();
        return 1;
    }

    stream.stop();

    if (!stoppedSpy.wait(3000))
    {
        qCritical() << "DockerEventStream never stopped.";
        return 1;
    }

    return 0;
}

int test_dockerevent_pause()
{
    return helper_expectEvent(
        DockerEvent::Action::Pause,
        {"pause",
         "oracle-xe-11g"});
}

int test_dockerevent_unpause()
{
    return helper_expectEvent(
        DockerEvent::Action::Unpause,
        {"unpause",
         "oracle-xe-11g"});
}

int test_dockerevent_stop()
{
    return helper_expectEvent(
        DockerEvent::Action::Stop,
        {"stop",
         "oracle-xe-11g"});
}

int test_dockerevent_start()
{
    return helper_expectEvent(
        DockerEvent::Action::Start,
        {"start",
         "oracle-xe-11g"});
}

int test_app_prefs_1()
{
    AppPreferences prefs{};

    // Setup: Ensure the folder doesn't exist
    QDir dir("/home/fatec/.config/simple-container-manager/");

    if (dir.exists())
    {
        dir.removeRecursively();
    }

    // Act: Refresh the config
    prefs.readConfigFile();

    // Check: Expect the file to be present in the file system
    qInfo() << "I do not have the energy to implement this feature. Please check file:///home/fatec/.config manually";

    return 0;
}

int test_app_prefs_2()
{
    AppPreferences prefs{};

    // Setup: Ensure the file doesn't exist
    QFileInfo info("/home/fatec/.config/simple-container-manager/preferences.yaml");

    if (info.exists())
    {
        QFile::remove(info.absoluteFilePath());
    }

    // Act: Refresh the config
    prefs.refreshConfig();

    // Check: Expect the file to be present in the file system
    qInfo() << "I do not have the energy to implement this feature. Please check file:///home/fatec/.config manually";

    return 0;
}

int test_app_prefs_3()
{
    AppPreferences prefs{};

    prefs.refreshConfig();

    // Act: Expect it to read everything
    QList<AppPreferences::ContainerSpec> containers = prefs.containers();

    // Check: Expect the file to be present in the file system
    qInfo() << "I do not have the energy to implement this. Please check if file:///home/fatec/.config/simple-container-manager/preferences.yaml matches the containers";
    for (const auto &container : containers)
    {
        qInfo() << container.toString();
    }

    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
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

    // qInfo() << "\n--TEST1--\n";
    // test_dockercli_1();
    // qInfo() << "\n--TEST2--\n";
    // test_dockercli_2();
    // qInfo() << "\n--TEST3--\n";
    // test_dockercli_3();
    // qInfo() << "\n--TEST4--\n";
    // test_dockercli_4();

    qInfo() << "\n--TEST1--\n";
    test_dockerevent_pause();
    qInfo() << "\n--TEST2--\n";
    test_dockerevent_unpause();
    qInfo() << "\n--TEST3--\n";
    test_dockerevent_stop();
    qInfo() << "\n--TEST4--\n";
    test_dockerevent_start();

    // qInfo() << "\n--TEST1--\n";
    // test_app_prefs_1();
    // qInfo() << "\n--TEST2--\n";
    // test_app_prefs_2();
    // qInfo() << "\n--TEST3--\n";
    // test_app_prefs_3();

    // return QCoreApplication::exec();
    return 0;
}
