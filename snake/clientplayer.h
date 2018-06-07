#ifndef CLIENTPLAYER_H
#define CLIENTPLAYER_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QPoint>
#include <QKeyEvent>

#include "player.h"
#include "gameobjectsanddata.h"



class ClientPlayer : public Player
{
    Q_OBJECT
public:
    ClientPlayer(QHostAddress address_, QObject *parent = 0);

    QTcpSocket tcpSocket;
    QUdpSocket udpSocket;
    unsigned char clientID;
    QHostAddress address;
    QByteArray block;

    bool startGameTimerOnScreen = false;
    short startGameTimer = TIMER_LENGTH;

    bool drawBomb = false;
    QPoint bombPosition;

    bool printWinnerName = false;
    QString winnerName;
    short winnerID;
    bool inFocus = true;
    bool isSprinting = false;

    short xPos = 50;
    short yPos = 50;
    short tempxPos1=0; short tempyPos1=0;
    short tempxPos2=0; short tempyPos2=0;
    int xDir = 1;
    int yDir = 0;

    QHash<Qt::Key, bool> keysPressed;

    int tempCounter=0;

    void handleKeyPress(QKeyEvent *event);
    void handleKeyReleased(QKeyEvent *event);
    void releaseAllKeys();

    void resetGameState();
    void sendName(std::string name);
    void sendPosition(unsigned char ID, short x, short y) override;
    void receivePosition(unsigned char ID, short x, short y) override;

    void sendBombMessage();
    void receiveStartPosition(unsigned char ID, short x, short y);
    void sendPos(short x, short y);
    void connect();
    void startGame();
    void gameOver(unsigned char ID);
    void manageBomb();
    void manageSprint();

signals:

public slots:
    void iterateGameState();
    void readyReadUdp() override;
    void readyReadTcp() override;
};

#endif // CLIENTPLAYER_H
