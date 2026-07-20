#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <data/apppreferences.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    AppPreferences prefs{this};
    connect(&prefs, &AppPreferences::preferencesUpdated, [&, this]() {
        ui->containerList->initialize(prefs.containers());
    });
    prefs.readConfigFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}
