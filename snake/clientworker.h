#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H

#include <QObject>
#include "mainwindow.h"
#include <QtNetwork/QUdpSocket>
#include <QTcpSocket>
#include <QMutex>
#include <QWaitCondition>

class MainWindow;

class ClientWorker : public QObject
{
    Q_OBJECT
public:
    explicit ClientWorker(QObject *parent = nullptr);

    bool isGameOver = false;
    bool gameInPropress = false;
    bool kill = false;

    QMutex mutex;
    QWaitCondition waitCondition;

    short xPos = 50;
    short yPos = 50;
    short unsigned int clientID = 0;

    bool inFocus = true;

    bool tailArray[100][100]; //this is constant size at the moment but it will end up being of variable size mate..

    void drawSquare(short x, short y, QRgb color);
    void draw(bool endGame);

signals:
    void sendPosition(short x, short y);
    void drawSignal();
    void sendKillAcknowledgement();

public slots:
    //void readyRead();
    void process();
    void focusChanged(bool value);
    void receivePositionSlot(short x,short y);
};

#endif // CLIENTWORKER_H
