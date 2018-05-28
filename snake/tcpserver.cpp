#include "tcpserver.h"
#include <QDataStream>
#include <QMessageBox>
#include <QTcpSocket>
#include <iostream>
#include <QAbstractSocket>

tcpServer::tcpServer(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    clientUdp = new QUdpSocket(this);
    clientUdp->bind(6666);

    in.setVersion(QDataStream::Qt_4_0); //i dont even know if this is necessary son...

    QObject::connect(server, SIGNAL(newConnection()), this, SLOT(handleConnection()) );
    QObject::connect(clientUdp, SIGNAL(readyRead()), this, SLOT(readyReadUdp()));

    if (!server->listen(QHostAddress::Any, 6666))
    {
        qDebug() << "could not listen on server mate..\n";
    }
    else
        qDebug() << "Listening on port 6666, localhost mate";

}

void tcpServer::readyReadUdp()
{

    while (clientUdp->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(clientUdp->pendingDatagramSize());
        clientUdp->readDatagram(datagram.data(), datagram.size());
        short x,y;
        QDataStream inblock(&datagram, QIODevice::ReadOnly);
        inblock >> x;
        inblock >> y;
        //qDebug() << x << "/" << y;
        emit receivePositionSignal(x,y);

        for(const QTcpSocket *client : clients)
        {
            QByteArray buffer;
            QDataStream stream(&buffer, QIODevice::WriteOnly); //THIS IS VERY VERY VERY TEMPORARY AT THE MOMENT BOYS. VERY TEMPORARY INDEED... ITS SHIT ATM...
            stream << x;
            stream << y;
            clientUdp->writeDatagram(buffer.data(), buffer.size(), client->peerAddress(), 1234);
        }

    }
}

//this is only called when a NEW tcp connection is made. Clients make these once when they join.
void tcpServer::handleConnection()
{
    while(server->hasPendingConnections())
    {
        QTcpSocket *tempClient = server->nextPendingConnection();
        clients.push_back( tempClient );

        tempClient->setSocketOption(QAbstractSocket::LowDelayOption,1);

        QObject::connect(tempClient, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
        QObject::connect(tempClient, SIGNAL(readyRead()), this, SLOT(readyReadTcp()));
    }
}

void tcpServer::clientDisconnected()
{
    qDebug() << "Client disconnected.";
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(QObject::sender()); //this will apparently give us the pointer to the socket that was disconnected mate...


    for(auto it=clients.begin(); it < clients.end(); it++)
    {
        if(clientSocket == *it)
        {
            clients.erase(it);
            break;
        }
    }

    clientSocket->deleteLater();
}

//this is just for testing at the moment.
void tcpServer::sendTcpMessage()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    unsigned char message = qrand() % (unsigned int)MessageType::COUNT;
    out << message;

    clients[0]->write(block);
}

void tcpServer::readyReadTcp()
{
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
            case MessageType::GAME_INFO :
                qDebug() << "game info mate";
                break;
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
