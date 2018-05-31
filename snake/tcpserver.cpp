#include "tcpserver.h"
#include <QDataStream>
#include <QMessageBox>
#include <QTcpSocket>
#include <iostream>
#include <QAbstractSocket>
#include <QRgb>


const std::vector<QRgb> playerColors
  = {qRgb(0,0,0) /* (0,0,0) is the background color.*/
    ,qRgb(230,25,75),   qRgb(60,180,75),    qRgb(255,225,25),   qRgb(0,130,200),    qRgb(245,130,48)
    ,qRgb(145,30,180),  qRgb(70,240,240),   qRgb(240,50,230),   qRgb(210,245,60),   qRgb(250,190,190)
    ,qRgb(0,128,128),   qRgb(230,190,255),  qRgb(170,110,40),   qRgb(255,250,200),  qRgb(128,0,0)
    ,qRgb(170,255,195), qRgb(128,128,0),    qRgb(255,215,180),  qRgb(0,0,128),      qRgb(128,128,128)	};

PlayerInfo::PlayerInfo(){} //if I don't have a defauly constructor QHash complains at me.. so that's why this is here.

PlayerInfo::PlayerInfo(unsigned char playerID_, QRgb color_, char* name_)
    :playerID(playerID_), color(color_)
{
    memset(name, 0, sizeof(char)*21);
    strncpy(name, name_, 20);
}

tcpServer::tcpServer(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    clientUdp = new QUdpSocket(this);
    clientUdp->bind(6666);
    //clientUdp->bind()

    std::default_random_engine rd( std::chrono::high_resolution_clock::now().time_since_epoch().count() );
    gen = std::mt19937(rd());
    dist = std::uniform_real_distribution<>(0.0,1.0);

    memset(playerPositionGrid, 0, sizeof(playerPositionGrid));

    QObject::connect(server, SIGNAL(newConnection()), this, SLOT(handleConnection()) );
    QObject::connect(clientUdp, SIGNAL(readyRead()), this, SLOT(readyReadUdp()));

    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(iterateStartGameCounter()));

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
        unsigned char clientID;
        short x,y;
        QDataStream inblock(&datagram, QIODevice::ReadOnly);
        inblock >> clientID;
        inblock >> x;
        inblock >> y;
        //qDebug() << x << "/" << y;
        emit receivePositionSignal(clientID, x,y);
    }
}

void tcpServer::sendPositionToAllClients(unsigned char clientID, short x, short y)
{
    for(const QTcpSocket *client : clients)
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly); //THIS IS VERY VERY VERY TEMPORARY AT THE MOMENT BOYS. VERY TEMPORARY INDEED... ITS SHIT ATM...
        stream << clientID;
        stream << x;
        stream << y;
        clientUdp->writeDatagram(buffer.data(), buffer.size(), client->peerAddress(), 1234);
    }
}

void tcpServer::calculateStartingPosition(short &x, short &y)
{
    //THIS WILL LIVELOCK IF YOU HAVE MORE THAN THE MAX PLAYERS!!!!!!!!!!!!!!! BEWARE!!!
    //CHANGE IT TO BE A BIT MORE ELEGENT. JUST GETTING IT WORKING FOR NOW THOUGH SON.
    while(true)
    {
        short tempX, tempY;
        tempX = dist(gen)*4;
        tempY = dist(gen)*5;
        if(!playerPositionGrid[tempX][tempY])
        {
            playerPositionGrid[tempX][tempY] = 1;
            qDebug() << "starting pos server: " << tempX << "/" << tempY;
            x = (0.15 + (tempX/(float)4)*0.7) * info.width;
            y = (0.15 + (tempY/(float)5)*0.7) * info.height;
            return;
        }
    }
}

