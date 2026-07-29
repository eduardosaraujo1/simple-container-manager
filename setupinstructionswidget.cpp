#include "setupinstructionswidget.h"
#include "ui_setupinstructionswidget.h"

SetupInstructionsWidget::SetupInstructionsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SetupInstructionsWidget)
{
    ui->setupUi(this);
}

SetupInstructionsWidget::~SetupInstructionsWidget()
{
    delete ui;
}
