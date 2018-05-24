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
        emit drawPosition(x,y);

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

    // INSERT LOGIC HERE FOR REMOVING FROM LIST AND OTHER THINGS...

    clientSocket->deleteLater();
}

void tcpServer::sendDataToClient()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << count;
    count++;

    clientTcp->write(block);
}

void tcpServer::readyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(QObject::sender()); //this will apparently give us the pointer to the socket that was disconnected mate...
    qDebug() << "clientSocket address: " << clientSocket;
    qDebug() << "in readyread son";

    while(clientTcp->bytesAvailable())
    {
        qDebug() << "bytes available son!";
        int dataSize = clientTcp->bytesAvailable();

        QByteArray buffer;
        buffer = clientTcp->read(dataSize);

        while(buffer.size() < dataSize) // only part of the message has been received
        {
            clientTcp->waitForReadyRead(); // alternatively, store the buffer and wait for the next readyRead()
            buffer.append(clientTcp->read(dataSize - buffer.size())); // append the remaining bytes of the message
        }

        QDataStream inblock(&buffer, QIODevice::ReadOnly);
        int result;
        inblock >> result;
        qDebug() << "result: " << result;

        //unsigned char mTypeChar;
        //MessageType mType;
        //inblock >> mTypeChar; //this is gypo as fuck mate.
        //mType = static_cast<MessageType>(mTypeChar);

        /*switch(mType){
            case MessageType::PLAYER_CONNECTED :
                std::cout << "player connected";
            case MessageType::PLAYER_DISCONNECTED :
                std::cout << "player disconnected";
            case MessageType::POSITION_UPDATE :
                std::cout << "player update";
            case MessageType::BOMB_ACTIVATION :
                std::cout << "bomb activation";
            case MessageType::PLAYER_DIED :
                std::cout << "player died";
            case MessageType::PLAYER_WON :
                std::cout << "player won";
            case MessageType::GAME_BEGIN :
                std::cout << "game begun";
            case MessageType::TIMER_UPDATE :
                std::cout << "timer update";
        }*/

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
