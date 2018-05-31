#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>
#include <QByteArray>
#include <QUdpSocket>
#include <QKeyEvent>
#include <vector>
#include <QRgb>
#include <QHash>
#include <chrono>
#include <random>
#include <QTimer>

#define MAX_NUM_PLAYERS 20

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

class GameInfo
{
public:
    int width = 100;
    int height = 100;

    bool enableSprint = true;
    bool enableBombs = true;

    bool normalMode = true;
    bool revengeMode = false;
    bool PUBGmode = false;

    short numPlayers = 0;

    bool gameInProgress = false;

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

class tcpServer : public QObject
{
    Q_OBJECT
public:
    explicit tcpServer(QObject *parent = nullptr);

    GameInfo info;

    QTcpServer *server;
    QUdpSocket *clientUdp = NULL;
    int count = 0;
    unsigned int clientIDCounter = 0;

    QTimer timer;
    short startGameCounter;

    unsigned char playerPositionGrid[4][5];

    std::mt19937 gen;
    std::uniform_real_distribution<> dist;

    QHash<unsigned char, PlayerInfo> playerList;
    QHash<QTcpSocket*, unsigned char> idList;

    std::vector<QTcpSocket *> clients;

    void keyPressEvent(QKeyEvent *event);
    void sendTcpMessage();
    void sendDeathSignal(unsigned char clientID);
    void calculateStartingPosition(short &x, short &y);

signals:
    void receivePositionSignal(unsigned char clientID, short x, short y);
    void updateServerUI();
    void startGameSignal();
public slots:
    void handleConnection();
    void readyReadTcp();
    void readyReadUdp();
    void clientDisconnected();
    void gameOver(unsigned char winnerID);
    void stopGame();
    void sendPositionToAllClients(unsigned char clientID, short x, short y);
    void sendWinSignal(unsigned char clientID);
    void checkWinConditions();
    void iterateStartGameCounter();
    void startGameCounterSlot();
private:
    //QDataStream *out;
    QByteArray block;
};

#endif // TCPSERVER_H
