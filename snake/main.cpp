#include "mainwindow.h"
#include <QApplication>

#include <QThread>
#include "serverclientselection.h"
#include <QHash>
#include <QDebug>
#include <QRgb>
#include <QChar>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerClientSelection w;

    qDebug() << "sizeof qchar" <<sizeof(QChar);
    w.show();

    return a.exec();
}
