#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setText_to_List(a.arguments());
    w.show();

    return a.exec();
}
