#ifndef SETUPINSTRUCTIONSWIDGET_H
#define SETUPINSTRUCTIONSWIDGET_H

#include <QWidget>

namespace Ui {
class SetupInstructionsWidget;
}

class SetupInstructionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SetupInstructionsWidget(QWidget *parent = nullptr);
    ~SetupInstructionsWidget();

private:
    Ui::SetupInstructionsWidget *ui;
};

#endif // SETUPINSTRUCTIONSWIDGET_H
