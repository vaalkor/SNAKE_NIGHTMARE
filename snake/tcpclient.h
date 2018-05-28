#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDataStream>
#include <QKeyEvent>
#include <QHostAddress>

class tcpClient : public QObject
{
    Q_OBJECT
public:
    explicit tcpClient(QHostAddress address_, QObject *parent = nullptr);
    //QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    QTcpSocket *tcpSocket;
    bool connected = false;

    QHostAddress address;

    unsigned int clientID;

    void sendTcpMessage();

signals:
    void receivePositionSignal(short x, short y);

public slots:
    void sendPosition(short x, short y);
    void connect();
    void readyRead();
    void readyReadTcp();
    void processPendingDatagrams();

private:
    QDataStream in;
    QDataStream out;
};

#endif // TCPCLIENT_H

