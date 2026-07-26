#include "containerrowwidget.h"
#include "ui_containerrowwidget.h"

ContainerRowWidget::ContainerRowWidget(
    const QString &iconPath,
    const QString &label,
    Status status,
    bool hasAction,
    QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ContainerRowWidget)
{
    ui->setupUi(this);

    setIcon(iconPath);
    setLabel(label);
    setStatus(status);

    connect(ui->toggle, &QPushButton::clicked,
            this, &ContainerRowWidget::onToggleClicked);

    if (hasAction)
    {
        connect(ui->admin, &QPushButton::clicked,
                this, &ContainerRowWidget::onAdminClicked);
    }

    ui->admin->setEnabled(hasAction);
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
    emit adminRequested();
}

void ContainerRowWidget::setAction(const QString &action)
{
    m_action = action;
}

void ContainerRowWidget::setLabel(const QString &label)
{
    if (!label.isEmpty())
    {
        ui->label->setText(label);
    }
    else
    {
        qWarning() << "[UI] Required field 'Label' is undefined.";
        ui->label->setText("Não definido.");
    }
}

void ContainerRowWidget::setIcon(const QString &iconPath)
{
    QString path = iconPath.trimmed();

    if (path.isEmpty())
    {
        path = ":/images/terminal.svg";
    }

    QPixmap pixmap(path);

    if (pixmap.isNull())
    {
        pixmap.load(":/images/terminal.svg");
    }

    ui->icon->setPixmap(
        pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
void ContainerRowWidget::setStatus(Status status)
{
    m_status = status;

    switch (status)
    {
    case Status::Running:
        ui->status->setText("Running");
        ui->toggle->setText("Stop");
        ui->toggle->setDisabled(false);
        break;

    case Status::Stopped:
        ui->status->setText("Stopped");
        ui->toggle->setText("Start");
        ui->toggle->setDisabled(false);
        break;

    case Status::Error:
        ui->status->setText("Error");
        ui->toggle->setText("Start");
        ui->toggle->setDisabled(false);
        break;
    case Status::Loading:
        ui->status->setText("Loading");
        ui->toggle->setDisabled(true);
        break;
    }

    updateStatusStyle();
}

void ContainerRowWidget::updateStatusStyle()
{
    switch (m_status)
    {
    case Status::Running:
        ui->status->setStyleSheet(
            "color: #008000;"
            "background-color: #C6EFCE;");
        break;

    case Status::Stopped:
        ui->status->setStyleSheet(
            "color: #9C6500;"
            "background-color: #FFEB9C;");
        break;

    case Status::Error:
        ui->status->setStyleSheet(
            "color: #9C0006;"
            "background-color: #FFC7CE;");
        break;
    case Status::Loading:
        ui->status->setStyleSheet(
            "color: #000000;"
            "background-color: #C6C6C6;");
        break;
    }
}