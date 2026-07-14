#include "containerrow.h"
#include "ui_containerrow.h"

ContainerRow::ContainerRow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ContainerRow)
{
    ui->setupUi(this);
}

ContainerRow::~ContainerRow()
{
    delete ui;
}
