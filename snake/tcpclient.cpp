#include "tcpclient.h"

#include <QTcpServer>
#include <QtNetwork>
#include <iostream>
#include <QHostAddress>
#include "tcpserver.h"
//#include <QtNetwork/QHostAddress>
tcpClient::tcpClient(QObject *parent) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(1234);

    QObject::connect(tcpSocket, SIGNAL(readyRead()), this, SLOT( readyReadTcp()) );
    QObject::connect(udpSocket, SIGNAL(readyRead()), this, SLOT( processPendingDatagrams()) );

    connect(); //connect to server over tcp... for important messages that need guaranteed delivery.

    //in.setDevice(tcpSocket);
    //in.setVersion(QDataStream::Qt_4_0);

}

void tcpClient::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        int x,y;
        QDataStream inblock(&datagram, QIODevice::ReadOnly);
        inblock >> x;
        inblock >> y;
        //qDebug() << x << "/" << y;
        emit receivePositionSignal(x,y);
    }
}

void tcpClient::connect()
{
    tcpSocket->connectToHost("192.168.0.118", 6666);
    connected  = true;
}

void tcpClient::sendPosition(int x, int y)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << x;
    stream << y;

    udpSocket->writeDatagram(buffer.data(), buffer.size(), QHostAddress("192.168.0.118"), 6666);

}

void tcpClient::sendTcpMessage()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    unsigned char message = qrand() % (unsigned int)MessageType::COUNT;
    out << message;

    tcpSocket->write(block);
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
}

void tcpClient::readyReadTcp()
{

}
