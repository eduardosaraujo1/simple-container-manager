#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <data/appconfigreader.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    AppConfigReader prefs{this};
    connect(&prefs, &AppConfigReader::configLoaded, [&, this]() {
        if (auto config = prefs.readContainers()) {
            ui->containerList->initialize(config.containers());
        }
    });
    prefs.readConfigFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}
