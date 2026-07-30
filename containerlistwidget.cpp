#include "containerlistwidget.h"
#include "ui_containerlistwidget.h"

ContainerListWidget::ContainerListWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::ContainerListWidget)
{
    ui->setupUi(this);
}

bool ContainerListWidget::initialize(
    const QList<ContainerDefinition> &containers,
    int loadingTimeout)
{
    if (m_initialized)
    {
        qWarning() << "[ContainerListWidget UI] Attempted to initialize more than once. This is unsupported.";
        return false;
    }

    for (const ContainerDefinition &spec : containers)
    {
        ContainerRowWidget *row = new ContainerRowWidget(
            spec.label(),
            spec.icon(),
            spec.hasAction(),
            ContainerRowWidget::Status::Loading,
            this);

        // Associate the container row pointer to its parent container
        const QString containerName = spec.name();
        m_rows.insert(containerName, row);

        // Setup interactions
        connect(row,
                &ContainerRowWidget::toggleRequested,
                this,
                [this, containerName](bool start)
                {
                    qInfo() << "[ContainerListWidget UI] Requesting to `" << (start ? "start" : "stop") << "` container " << containerName;
                    emit containerToggle(containerName, start);
                });

        connect(row,
                &ContainerRowWidget::adminRequested,
                this,
                [this, containerName]()
                {
                    qInfo() << "[ContainerListWidget UI] Requesting to run admin command from " << containerName;
                    emit containerAction(containerName);
                });

        // Include created widget in layout
        ui->contentsLayout->addWidget(row);
    }

    // After X seconds of initialization, any container that was Loading becomes NotFound.
    QTimer::singleShot(loadingTimeout, this, [this]() {
        qInfo() << "[ContainerListWidget UI] Applying not found status to containers past the timeout.";
        for (ContainerRowWidget *row : std::as_const(m_rows)) {
            if (row && row->status() == ContainerRowWidget::Status::Loading) {
                row->setStatus(ContainerRowWidget::Status::NotFound);
            }
        }
    });

    // Finish up initialization
    ui->loading->hide();
    ui->contentsLayout->addStretch();
    m_initialized = true;

    return true;
}

bool ContainerListWidget::isInitialized()
{
    return m_initialized;
}

ContainerListWidget::~ContainerListWidget()
{
    delete ui;
}

void ContainerListWidget::refreshContainerInfo(
    const QList<ContainerState> &containers)
{
    for (const ContainerState &container : containers)
    {
        auto it = m_rows.find(container.name());

        if (it == m_rows.end()) {
            qWarning() << "[ContainerListWidget UI] The corresponding widget for '" << container.name() << "'' was not found. Status will not be displayed.";
            continue;
        }

        it.value() // ContainerRowWidget
            ->setStatus(mapStatus(container.status()));
    }
}

void ContainerListWidget::setContainerStatus(const QString &containerName, ContainerRowWidget::Status status)
{
    auto it = m_rows.find(containerName);

    if (it == m_rows.end()) {
        qWarning() << "[ContainerListWidget UI] The corresponding widget for '" << containerName << "'' was not found. Status will not be displayed.";
        return;
    }

    it.value() // ContainerRowWidget
        ->setStatus(status);

    // Timeout to NotFound if Loading, Starting of Stopping remains for too long.
    if (status == ContainerRowWidget::Status::Loading
        || status == ContainerRowWidget::Status::Starting
        || status == ContainerRowWidget::Status::Stopping
        ) {
        scheduleContainerTimeout(containerName);
    }
}

ContainerRowWidget::Status
ContainerListWidget::mapStatus(ContainerState::Status status)
{
    switch (status)
    {

    case ContainerState::Status::Running:
        return ContainerRowWidget::Status::Started;

    case ContainerState::Status::Created:
    case ContainerState::Status::Exited:
        return ContainerRowWidget::Status::Stopped;
    case ContainerState::Status::Restarting:
    case ContainerState::Status::Removing:
        return ContainerRowWidget::Status::Loading;

    case ContainerState::Status::Paused:
    case ContainerState::Status::Dead:
    case ContainerState::Status::Unknown:
    default:
        return ContainerRowWidget::Status::NotFound;
    }
}

void ContainerListWidget::scheduleContainerTimeout(const QString &containerName, int timeout)
{
    QTimer::singleShot(timeout, this, [this, containerName]() {
        auto it = m_rows.find(containerName);
        if (it == m_rows.end()) return;
        ContainerRowWidget *row = it.value();

            if (row && (row->status() == ContainerRowWidget::Status::Loading
                    || row->status() == ContainerRowWidget::Status::Starting
                    || row->status() == ContainerRowWidget::Status::Stopping)) {
                qInfo() << "[ContainerListWidget UI] Docker did not respond with permanent"
                           << "state in time for" << containerName;
                row->setStatus(ContainerRowWidget::Status::NotFound);
            }
    });
}
