#include "containerrowwidget.h"
#include "ui_containerrowwidget.h"

ContainerRowWidget::ContainerRowWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ContainerRowWidget)
{
    ui->setupUi(this);
}

ContainerRowWidget::~ContainerRowWidget()
{
    delete ui;
}