//this is only called when a NEW tcp connection is made. Clients make these once when they join.
void tcpServer::handleConnection()
{
    if(info.numPlayers < MAX_NUM_PLAYERS)
    {
        while(server->hasPendingConnections())
        {
            QTcpSocket *tempClient = server->nextPendingConnection();
            clients.push_back( tempClient );

            tempClient->setSocketOption(QAbstractSocket::LowDelayOption,1);

            QObject::connect(tempClient, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
            QObject::connect(tempClient, SIGNAL(readyRead()), this, SLOT(readyReadTcp()));

            //work out a new id for this client.
            unsigned char tempID = 1; //this starts at 1 so I can just use 0 for having NO color in an array spot. a bit dirty. whatever.
            for(auto it=playerList.begin(); it!=playerList.end(); it++)
                for(auto it2=playerList.begin(); it2!=playerList.end(); it2++)
                    if( it2->playerID == tempID)
                        tempID++;

            playerList[tempID] = PlayerInfo(tempID, playerColors[tempID], "");
            idList[tempClient] = tempID;

            //inform that person of their new ID and their starting position...
            block.clear();
            QDataStream out(&block, QIODevice::WriteOnly);

            out << (unsigned char)MessageType::PLAYER_ID_ASSIGNMENT;
            out << tempID;
            tempClient->write(block);

            info.numPlayers++;
        }
    }

}

void tcpServer::clientDisconnected()
{
    qDebug() << "Client disconnected.";
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(QObject::sender()); //this will apparently give us the pointer to the socket that was disconnected mate...

    playerList.remove( idList[clientSocket] );
    idList.remove(clientSocket);

    for(auto it=clients.begin(); it < clients.end(); it++)
    {
        if(clientSocket == *it)
        {
            clients.erase(it);
            info.numPlayers--;
            break;
        }
    }
    emit updateServerUI();
    clientSocket->deleteLater();
}

//this is just for testing at the moment.
void tcpServer::sendTcpMessage()
{
    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    unsigned char message = qrand() % (unsigned int)MessageType::COUNT;
    out << message;

    for(auto &socket : clients)
        socket->write(block);
}
void tcpServer::startGameCounterSlot()
{
    qDebug() << "startgame button pressed... sending start game messages...";
    if(info.numPlayers > 0)
    {
        //CALCULATE STARTING POSITIONS AND WRITE THEM TO THE APPROPRIATE CLIENTS
        memset(playerPositionGrid, 0, sizeof(playerPositionGrid)); //set the positions to blank.

        for(auto it=playerList.begin(); it!=playerList.end(); it++)
            it->alive = true;

        //this needs to be changed. the starting position of EACH PLAYER needs to be sent to every other player mate.. I'll do that tomorrow
        //I also need to add some checks so that ONCE the game start button has been clicked, no more people can join.... cool mate cool.
        //also need to add some stuff to make sure that all values and whatever are changed to the right things when games are ended, and stsrted.... whatever.
        for(auto &socket : clients)
        {
            short x,y;
            calculateStartingPosition(x,y);
            qDebug() << "starting position: " << x << "..." << y;
            block.clear();
            QDataStream out(&block, QIODevice::WriteOnly);
            out << (unsigned char)MessageType::START_POSITION;
            out << x << y;
            socket->write(block);
        }

        info.gameInProgress = true;

        timer.start(1000);
    }
}
void tcpServer::iterateStartGameCounter()
{
    qDebug() << "GAME COUNTER: " << startGameCounter;
    if(startGameCounter == 0)   //start the game!...
    {
        startGameCounter = 3;


        emit startGameSignal(); //this will have to be changed when we presumabely change to having a server without serverworker... and just have one thing that EXTENDS TCPSERVER!!!! FUCKING NICE...

        block.clear();
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (unsigned char)MessageType::GAME_BEGIN;

        for(auto &socket : clients)
            socket->write(block);

        emit updateServerUI();

        timer.stop();

    }else                       //send the counter...
    {

        block.clear();
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (unsigned char)MessageType::TIMER_UPDATE;
        out << startGameCounter;

        for(auto &socket : clients)
            socket->write(block);

        startGameCounter--;
    }


}
void tcpServer::stopGame()
{
    qDebug() << "stopgame button pressed... sending start game messages...";
    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (unsigned char)MessageType::GAME_STOPPED;

    for(auto &socket : clients)
        socket->write(block);

    emit updateServerUI();
}
void tcpServer::gameOver(unsigned char winnerID)
{
    qDebug() << "gameOver! player with ID: " << winnerID << " wins!";
    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (unsigned char)MessageType::PLAYER_WON;
    out << winnerID;

    for(auto &socket : clients)
        socket->write(block);

    info.gameInProgress = false;
    emit updateServerUI();
}

void tcpServer::sendDeathSignal(unsigned char clientID)
{
    playerList[clientID].alive = false;

    for(QTcpSocket* socket : clients)
        if(clientID == idList[socket])
        {
            block.clear();
            QDataStream out(&block, QIODevice::WriteOnly);
             out << (unsigned char)MessageType::PLAYER_DIED;

            socket->write(block);
            break;
        }
}

void tcpServer::checkWinConditions()
{
    if(info.gameInProgress)
    {
        int numAlive = 0;
        unsigned char aliveID = 0; //the id of the survivor
        for(auto it=playerList.begin(); it!=playerList.end(); it++)
            if(it->alive)
            {
                numAlive++;
                aliveID = it->playerID;
            }

        //important note: if there are no players alive the ID will be ZERO. this just signifies a draw as no players ever have their ID set to ZERO. The ID's start from 1.

        if(info.numPlayers == 1)
        {
            if(numAlive == 0)
                sendWinSignal( playerList.begin()->playerID );
        }else
        {
            if(numAlive == 1 || numAlive == 0)
            {
                sendWinSignal( playerList[aliveID].playerID);
            }
        }
    }

}

void tcpServer::sendWinSignal(unsigned char clientID)
{
    info.gameInProgress = false;
    for(QTcpSocket* socket : clients)
    {
        block.clear();
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (unsigned char)MessageType::PLAYER_WON;
        out << clientID;
        socket->write(block);
    }

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
            case MessageType::NOTIFY_SERVER_OF_PLAYER_NAME:
                qDebug() << "notify of name...";
                QString data;
                inblock >> data;
                std::string tempStr = data.toStdString();
                strncpy( playerList[ idList[clientSocket] ].name, tempStr.data(), 20 );
                emit updateServerUI();
                break;


        }
        qDebug() << "after switch!";

    }

}
