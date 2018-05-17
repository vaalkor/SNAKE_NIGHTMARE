#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>

enum class MessageType : unsigned char
{
    PLAYER_CONNECTED,
    PLAYER_DISCONNECTED,
    POSITION_UPDATE,
    BOMB_ACTIVATION,
    PLAYER_DIED,
    PLAYER_WON,
    GAME_BEGIN
};

class tcpServer : public QObject
{
    Q_OBJECT
public:
    explicit tcpServer(QObject *parent = nullptr);

    QTcpServer *server;
    QTcpSocket *client = NULL;
    int count = 0;

    void connect();

signals:
    void drawPosition(int x, int y);
public slots:
    void handleConnection();
    void sendDataToClient();
    void readyRead();
private:
    QDataStream in;
    QDataStream out;
};

#endif // TCPSERVER_H
