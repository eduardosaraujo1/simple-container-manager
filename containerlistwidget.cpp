#include "containerlistwidget.h"
#include "ui_containerlistwidget.h"
#include "containerrowwidget.h"

ContainerListWidget::ContainerListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ContainerListWidget)
{
    ui->setupUi(this);
    for (int i=0; i < 100; ++i) {
        ui->contentsLayout->addWidget(new ContainerRowWidget());
    }
}

ContainerListWidget::~ContainerListWidget()
{
    delete ui;
}
