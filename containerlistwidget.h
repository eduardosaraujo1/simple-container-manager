#ifndef CONTAINERLISTWIDGET_H
#define CONTAINERLISTWIDGET_H

#include <QWidget>

namespace Ui {
class ContainerListWidget;
}

class ContainerListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContainerListWidget(QWidget *parent = nullptr);
    ~ContainerListWidget();

private:
    Ui::ContainerListWidget *ui;
};

#endif // CONTAINERLISTWIDGET_H
