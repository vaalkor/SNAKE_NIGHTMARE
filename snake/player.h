#ifndef PLAYER_H
#define PLAYER_H

#include <QOBject>
#include <random>
#include <QHash>
#include <QTimer>
#include <QByteArray>
#include "gameobjectsanddata.h"
//#include <   include tcp server... for game info...

class Player : public QObject
{
    Q_OBJECT
public:
    Player(QObject *parent = 0);

    GameInfo gameInfo;

    bool gameInProgress = false;

    QTimer timer;

    std::mt19937 gen;
    std::uniform_real_distribution<> dist;

    QHash<unsigned char, PlayerInfo> playerList;

    unsigned char tailArray[100][100]; //this is constant size at the moment but it will end up being of variable size mate..

    void resizeTailArray();
    virtual void sendPosition(unsigned char ID, short x, short y) = 0;
    virtual void receivePosition(unsigned char ID, short x, short y) = 0;
signals:
    void drawSignal();
public slots:
    virtual void readyReadUdp() = 0;
    virtual void readyReadTcp() = 0;
protected:
    QByteArray block;
};

#endif // PLAYER_H
