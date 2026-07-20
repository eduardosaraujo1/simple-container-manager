#include "containerlistwidget.h"
#include "ui_containerlistwidget.h"

ContainerListWidget::ContainerListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ContainerListWidget)
{
    ui->setupUi(this);
}

void ContainerListWidget::initialize(
    const QList<AppPreferences::ContainerSpec> &containers)
{
    if (m_initialized) {
        qWarning() << "ContainerListWidget::initialize() called more than once.";
        return;
    }

    ui->loading->hide();

    for (const auto &spec : containers) {

        auto *row = new ContainerRowWidget(
            spec.icon,
            spec.label,
            ContainerRowWidget::Status::Loading,
            spec.action,
            this);

        m_rows.insert(spec.name, row);

        const QString containerName = spec.name;

        connect(row,
                &ContainerRowWidget::toggleRequested,
                this,
                [this, containerName](bool start) {
                    emit containerToggle(containerName, start);
                });

        connect(row,
                &ContainerRowWidget::adminRequested,
                this,
                [this, containerName](const QString &action) {
                    emit containerAction(containerName, action);
                });

        ui->contentsLayout->addWidget(row);
    }

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
    for (const auto &row : std::as_const(m_rows))
        row->setStatus(ContainerRowWidget::Status::Stopped);

    for (const auto &container : containers) {

        auto it = m_rows.find(container.name());

        if (it == m_rows.end())
            // container widget not found
            qWarning() << "refreshContainerInfo: the corresponding widget for '" << container.name() << "'' was not found. Status will not be displayed.";
            continue;

        it.value() // ContainerRowWidget
            ->setStatus(mapStatus(container.status()));
    }
}

ContainerRowWidget::Status
ContainerListWidget::mapStatus(ContainerInfo::Status status)
{
    switch (status) {

    case ContainerInfo::Status::Running:
        return ContainerRowWidget::Status::Running;

    case ContainerInfo::Status::Created:
    case ContainerInfo::Status::Exited:
        return ContainerRowWidget::Status::Stopped;

    case ContainerInfo::Status::Paused:
    case ContainerInfo::Status::Restarting:
    case ContainerInfo::Status::Removing:
    case ContainerInfo::Status::Dead:
    case ContainerInfo::Status::Unknown:
    default:
        return ContainerRowWidget::Status::Error;
    }
}