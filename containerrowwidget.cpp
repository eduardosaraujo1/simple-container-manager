#include "containerrowwidget.h"
#include "ui_containerrowwidget.h"

ContainerRowWidget::ContainerRowWidget(
    const QString& iconPath,
    const QString& label,
    Status status,
    const QString& action,
    QWidget *parent
)
    : QWidget(parent),
      ui(new Ui::ContainerRowWidget),
      m_status(status),
      m_action(action)
{
    ui->setupUi(this);

    setIcon(iconPath);
    setLabel(label);
    setStatus(status);

    connect(ui->toggle, &QPushButton::clicked,
            this, &ContainerRowWidget::onToggleClicked);

    connect(ui->admin, &QPushButton::clicked,
            this, &ContainerRowWidget::onAdminClicked);
}

ContainerRowWidget::~ContainerRowWidget()
{
    delete ui;
}

void ContainerRowWidget::onToggleClicked()
{
    bool shouldStart = m_status != Status::Running;

    emit toggleRequested(shouldStart);
}

void ContainerRowWidget::onAdminClicked()
{
    emit adminRequested(m_action);
}

void ContainerRowWidget::setAction(const QString& action) {
    m_action = action;
}

void ContainerRowWidget::setLabel(const QString& label) {
    ui->label->setText(label);
}

void ContainerRowWidget::setIcon(const QString& iconPath)
{
    QPixmap p(iconPath);
    p.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->icon->setPixmap(p);
}

void ContainerRowWidget::setStatus(Status status)
{
    m_status = status;

    switch(status)
    {
    case Status::Running:
        ui->status->setText("Running");
        ui->toggle->setText("Stop");
        break;

    case Status::Stopped:
        ui->status->setText("Stopped");
        ui->toggle->setText("Start");
        break;

    case Status::Error:
        ui->status->setText("Error");
        ui->toggle->setText("Start");
        break;
    }

    updateStatusStyle();
}

void ContainerRowWidget::updateStatusStyle()
{
    switch(m_status)
    {
    case Status::Running:
        ui->status->setStyleSheet(
            "color: #008000;"
            "background-color: #C6EFCE;"
        );
        break;

    case Status::Stopped:
        ui->status->setStyleSheet(
            "color: #9C6500;"
            "background-color: #FFEB9C;"
        );
        break;

    case Status::Error:
        ui->status->setStyleSheet(
            "color: #9C0006;"
            "background-color: #FFC7CE;"
        );
        break;
    }
}