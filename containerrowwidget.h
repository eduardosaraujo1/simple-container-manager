#ifndef CONTAINERROWWIDGET_H
#define CONTAINERROWWIDGET_H

#include <QWidget>

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
