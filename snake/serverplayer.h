#ifndef SERVERPLAYER_H
#define SERVERPLAYER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>

#include "player.h"
#include "servercontrolwindow.h"
#include "gameobjectsanddata.h"


class ServerPlayer : public Player
{
    Q_OBJECT
public:
    ServerPlayer(QObject *parent = 0);

    ServerControlWindow *serverWindow;

    QTcpServer *server;
    QUdpSocket *clientUdp;

    QTimer battleRoyaleTimer;

    unsigned int clientIDCounter = 0;
    short startGameCounter;

    unsigned char playerPositionGrid[4][5];
    QHash<QTcpSocket*, unsigned char> idList;

    std::vector<QTcpSocket *> clients;

    void sendGameParameters();
    void sendDeathSignal(unsigned char ID);
    void calculateStartingPosition(short &x, short &y);
    void sendStartingPositions();
    void gameOver(unsigned char winnerID);
    void sendWinSignal(unsigned char ID);
    void checkWinConditions();
    void sendBombMessage(short x, short y);
    void sendTcpBlock(QByteArray &block);

    void sendPosition(unsigned char ID, short x, short y) override;
    void receivePosition(unsigned char ID, short x, short y) override;
    void resetGameState();

    ~ServerPlayer();

signals:

public slots:
    void readyReadUdp() override;
    void readyReadTcp() override;
    void handleConnection();
    void clientDisconnected();
    void startGameCounterSlot();
    void iterateStartGameCounter();
    void manageBattleRoyaleMode();
};

#endif // SERVERPLAYER_H
