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
    QObject::connect(udpSocket, SIGNAL(readyRead()), this, SLOT( readyReadUdp()) );

}

void tcpClient::readyReadUdp()
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

//this is just for testing at the moment.
void tcpClient::sendTcpMessage()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    unsigned char message = qrand() % (unsigned int)MessageType::COUNT;
    out << message;

    tcpSocket->write(block);
}

void tcpClient::readyReadTcp()
{
    qDebug() << "CLIENT READING TCP MESSAGE!";
    //this gets the address of the socket we are going to read from.
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(QObject::sender());

    qDebug() << "Bytes available: " << clientSocket->bytesAvailable();
    while(clientSocket->bytesAvailable())
    {
        int dataSize = clientSocket->bytesAvailable();

        QByteArray buffer;
        buffer = clientSocket->read(dataSize);

        while(buffer.size() < dataSize) // only part of the message has been received
        {
            clientSocket->waitForReadyRead(); // alternatively, store the buffer and wait for the next readyRead()
            buffer.append(clientSocket->read(dataSize - buffer.size())); // append the remaining bytes of the message
        }

        QDataStream inblock(&buffer, QIODevice::ReadOnly);

        unsigned char mTypeChar;
        inblock >> mTypeChar;
        MessageType mType;
        mType = static_cast<MessageType>(mTypeChar);

        qDebug() << (unsigned int)mType;

        switch(mType){
            case MessageType::PLAYER_CONNECTED :
                qDebug() << "player connected";
                break;
            case MessageType::PLAYER_DISCONNECTED :
                qDebug() << "player disconnected";
                break;
            case MessageType::POSITION_UPDATE :
                qDebug() << "player update";
                break;
            case MessageType::BOMB_ACTIVATION :
                qDebug() << "bomb activation";
                break;
            case MessageType::PLAYER_DIED :
                qDebug() << "player died";
                break;
            case MessageType::PLAYER_WON :
                qDebug() << "player won";
                break;
            case MessageType::GAME_BEGIN :
                qDebug() << "game begun";
                break;
            case MessageType::TIMER_UPDATE :
                qDebug() << "timer update";
                break;
        }
        qDebug() << "after switch!";

    }
}
