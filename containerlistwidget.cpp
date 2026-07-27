#include "containerlistwidget.h"
#include "ui_containerlistwidget.h"

ContainerListWidget::ContainerListWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::ContainerListWidget)
{
    ui->setupUi(this);
}

void ContainerListWidget::initialize(
    const QList<ContainerSpec> &containers)
{
    if (m_initialized)
    {
        qWarning() << "[ContainerListWidget] Attempted to initialize more than once. This is unsupported.";
        return;
    }

    for (const ContainerSpec &spec : containers)
    {
        ContainerRowWidget *row = new ContainerRowWidget(
            spec.label(),
            this);

        const QString containerName = spec.name();

        // Associate the container row pointer to its parent container
        m_rows.insert(containerName, row);

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
    const QList<ContainerInfo> &containers)
{
    for (const ContainerInfo &container : containers)
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
ContainerListWidget::mapStatus(ContainerInfo::Status status)
{
    switch (status)
    {

    case ContainerInfo::Status::Running:
        return ContainerRowWidget::Status::Running;

    case ContainerInfo::Status::Created:
    case ContainerInfo::Status::Exited:
        return ContainerRowWidget::Status::Stopped;
    case ContainerInfo::Status::Restarting:
        return ContainerRowWidget::Status::Loading;

    case ContainerInfo::Status::Paused:
    case ContainerInfo::Status::Removing:
    case ContainerInfo::Status::Dead:
    case ContainerInfo::Status::Unknown:
    default:
        return ContainerRowWidget::Status::Error;
    }
}