#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include "mainwindow.h"
#include <QtNetwork/QUdpSocket>
#include <QTcpSocket>
#include <QMutex>
#include <QWaitCondition>
#include "servercontrolwindow.h"

class MainWindow;

class ServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit ServerWorker(QObject *parent = nullptr);
    MainWindow *w;
    ServerControlWindow *serverWindow;

    bool isGameOver = false;
    bool gameInProgress = false;

    QMutex mutex;
    QWaitCondition waitCondition;

    int xPos = 0;
    int yPos = 0;
    bool checkCollisions();

    void drawSquare(short x, short y, QRgb color);
    void draw(bool endGame);

    bool tailArray[100][100]; //this is constant size at the moment but it will end up being of variable size mate..

signals:

public slots:
    void process();
};

#endif // SERVER WORKER_H
