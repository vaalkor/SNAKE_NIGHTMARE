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
    QObject::connect(clientUdp, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

    if (!server->listen(QHostAddress::Any, 6666))
    {
        qDebug() << "could not listen on server mate..\n";
        //QMessageBox::critical(this, tr("Fortune Server"), tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
    }
    else
        qDebug() << "Listening on port 1234, localhost mate";

}

void tcpServer::processPendingDatagrams()
{

    while (clientUdp->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(clientUdp->pendingDatagramSize());
        clientUdp->readDatagram(datagram.data(), datagram.size());
        int x,y;
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

void tcpServer::handleConnection()
{
    qDebug() << "Here we go mate we are handling a fooking connection son\n";

    while(server->hasPendingConnections())
    {
        QTcpSocket *tempClient = server->nextPendingConnection();
        clients.push_back( tempClient );

        tempClient->setSocketOption(QAbstractSocket::LowDelayOption,1);

        //in.setDevice(clientTcp); you will have to do this every time you get a fucking tcp message will you not mate????? or will you have to have a seperate fucking thing for each one?... good question son good question...
        //in.setVersion(QDataStream::Qt_4_0);

        QObject::connect(tempClient, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
        QObject::connect(tempClient, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }
}

void tcpServer::clientDisconnected()
{
    //IM NOT SURE ABOUT THIS AT THE MOMENT MATE. IT'S HAPPENING WHEN I FUCKING CREATE THE SOCKET, NOT WHEN I CLOSE IT.... STRANGE...
    qDebug() << "client disconnected mate";
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

void tcpServer::sendDataToClient()
{ //this was just for testing.. now its just useless...
    /*QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << count;
    count++;

    clientTcp->write(block);*/
}

void tcpServer::readyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(QObject::sender()); //this will apparently give us the pointer to the socket that was disconnected mate...
    qDebug() << "clientSocket address: " << clientSocket;
    qDebug() << "in readyread son";

    while(clientSocket->bytesAvailable())
    {
        qDebug() << "bytes available son!";
        int dataSize = clientSocket->bytesAvailable();

        QByteArray buffer;
        buffer = clientSocket->read(dataSize);

        /*while(buffer.size() < dataSize) // only part of the message has been received
        {
            clientTcp->waitForReadyRead(); // alternatively, store the buffer and wait for the next readyRead()
            buffer.append(clientTcp->read(dataSize - buffer.size())); // append the remaining bytes of the message
        }*/

        QDataStream inblock(&buffer, QIODevice::ReadOnly);

        unsigned char mTypeChar;
        MessageType mType;
        inblock >> mTypeChar; //this is gypo as fuck mate.
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

        //qDebug() << "x/y: " << x << "/" << y;
        //emit drawPosition(x,y);
    }

    /*in.startTransaction();

    int x,y;
    in >> x;
    in >> y;
    qDebug() << "x/y: " << x << "/" << y;

    //emit drawPosition(x,y);

    if(!in.commitTransaction())
        return;*/
}
