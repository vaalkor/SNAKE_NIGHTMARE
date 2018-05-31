#include "mainwindow.h"
#include <QApplication>

#include <QThread>
#include "serverclientselection.h"
#include <QHash>
#include <QDebug>
#include <QRgb>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerClientSelection w;
    w.show();

    return a.exec();
}
