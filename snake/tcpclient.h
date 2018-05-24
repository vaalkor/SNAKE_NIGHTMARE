#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDataStream>
#include <QKeyEvent>

class tcpClient : public QObject
{
    Q_OBJECT
public:
    explicit tcpClient(QObject *parent = nullptr);
    //QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    QTcpSocket *tcpSocket;
    bool connected = false;

    unsigned int clientID;

    void sendTcpMessage();

signals:
    void receivePosition(int x, int y);

public slots:
    void sendPosition(int x, int y);
    void connect();
    void readyRead();
    void readyReadTcp();
    void processPendingDatagrams();

private:
    QDataStream in;
    QDataStream out;
};

#endif // TCPCLIENT_H

