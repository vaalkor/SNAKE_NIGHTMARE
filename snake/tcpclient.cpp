#include "tcpclient.h"

#include <QTcpServer>
#include <QtNetwork>
#include <iostream>
#include <QHostAddress>
#include "tcpserver.h"

tcpClient::tcpClient(QHostAddress address_, QObject *parent) : address(address_), QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(1234);

    QObject::connect(tcpSocket, SIGNAL(readyRead()), this, SLOT( readyReadTcp()) );
    QObject::connect(udpSocket, SIGNAL(readyRead()), this, SLOT( processPendingDatagrams()) );

}

void tcpClient::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        short x,y;
        QDataStream inblock(&datagram, QIODevice::ReadOnly);
        inblock >> x;
        inblock >> y;
        emit receivePositionSignal(x,y);
    }
}

void tcpClient::connect()
{
    tcpSocket->connectToHost(address, 6666);
    connected  = true;
}

void tcpClient::sendPosition(short x, short y)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << x;
    stream << y;

    udpSocket->writeDatagram(buffer.data(), buffer.size(), address, 6666);

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
