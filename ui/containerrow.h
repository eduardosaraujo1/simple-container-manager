#ifndef CONTAINERROW_H
#define CONTAINERROW_H

#include <QWidget>

namespace Ui {
class ContainerRow;
}

class ContainerRow : public QWidget
{
    Q_OBJECT

public:
    explicit ContainerRow(QWidget *parent = nullptr);
    ~ContainerRow();

private:
    Ui::ContainerRow *ui;
};

#endif // CONTAINERROW_H
