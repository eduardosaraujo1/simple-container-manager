#include "containerlistwidget.h"
#include "ui_containerlistwidget.h"
#include "containerrowwidget.h"

ContainerListWidget::ContainerListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ContainerListWidget)
{
    ui->setupUi(this);
    for (int i=0; i < 5; ++i) {
        ui->contentsLayout->addWidget(new ContainerRowWidget(
            ":/images/refresh.svg",
            "Oracle XE 11g",
            ContainerRowWidget::Status::Running,
            "firefox",
            this
            ));
        ui->contentsLayout->addWidget(new ContainerRowWidget(
            ":/images/refresh.svg",
            "MySQL",
            ContainerRowWidget::Status::Stopped,
            "firefox",
            this
            ));
        ui->contentsLayout->addWidget(new ContainerRowWidget(
            ":/images/refresh.svg",
            "Apache",
            ContainerRowWidget::Status::Error,
            "firefox",
            this
            ));
    }
}

ContainerListWidget::~ContainerListWidget()
{
    delete ui;
}
