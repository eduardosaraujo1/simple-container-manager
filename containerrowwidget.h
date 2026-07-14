#ifndef CONTAINERROWWIDGET_H
#define CONTAINERROWWIDGET_H

#include <QWidget>

// SHould:
// Be instantiated with values (icon path, label, status and action string)
// Action string calls domain/ActionRunner
// Have a mutator method for each attribute, and update the style accordingly
namespace Ui {
class ContainerRowWidget;
}

class ContainerRowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContainerRowWidget(QWidget *parent = nullptr);
    ~ContainerRowWidget();

private:
    Ui::ContainerRowWidget *ui;
};

#endif // CONTAINERROWWIDGET_H
