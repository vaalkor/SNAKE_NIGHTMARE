#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include "mainwindow.h"
#include <QtNetwork/QUdpSocket>
#include <QTcpSocket>
#include <QMutex>
#include <QWaitCondition>
#include "servercontrolwindow.h"
#include "tcpserver.h"
#include <QHash>

class MainWindow;

class ServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit ServerWorker(QObject *parent = nullptr);

    ServerControlWindow serverWindow;

    GameInfo lobbyGameInfo; //this stores the temporary settings chosen in lobby
    GameInfo currentGameInfo; //this stores the settings for the duration of a game which were set when the game was started

    bool isGameOver = false;
    bool gameInProgress = false;

    QMutex mutex;
    QWaitCondition waitCondition;

    bool checkCollisions();

    void drawSquare(short x, short y, QRgb color);
    void draw(bool endGame);

    unsigned char tailArray[100][100]; //this is constant size at the moment but it will end up being of variable size mate..

signals:

public slots:
    void process();
};

#endif // SERVER WORKER_H
