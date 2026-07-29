#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <data/appconfigreader.h>
#include <setupinstructionswidget.h>
#include <containerlistwidget.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    AppConfigReader *prefs = new AppConfigReader{this};

    // ui->scrollAreaLayout->addWidget(new SetupInstructionsWidget(this));

    auto *containerList = new ContainerListWidget();
    ui->scrollAreaLayout->addWidget(containerList);

    connect(prefs, &AppConfigReader::configLoaded, [&, this]() {
        if (auto config = prefs->readContainers()) {
            containerList->initialize(config->containers());
        } else {
            QMessageBox msgBox(this);
            msgBox.setText("Unknown error has occurred.");
            msgBox.exec();
            this->close();
        }
    });
    prefs->readConfigFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}
