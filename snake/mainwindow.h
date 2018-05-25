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
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void closeEvent(QCloseEvent *event);

    bool isServer;

    QThread *thread;
    ClientWorker *clientWorker;
    ServerWorker *serverWorker;

    tcpServer *server;
    tcpClient *client;

    void draw(bool endGame);
    void drawSquare(int x, int y, QRgb color);

    virtual bool event(QEvent *e);

    QImage image;
    ~MainWindow();
signals:
    focusChanged(bool value);
public slots:
    void receivePositionSlot(int x, int y);
    void drawSlot();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
