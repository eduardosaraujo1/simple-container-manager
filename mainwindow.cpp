#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <data/appconfigreader.h>
#include <setupinstructionswidget.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // AppConfigReader prefs{this};
    ui->scrollAreaLayout->addWidget(new SetupInstructionsWidget(this));
    // connect(&prefs, &AppConfigReader::configLoaded, [&, this]() {
        // if (auto config = prefs.readContainers()) {
            // ui->containerList->initialize(config->containers());
        // } else {
            // QMessageBox msgBox(this);
            // msgBox.setText("Unknown error has occurred.");
            // msgBox.exec();
            // this->close();
        // }
    // });
    // prefs.readConfigFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}
