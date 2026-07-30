#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <setupinstructionswidget.h>

#include <QPushButton>
#include <QTimer>
#include <QList>

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lblWarning->hide();
    ui->heading->hide();
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
    connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::onExitClicked);

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

    qDebug() << "[UI] Successfully connected buttons to domain actions.";
}

bool MainWindow::initializeContainers()
{
    if (!m_containerService || !m_listWidget) {
        qWarning() << "[UI] Attempted to initialize container configuration without list widget.";
        return false;
    }
    if (m_listWidget->isInitialized()) {
        return false;
    }

    m_listWidget->initialize(m_containerService->containers().asList());
    m_containerService->requestContainerUpdate();

    return true;
}

void MainWindow::onExitClicked()
{
    this->close();
}

void MainWindow::onRefreshClicked()
{
    if (!m_containerService || !m_listWidget) {
        qWarning() << "[UI] Received container refresh request without active ContainerService.";
        return;
    }

    m_containerService->requestContainerUpdate();
}

void MainWindow::onContainerToggleRequested(const QString &containerName, bool isStartCommand)
{
    if (!m_containerService) {
        qWarning() << "[UI] Received container toggle request without active ContainerService.";
        return;
    }
    // Set status to "Starting" or "Stopping" imediatelly because
    // to communicate "request was received and is in progress"
    m_listWidget->setContainerStatus(containerName, isStartCommand
                                     ? ContainerRowWidget::Status::Starting
                                     : ContainerRowWidget::Status::Stopping);

    if (isStartCommand) {
        m_containerService->startContainer(containerName);
    } else {
        m_containerService->stopContainer(containerName);
    }

    // If the event stream (in-real-time status update) is down, trigger the update here
    if (! m_containerService->isAutoRefreshUp()) {
        m_containerService->requestContainerUpdate();
    }
}

void MainWindow::onContainerActionRequested(const QString &containerName)
{
    if (!m_containerService) {
        qWarning() << "[UI] Received container admin action request without active ContainerService.";
        return;
    }

    m_containerService->runAction(containerName);
}

void MainWindow::onContainersUpdated(const QList<ContainerState> &containers)
{
    if (!m_containerService || !m_listWidget) {
        qWarning() << "[UI] Received container list update without available list widget or ContainerService.";
        return;
    }

    // Apply update to list
    m_listWidget->refreshContainerInfo(containers);

    // When a refresh is finished, ensure the user can click the refresh button again
    QTimer::singleShot(500, [this]() {
        ui->btnRefresh->reenable();
    });
}

void MainWindow::onAutoRefreshDown()
{
    setWarning("Auto refresh has crashed. Container state may become stale.");
}

void MainWindow::onAutoRefreshUp()
{
    setWarning("");
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