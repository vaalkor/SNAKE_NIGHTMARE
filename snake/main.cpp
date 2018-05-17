#include "mainwindow.h"
#include <QApplication>

#include "clientworker.h"
#include <QThread>
#include "serverclientselection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerClientSelection w;
    //MainWindow w;
    w.show();

    return a.exec();
}
