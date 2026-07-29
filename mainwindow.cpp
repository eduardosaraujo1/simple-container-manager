#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <setupinstructionswidget.h>
#include <QPushButton>

MainWindow::MainWindow(
    ContainerService &containerService,
    QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_containerService(&containerService),
    m_listWidget(new ContainerListWidget())
    // parent intentionally unset ->addWidget() method automatically assigns a parent
{
    ui->setupUi(this);

    ui->lblWarning->hide();
    ui->scrollAreaLayout->addWidget(m_listWidget);

    setupConnections();
}

/// When no ContainerService is provided, the default welcome screen is displayed instead
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lblWarning->hide();
    ui->btnRefresh->setEnabled(false);
    ui->scrollAreaLayout->addWidget(new SetupInstructionsWidget(this));

    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    if (m_containerService && m_listWidget) {
        connect(m_containerService, &ContainerService::containersUpdated,
                this, &MainWindow::onContainersUpdated);

        connect(m_containerService, &ContainerService::autoRefreshDown,
                this, &MainWindow::onAutoRefreshDown);

        connect(m_listWidget, &ContainerListWidget::containerToggle,
                this, &MainWindow::onContainerToggleRequested);

        connect(m_listWidget, &ContainerListWidget::containerAction,
                this, &MainWindow::onContainerActionRequested);

        connect(ui->btnRefresh, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    }

    connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::onExitClicked);
}

void MainWindow::initializeContainers(const ConfiguredContainers &containers)
{

}

void MainWindow::onExitClicked()
{
    this->close();
}

void MainWindow::onRefreshClicked()
{

}

void MainWindow::onContainerToggleRequested(const QString &containerName, bool isStartCommand)
{

}

void MainWindow::onContainerActionRequested(const QString &containerName)
{

}

void MainWindow::onContainersUpdated(const QList<ContainerState> &containers)
{

}

void MainWindow::onAutoRefreshDown()
{

}

void MainWindow::setWarning(const QString &warning)
{
    if (warning.isEmpty()) {
        ui->lblWarning->hide();
        return;
    }

    ui->lblWarning->setText("**Warning**: " % warning);
    ui->lblWarning->show();
}