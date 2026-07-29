#include "containerrowwidget.h"
#include "ui_containerrowwidget.h"

ContainerRowWidget::ContainerRowWidget(
    const QString &label,
    const QString &icon,
    bool hasAction,
    QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ContainerRowWidget)
{
    ui->setupUi(this);

    connect(ui->toggle, &QPushButton::clicked,
            this, &ContainerRowWidget::onToggleClicked);

    connect(ui->admin, &QPushButton::clicked,
        this, &ContainerRowWidget::onAdminClicked);

    setLabel(label);
    setIcon(icon);
    setHasAction(hasAction);
    setStatus(Status::Loading);
}

ContainerRowWidget::~ContainerRowWidget()
{
    delete ui;
}

void ContainerRowWidget::onToggleClicked()
{
    bool shouldStart = m_status != Status::Started;

    emit toggleRequested(shouldStart);
}

void ContainerRowWidget::onAdminClicked()
{
    emit adminRequested();
}

void ContainerRowWidget::setLabel(const QString &label)
{
    ui->label->setText(label);
}

void ContainerRowWidget::setIcon(const QString &iconPath)
{
    QString path = iconPath.trimmed();

    if (path.isEmpty())
    {
        qInfo() << "[UI] No icon specified. Loading default...";
        path = ":/images/terminal.svg";
    }

    QPixmap pixmap(path);

    if (pixmap.isNull())
    {
        qInfo() << "[UI] Icon file not found. Loading default...";
        pixmap.load(":/images/terminal.svg");
    }

    ui->icon->setPixmap(
        pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ContainerRowWidget::setHasAction(bool hasAction) {
    ui->admin->setEnabled(hasAction);
}

void ContainerRowWidget::setStatus(Status status)
{
    m_status = status;

    switch (status)
    {
    case Status::Started:
        ui->status->setText("Running");
        ui->toggle->setText("Stop");
        ui->toggle->setDisabled(false);
        break;

    case Status::Stopped:
        ui->status->setText("Stopped");
        ui->toggle->setText("Start");
        ui->toggle->setDisabled(false);
        break;

    case Status::NotFound:
        ui->status->setText("Not Found");
        ui->toggle->setText("Start");
        ui->toggle->setDisabled(false);
        break;
    case Status::Loading:
        ui->status->setText("Loading");
        ui->toggle->setText("Start");
        ui->toggle->setDisabled(true);
        break;
    case Status::Starting:
        ui->status->setText("Starting...");
        ui->toggle->setText("Start");
        ui->toggle->setDisabled(true);
        break;
    case Status::Stopping:
        ui->status->setText("Stopping...");
        ui->toggle->setText("Start");
        ui->toggle->setDisabled(true);
        break;
    }

    updateStatusStyle();
}

void ContainerRowWidget::updateStatusStyle()
{
    switch (m_status)
    {
    case Status::Started:
        ui->status->setStyleSheet(
            "color: #008000;"
            "background-color: #C6EFCE;");
        break;

    case Status::Stopped:
        ui->status->setStyleSheet(
            "color: #9C6500;"
            "background-color: #FFEB9C;");
        break;

    case Status::NotFound:
        ui->status->setStyleSheet(
            "color: #9C0006;"
            "background-color: #FFC7CE;");
        break;

    case Status::Loading:
        ui->status->setStyleSheet(
            "color: #404040;"
            "background-color: #E0E0E0;");
        break;

    case Status::Starting:
        ui->status->setStyleSheet(
            "color: #0C5460;"
            "background-color: #D1ECF1;");
        break;

    case Status::Stopping:
        ui->status->setStyleSheet(
            "color: #8A4B08;"
            "background-color: #FCE5CD;");
        break;
    }
}
