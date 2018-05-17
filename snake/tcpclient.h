#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDataStream>

class tcpClient : public QObject
{
    Q_OBJECT
public:
    explicit tcpClient(QObject *parent = nullptr);
    //QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    bool connected = false;

signals:

public slots:
    void sendPosition(int x, int y);
    void connect();
    void readyRead();
private:
    QDataStream in;
    QDataStream out;
};

#endif // TCPCLIENT_H

