#include "MainWindow.h"
#include <QApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QInputDialog>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
