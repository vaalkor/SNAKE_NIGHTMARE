#ifndef GAMEOBJECTSANDDATA_H
#define GAMEOBJECTSANDDATA_H

#define MAX_NUM_PLAYERS 20
#define TIMER_LENGTH 3

#include <vector>
#include <QRgb>
#include <cstring>

extern const std::vector<QRgb> playerColors;

class PlayerInfo
{
public:
    unsigned char playerID;
    QRgb color;
    char name[21];
    bool alive = true;

    PlayerInfo();
    PlayerInfo(unsigned char playerID_, QRgb color_, char* name_);
};

class GameState
{
public:
    bool gameInProgress = false;
    short numPlayers = 0;
};

class GameParameters
{
public:
    int width = 100;
    int height = 100;

    bool enableSprint = true;
    bool enableBombs = true;

    bool normalMode = true;
    bool revengeMode = false;
    bool PUBGmode = false;
};

enum class MessageType : unsigned char
{
    PLAYER_CONNECTED,
    PLAYER_DISCONNECTED,
    PLAYER_ID_ASSIGNMENT,
    NOTIFY_SERVER_OF_PLAYER_NAME,
    POSITION_UPDATE,
    BOMB_ACTIVATION,
    PLAYER_DIED,
    PLAYER_WON,
    GAME_BEGIN,
    GAME_STOPPED,
    TIMER_UPDATE,
    START_POSITION,
    GAME_INFO,
    COUNT //this is a cheeky way of accessing the number of enum entries in code.
};

#endif // GAMEOBJECTSANDDATA_H
