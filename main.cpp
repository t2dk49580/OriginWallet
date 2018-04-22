#include "MainWindow.h"
#include <QApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QInputDialog>
#include <QTranslator>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator traslator;
    traslator.load(":/wallet.qm");
    a.installTranslator(&traslator);
    MainWindow w;
    w.show();
    return a.exec();
}
