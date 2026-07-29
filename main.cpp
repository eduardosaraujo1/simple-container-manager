#include "mainwindow.h"

#include <QApplication>
#include <QObject>

#include <data/appconfigreader.h>
#include <data/dockereventstream.h>
#include <data/dockercli.h>
#include <domain/containerservice.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("windowsvista");

    AppConfigReader configReader;

    QObject::connect(&configReader, &AppConfigReader::errorOccurred, [](AppConfigReader::Error error) {
        // Handle cases for write error (not warning) and read error (critical)
    });

    // TODO: handle success path on configReader

    configReader.readConfigFile();

    MainWindow w;
    w.show();
    return QApplication::exec();
}
