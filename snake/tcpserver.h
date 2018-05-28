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

enum class MessageType : unsigned char
{
    PLAYER_CONNECTED,
    PLAYER_DISCONNECTED,
    POSITION_UPDATE,
    BOMB_ACTIVATION,
    PLAYER_DIED,
    PLAYER_WON,
    GAME_BEGIN,
    TIMER_UPDATE,
    COUNT //this is a cheeky way of accessing the number of enum entries in code.
};

class tcpServer : public QObject
{
    Q_OBJECT
public:
    explicit tcpServer(QObject *parent = nullptr);

    QTcpServer *server;
    QUdpSocket *clientUdp = NULL;
    int count = 0;
    unsigned int clientIDCounter = 0;

    std::vector<QTcpSocket *> clients;

    void keyPressEvent(QKeyEvent *event);

signals:
    void receivePositionSignal(short x, short y);
public slots:
    void handleConnection();
    void readyRead();
    void processPendingDatagrams();
    void clientDisconnected();
private:
    QDataStream in;
    QDataStream out;
    QByteArray block;
};

#endif // TCPSERVER_H
