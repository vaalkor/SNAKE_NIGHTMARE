#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include <QImage>
#include <QGraphicsScene>
#include <QPainter>
#include <QRgb>

#include "clientworker.h"
#include "serverworker.h"
#include "tcpclient.h"
#include "tcpserver.h"

namespace Ui {
class MainWindow;
}

class ClientWorker;
class ServerWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool isServer_, QWidget *parent = 0);

    virtual void paintEvent(QPaintEvent *event);

    bool isServer;

    QThread *thread;
    ClientWorker *clientWorker;
    ServerWorker *serverWorker;

    tcpServer *server;
    tcpClient *client;

    QImage image;
    ~MainWindow();
public slots:

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
