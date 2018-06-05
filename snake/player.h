#ifndef PLAYER_H
#define PLAYER_H

#include <random>

#include <QOBject>
#include <QHash>
#include <QTimer>
#include <QByteArray>

#include "gameobjectsanddata.h"
//#include <   include tcp server... for game info...

//an id of zero indicates that it is empty...
class GridLocation
{
public:
    short id = 0;
    bool partOfWall = false;
};

class Player : public QObject
{
    Q_OBJECT
public:
    Player(QObject *parent = 0);

    GameState gameState;
    GameParameters gameParameters;

    QTimer timer;

    std::mt19937 gen;
    std::uniform_real_distribution<> dist;

    QHash<unsigned char, PlayerInfo> playerList;

    GridLocation tailArray[100][100]; //this is constant size at the moment but it will end up being of variable size mate..

    void triggerBomb(short x, short y);
    void resizeTailArray();
    void updateBattleRoyaleMode();

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
