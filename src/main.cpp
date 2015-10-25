#include <RInside.h>
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RInside r(argc,argv);
    MainWindow w(r);
    w.show();
    return a.exec();
}
