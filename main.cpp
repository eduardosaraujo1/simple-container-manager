#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QObject>

#include <data/appconfigreader.h>
#include <data/dockercli.h>
#include <data/dockereventstream.h>
#include <domain/containerservice.h>
#include <domain/logmanager.h>

namespace
{
    /**
     * @brief Loads the app configuration and reports any errors to the user.
     *
     * readConfigFile() is fully synchronous (no QProcess/QTimer/threads
     * involved), so by the time it returns, either configLoaded() or
     * errorOccurred() has already fired and hasCriticalError below is final.
     *
     * @return The parsed containers, or std::nullopt if config could not
     * be loaded and the app should fall back to running without a
     * ContainerService.
     */
    std::optional<ConfiguredContainers> loadAppConfig()
    {
        AppConfigReader configReader;
        bool hasCriticalError = false;

        QObject::connect(&configReader, &AppConfigReader::errorOccurred,
                          [&hasCriticalError](AppConfigReader::Error error) {
            switch (error) {
            case AppConfigReader::Error::FileReadError:
                hasCriticalError = true;
                qCritical() << "[main] Config file could not be read; "
                               "app will start without ContainerService.";
                QMessageBox::critical(
                    nullptr,
                    "Configuration Error",
                    "The configuration file could not be read. "
                    "The application will start without any configured containers.");
                break;
            case AppConfigReader::Error::TemplateWriteError:
                qWarning() << "[main] Failed to write the default config "
                              "template to disk; continuing with in-memory defaults.";
                QMessageBox::warning(
                    nullptr,
                    "Configuration Warning",
                    "Could not write the default configuration file to disk. "
                    "Please create it manually; your settings won't be saved "
                    "between runs until you do.");
                break;
            }
        });

        qInfo() << "[main] Loading application configuration...";
        configReader.readConfigFile();

        if (hasCriticalError) {
            return std::nullopt;
        }

        std::optional<ConfiguredContainers> containers = configReader.readContainers();
        if (!containers.has_value()) {
            // Shouldn't happen given configLoaded() fired, but guard anyway.
            qWarning() << "[main] Config reported as loaded but readContainers() "
                          "returned nothing; starting without ContainerService.";
        } else {
            qInfo() << "[main] Configuration loaded with"
                     << containers->asList().size() << "container(s) configured.";
        }

        return containers;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("windowsvista");

    LogManager logManager(QtInfoMsg, 10); // capture Info+ , keep last 10 files
    logManager.setup();

    qInfo() << "[main] Application starting.";

    std::optional<ConfiguredContainers> containers = loadAppConfig();

    if (!containers.has_value() || !containers.value().hasContainers()) {
        qWarning() << "[main] Starting MainWindow without ContainerService.";
        MainWindow w;
        w.show();
        return QApplication::exec();
    }

    DockerCLI cli;
    DockerEventStream eventStream;
    ContainerService containerService(cli, eventStream, *containers);

    qInfo() << "[main] ContainerService initialized; starting MainWindow.";
    MainWindow w(containerService);
    w.show();
    return QApplication::exec();
}