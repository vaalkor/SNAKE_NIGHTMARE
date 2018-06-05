#ifndef GAMEOBJECTSANDDATA_H
#define GAMEOBJECTSANDDATA_H

#define MAX_NUM_PLAYERS 20
#define TIMER_LENGTH 3
#define MAX_NAME_LENGTH 20
#define MIN_TICKRATE 33
#define MAX_TICKRATE 66

#include <vector>
#include <QRgb>
#include <cstring>
#include <QDataStream>

extern const std::vector<QRgb> playerColors;

class PlayerInfo
{
public:
    unsigned char playerID;
    QRgb color;
    char name[MAX_NAME_LENGTH+1]; //+1 for the null terminator
    bool alive = true;
    bool inCurrentGame = false;

    PlayerInfo();
    PlayerInfo(unsigned char playerID_, QRgb color_, char* name_);
};

class GameState
{
public:
    bool gameInProgress = false;
    short numPlayers = 0;
    float sprintMeter = 1000; //set this to sprint length later on mate... yihyihyihm8
    float bombCharge = 0;
    short wallEncroachment = 0;
};

class GameParameters
{
public:
    int width = 100;
    int height = 100;

    short tickLength = 33;              //how many milliseconds to wait between frames. The lower, the faster the game...

    bool sprintEnabled = true;
    bool bombsEnabled = true;

    int sprintLength = 1000;            //total milliseconds of sprinting in the sprint bar...              (1 second currently)
    int sprintRechargeLength = 20000;   //length of time in milliseconds to recharge an entire sprint bar   (20 seconds currently)

    int bombRadius = 4;
    int bombChargeTime = 10000;

    bool revengeModeEnabled = false;
    bool PUBGmodeEnabled = false;

    int PUBGWallIncrease = 2;
    int PUBGCircleTime = 2500;

    int sizeInBytes();
};

QDataStream& operator<<(QDataStream &out, GameParameters &obj);
QDataStream& operator>>(QDataStream &in,  GameParameters &obj);

enum class MessageType : unsigned char
{
    PLAYER_CONNECTED,
    PLAYER_DISCONNECTED,
    PLAYER_ID_ASSIGNMENT,
    NOTIFY_SERVER_OF_PLAYER_NAME,
    BATTLE_ROYALE_WALL_UPDATE,
    POSITION_UPDATE,
    BOMB_ACTIVATION,
    PLAYER_DIED,
    PLAYER_WON,
    GAME_BEGIN,
    GAME_STOPPED,
    TIMER_UPDATE,
    START_POSITION,
    GAME_PARAMETERS,
    COUNT //this is a cheeky way of accessing the number of enum entries in code.
};

#endif // GAMEOBJECTSANDDATA_H
