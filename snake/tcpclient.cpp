#include "tcpclient.h"

#include <QTcpServer>
#include <QtNetwork>
#include <iostream>
//#include <QtNetwork/QHostAddress>
tcpClient::tcpClient(QObject *parent) : QObject(parent)
{
    //tcpSocket = new QTcpSocket(this);
    udpSocket = new QUdpSocket(this);

    //QObject::connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    //in.setDevice(tcpSocket);
    //in.setVersion(QDataStream::Qt_4_0);

}

void tcpClient::connect()
{
    if(!connected)
    {
        qDebug() << "client connect slot triggered son...\n";
        std::cout << "client connect method called mate...\n";
        tcpSocket->abort();
        tcpSocket->connectToHost("192.168.0.118", 1234);
        connected  = true;
    }

}

void tcpClient::sendPosition(int x, int y)
{
    qDebug() << "sending position mate x/y" << x << "/" << y;
    /*QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << x << y;

    tcpSocket->write(block);*/

    QByteArray::number arr1(x);
    arr1 += QByteArray::number(y);
    udpSocket->writeDatagram(arr1.data(), arr1.size(), "192.168.0.118", 6666);

}

void tcpClient::readyRead()
{
    qDebug() << "In the readyread function mate\n";
    in.startTransaction();

    int x;
    in >> x;
    qDebug() << "x: " << x;

    if(!in.commitTransaction())
        return;
    //in.
}
