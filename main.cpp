#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("windowsvista");

    MainWindow w;
    w.show();
    return QApplication::exec();
}
