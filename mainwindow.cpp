#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <setupinstructionswidget.h>
#include <data/data-objects/containerdefinition.h>

#include <QPushButton>
#include <QTimer>
#include <QList>

MainWindow::MainWindow(
    ContainerService &containerService,
    QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (containerService.isConfigEmpty()) {
        setupWelcomeScreen();
    } else {
        m_containerService = &containerService;
        setupContainerList(containerService);
    }

    connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::onExitClicked);
    m_initialized = true;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupWelcomeScreen();

    connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::onExitClicked);
    m_initialized = true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupContainerConnections(ContainerListWidget &listWidget,
                                           ContainerService &containerService)
{
    if (listWidget.isInitialized()) {
        qWarning() << "[UI] Attempted to initialize container connections more than once. Ignoring request...";
        return;
    }

    connect(&containerService, &ContainerService::containersUpdated,
            this, &MainWindow::onContainersUpdated);

    connect(&containerService, &ContainerService::autoRefreshDown,
            this, &MainWindow::onAutoRefreshDown);

    connect(&listWidget, &ContainerListWidget::containerToggle,
            this, &MainWindow::onContainerToggleRequested);

    connect(&listWidget, &ContainerListWidget::containerAction,
            this, &MainWindow::onContainerActionRequested);

    connect(ui->btnRefresh, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);

    qDebug() << "[UI] Successfully connected buttons to domain actions.";
}

bool MainWindow::setupContainerList(ContainerService &containerService)
{
    if (m_listWidget) {
        qWarning() << "[UI] Attempted to initialize container configuration more than once. Ignoring request...";
        return false;
    }

    // Setup the widgets
    m_listWidget = new ContainerListWidget();
    ui->scrollAreaLayout->addWidget(m_listWidget);
    setupContainerConnections(*m_listWidget, containerService);

    // Populate the widget
    m_listWidget->initialize(containerService.containers().asList(), 5000);
    containerService.requestContainerUpdate();

    // Other constructor operations
    ui->lblWarning->hide();

    qInfo() << "[UI] Successfully finished loading container widgets.";

    return true;
}

bool MainWindow::setupWelcomeScreen()
{
    if (m_listWidget) {
        qWarning() << "[UI] Attempted to setup welcome screen after previous ContainerListWidget setup. Ignoring request...";
        return false;
    }
    if (m_initialized) {
        qWarning() << "[UI] Attempted to setup welcome screen widget more than once. Ignoring request...";
        return false;
    }
    ui->scrollAreaLayout->addWidget(new SetupInstructionsWidget(this));

    ui->lblWarning->hide();
    ui->heading->hide();
    ui->btnRefresh->setEnabled(false);

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

    qInfo() << "[UI] Request button clicked. Initiating request...";

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
        qInfo() << "[UI] Requesting start container " << containerName;
        m_containerService->startContainer(containerName);
    } else {
        qInfo() << "[UI] Requesting stop container " << containerName;
        m_containerService->stopContainer(containerName);
    }

    // If the event stream (in-real-time status update) is down, trigger the update here
    if (! m_containerService->isAutoRefreshUp()) {
        qInfo() << "[UI] Auto refresh is down. Performing automatic update";
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
    qInfo() << "[UI] Identified container list update. Applying refresh.";
    m_listWidget->refreshContainerInfo(containers);

    // When a refresh is finished, ensure the user can click the refresh button again
    QTimer::singleShot(500, [this]() {
        ui->btnRefresh->reenable();
    });
}

void MainWindow::onAutoRefreshDown()
{
    qInfo() << "[UI] Detected auto refresh crash. Displaying error...";
    setWarning("Auto refresh has crashed. Container state may become stale.");
}

void MainWindow::onAutoRefreshUp()
{
    qInfo() << "[UI] Detected auto refresh restoration. Removing error...";
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