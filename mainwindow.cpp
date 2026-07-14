#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "containerrowwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->containerList_layout->addWidget(new ContainerRowWidget());
    ui->containerList_layout->addWidget(new ContainerRowWidget());
    ui->containerList_layout->addWidget(new ContainerRowWidget());
    ui->containerList_layout->addWidget(new ContainerRowWidget());
}

MainWindow::~MainWindow()
{
    delete ui;
}
