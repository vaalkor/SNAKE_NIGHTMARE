#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDataStream>
#include <QKeyEvent>
#include <QHostAddress>
#include "tcpserver.h"
#include <QHash>

class tcpClient : public QObject
{
    Q_OBJECT
public:
    explicit tcpClient(QHostAddress address_, QObject *parent = nullptr);

    QUdpSocket *udpSocket;
    QTcpSocket *tcpSocket;
    bool connected = false;

    QHash<unsigned char, PlayerInfo> playerList;

    QHostAddress address;

    unsigned char clientID;

    void sendTcpMessage();
    void sendName(std::string name);

signals:
    void receivePositionSignal(unsigned char clientID, short x, short y);
    void startGameSignal();
    void gameOverSignal();
    void stopGameSignal();

public slots:
    void sendPosition(short x, short y);
    void connect();
    void readyReadTcp();
    void readyReadUdp();

private:
    QDataStream in;
    QDataStream out;
};

#endif // TCPCLIENT_H

