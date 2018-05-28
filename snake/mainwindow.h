#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include <QImage>
#include <QPen>
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
    explicit MainWindow(bool isServer_, bool testingMode_ = false, QHostAddress serverAddress_ = QHostAddress("000.000.000.000"), QWidget *parent = 0);

    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void closeEvent(QCloseEvent *event);

    bool isServer;
    bool testingMode;
    QHostAddress serverAddress;

    QThread *thread;
    ClientWorker *clientWorker;
    ServerWorker *serverWorker;

    tcpServer *server;
    tcpClient *client;

    void draw(bool endGame);
    void drawSquare(short x, short y, QRgb color);

    virtual bool event(QEvent *e);

    QImage image;
    QPen pen;

    ~MainWindow();
signals:
    focusChanged(bool value);
public slots:
    void serverReceivePositionSlot(short x, short y);
    void clientReceivePositionSlot(short x, short y);
    void drawSlot();
    void receivedKillAcknowledgement();
    void clientConnectionSuccess();
    void clientConnectionFailure(QAbstractSocket::SocketError err);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
