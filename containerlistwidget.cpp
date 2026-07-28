#include "containerlistwidget.h"
#include "ui_containerlistwidget.h"

ContainerListWidget::ContainerListWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::ContainerListWidget)
{
    ui->setupUi(this);
}

void ContainerListWidget::initialize(
    const QList<ContainerDefinition> &containers)
{
    if (m_initialized)
    {
        qWarning() << "[ContainerListWidget] Attempted to initialize more than once. This is unsupported.";
        return;
    }

    for (const ContainerDefinition &spec : containers)
    {
        ContainerRowWidget *row = new ContainerRowWidget(
            spec.label(),
            spec.icon(),
            spec.hasAction(),
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
                    qInfo() << "[UI] Requesting to `" << (start ? "start" : "stop") << "` container " << containerName;
                    emit containerToggle(containerName, start);
                });

        connect(row,
                &ContainerRowWidget::adminRequested,
                this,
                [this, containerName]()
                {
                    qInfo() << "[UI] Requesting to run admin command from " << containerName;
                    emit containerAction(containerName);
                });

        // Include created widget in layout
        ui->contentsLayout->addWidget(row);
    }

    ui->loading->hide();
    ui->contentsLayout->addStretch();
    m_initialized = true;
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
            qWarning() << "[UI] The corresponding widget for '" << container.name() << "'' was not found. Status will not be displayed.";
            continue;
        }

        it.value() // ContainerRowWidget
            ->setStatus(mapStatus(container.status()));
    }
}

ContainerRowWidget::Status
ContainerListWidget::mapStatus(ContainerState::Status status)
{
    switch (status)
    {

    case ContainerState::Status::Running:
        return ContainerRowWidget::Status::Running;

    case ContainerState::Status::Created:
    case ContainerState::Status::Exited:
        return ContainerRowWidget::Status::Stopped;
    case ContainerState::Status::Restarting:
        return ContainerRowWidget::Status::Loading;

    case ContainerState::Status::Paused:
    case ContainerState::Status::Removing:
    case ContainerState::Status::Dead:
    case ContainerState::Status::Unknown:
    default:
        return ContainerRowWidget::Status::Error;
    }
}
