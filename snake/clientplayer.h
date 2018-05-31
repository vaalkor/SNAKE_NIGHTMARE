#ifndef CLIENTPLAYER_H
#define CLIENTPLAYER_H

#include "windows.h"
#include "gameobjectsanddata.h"

#include <QObject>
#include "player.h"
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>

class ClientPlayer : public Player
{
    Q_OBJECT
public:
    ClientPlayer(QHostAddress address_, QObject *parent = 0);

    QTcpSocket tcpSocket;
    QUdpSocket udpSocket;
    unsigned char clientID;
    QHostAddress address;

    bool startGameTimerOnScreen = false;
    short startGameTimer = 3;

    bool inFocus = true;

    short xPos = 50;
    short yPos = 50;
    int xDir = 1;
    int yDir = 0;

    void sendName(std::string name);
    void sendPosition(unsigned char ID, short x, short y) override;
    void receivePosition(unsigned char ID, short x, short y) override;
    void receiveStartPosition(unsigned char ID, short x, short y);
    void sendPos(short x, short y);
    void connect();
    void startGame();
    void gameOver(unsigned char ID);

signals:

public slots:
    void iterateGameState();
    void readyReadUdp() override;
    void readyReadTcp() override;
};

#endif // CLIENTPLAYER_H
