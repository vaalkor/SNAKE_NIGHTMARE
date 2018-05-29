#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H

#include <QObject>
#include "mainwindow.h"
#include <QtNetwork/QUdpSocket>
#include <QTcpSocket>
#include <QMutex>
#include <QWaitCondition>
#include <tcpserver.h>
#include <random>

class MainWindow;

class ClientWorker : public QObject
{
    Q_OBJECT
public:
    explicit ClientWorker(bool testingMode_=false, unsigned int seed_=0, QObject *parent = nullptr);

    GameInfo gameInfo;

    bool isGameOver = false;
    bool testingMode = false;

    int xDirDirections[4] = {0, 1, 0, -1};
    int yDirDirections[4] = {-1, 0, 1, 0};
    int currentDirection = 0;
    unsigned int seed = 0;

    std::mt19937 gen;
    std::uniform_real_distribution<> dist;

    bool gameInProgress = false;
    bool kill = false;

    QMutex mutex;
    QWaitCondition waitCondition;

    short xPos = 50;
    short yPos = 50;
    int xDir = 1;
    int yDir = 0;
    unsigned char playerID = 0;

    unsigned int maintainDirectionLength = 250;
    unsigned int timeSinceLastDirectionChange = 0;

    bool inFocus = true;

    unsigned char tailArray[100][100]; //this is constant size at the moment but it will end up being of variable size mate..
    //bool **tailArray;

    void drawSquare(short x, short y, QRgb color);
    void draw(bool endGame);

    ~ClientWorker();

signals:
    void sendPosition(short x, short y);
    void drawSignal();
    void sendKillAcknowledgement();

public slots:
    void process();
    void focusChanged(bool value);
    void receivePositionSlot(short x,short y);
};

#endif // CLIENTWORKER_H
