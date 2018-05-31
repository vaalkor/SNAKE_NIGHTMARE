#include "serverplayer.h"

ServerPlayer::ServerPlayer(QObject *parent) : Player(parent)
{
    clientUdp.bind(6666);

    memset(playerPositionGrid, 0, sizeof(playerPositionGrid));

    QObject::connect(&server, SIGNAL(newConnection()), this, SLOT(handleConnection()));
    QObject::connect(&clientUdp, SIGNAL(readyRead()), this, SLOT(readyReadUdp()));
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(iterateStartGameCounter()));

    if (!server.listen(QHostAddress::Any, 6666))
        qDebug() << "could not listen on server mate..\n";
    else
        qDebug() << "Listening on port 6666, localhost mate";

    serverWindow.show();
}

void ServerPlayer::readyReadUdp()
{
    while (clientUdp.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(clientUdp.pendingDatagramSize());
        clientUdp.readDatagram(datagram.data(), datagram.size());
        unsigned char clientID;
        short x,y;
        QDataStream inblock(&datagram, QIODevice::ReadOnly);
        inblock >> clientID;
        inblock >> x;
        inblock >> y;
        receivePosition(clientID, x, y);
    }
}

void ServerPlayer::gameOver(unsigned char winnerID)
{
    qDebug() << "gameOver! player with ID: " << winnerID << " wins!";
    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (unsigned char)MessageType::PLAYER_WON;
    out << winnerID;

    for(auto &socket : clients)
        socket->write(block);

    gameInfo.gameInProgress = false;
    serverWindow.updateUI(playerList, gameInfo);
}

void ServerPlayer::sendPosition(unsigned char ID, short x, short y)
{
    for(const QTcpSocket *client : clients)
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly); //THIS IS VERY VERY VERY TEMPORARY AT THE MOMENT BOYS. VERY TEMPORARY INDEED... ITS SHIT ATM...
        stream << ID;
        stream << x;
        stream << y;
        clientUdp.writeDatagram(buffer.data(), buffer.size(), client->peerAddress(), 1234);
    }
}

void ServerPlayer::sendDeathSignal(unsigned char ID)
{
    playerList[ID].alive = false;

    for(QTcpSocket* socket : clients)
        if(ID == idList[socket])
        {
            block.clear();
            QDataStream out(&block, QIODevice::WriteOnly);
             out << (unsigned char)MessageType::PLAYER_DIED;

            socket->write(block);
            break;
        }
}
void ServerPlayer::calculateStartingPosition(short &x, short &y)
{
    while(true)
    {
        short tempX, tempY;
        tempX = dist(gen)*4;
        tempY = dist(gen)*5;
        if(!playerPositionGrid[tempX][tempY])
        {
            playerPositionGrid[tempX][tempY] = 1;
            qDebug() << "starting pos server: " << tempX << "/" << tempY;
            x = (0.15 + (tempX/(float)4)*0.7) * gameInfo.width;
            y = (0.15 + (tempY/(float)5)*0.7) * gameInfo.height;
            return;
        }
    }
}
void ServerPlayer::sendWinSignal(unsigned char ID)
{
    gameInfo.gameInProgress = false;
    for(QTcpSocket* socket : clients)
    {
        block.clear();
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (unsigned char)MessageType::PLAYER_WON;
        out << ID;
        socket->write(block);
    }
}

void ServerPlayer::checkWinConditions()
{
    if(gameInfo.gameInProgress)
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

        if(gameInfo.numPlayers == 1)
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

void ServerPlayer::handleConnection()
{
    if(gameInfo.numPlayers < MAX_NUM_PLAYERS)
    {
        while(server.hasPendingConnections())
        {
            QTcpSocket *tempClient = server.nextPendingConnection();
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

            gameInfo.numPlayers++;
        }
    }
}
void ServerPlayer::clientDisconnected()
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
            gameInfo.numPlayers--;
            break;
        }
    }

    serverWindow.updateUI(playerList, gameInfo);
    clientSocket->deleteLater();
}

void ServerPlayer::startGameCounterSlot()
{
    memset(tailArray, 0, sizeof(tailArray));

    startGameCounter = 3;

    qDebug() << "startgame button pressed... sending start game messages...";
    if(gameInfo.numPlayers > 0)
    {
        //CALCULATE STARTING POSITIONS AND WRITE THEM TO THE APPROPRIATE CLIENTS
        memset(playerPositionGrid, 0, sizeof(playerPositionGrid)); //set the positions to blank.

        for(auto it=playerList.begin(); it!=playerList.end(); it++)
            it->alive = true;

        gameInfo.gameInProgress = true;

        timer.start(1000);
    }
}
void ServerPlayer::iterateStartGameCounter()
{
    qDebug() << "GAME COUNTER: " << startGameCounter;
    if(startGameCounter == 0)   //start the game!...
    {
        startGameCounter = 3;

        block.clear();
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (unsigned char)MessageType::GAME_BEGIN;

        for(auto &socket : clients)
            socket->write(block);

        serverWindow.updateUI(playerList, gameInfo);

        timer.stop();

    }else                       //send the counter...
    {
        block.clear();
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (unsigned char)MessageType::TIMER_UPDATE;
        out << startGameCounter;

        for(auto &socket : clients)
            socket->write(block);

        if(startGameCounter == 3) //putting this here is a shitty solution but it works for the moment. I think I'll add a signal to inform the client that it should clear it's tail array instead of relying on this very stateful solution
        {
            //this needs to be changed. the starting position of EACH PLAYER needs to be sent to every other player mate.. I'll do that tomorrow
            //I also need to add some checks so that ONCE the game start button has been clicked, no more people can join.... cool mate cool.
            //also need to add some stuff to make sure that all values and whatever are changed to the right things when games are ended, and stsrted.... whatever.
            for(QTcpSocket *socket : clients)
            {
                short x,y;
                calculateStartingPosition(x,y);
                qDebug() << "starting position: " << x << "..." << y;
                block.clear();
                QDataStream out(&block, QIODevice::WriteOnly);
                out << (unsigned char)MessageType::START_POSITION;
                out << idList[socket]; //send the id of that player...
                out << x << y;
                socket->write(block);

                //now write out that starting position to all clients in the game...
                for(auto &clientSocket : clients)
                    clientSocket->write(block);
            }
        }

        startGameCounter--;
    }
}

//recall that on the client you have an extra loop for dealing with multiple message in a row. that might be required here at some point son.. think abaaaat it...
void ServerPlayer::readyReadTcp()
{
    //this gets the address of the socket we are going to read from.
        QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(QObject::sender());

        //qDebug() << "Bytes available: " << clientSocket->bytesAvailable();
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

            //qDebug() << (unsigned int)mType;

            switch(mType){
                case MessageType::BOMB_ACTIVATION :
                    //qDebug() << "bomb activation";
                    //NEED TO HANDLE THIS LATER ON M8....
                    break;
                case MessageType::NOTIFY_SERVER_OF_PLAYER_NAME:
                    //qDebug() << "notify of name...";
                    QString data;
                    inblock >> data;
                    std::string tempStr = data.toStdString();
                    strncpy( playerList[ idList[clientSocket] ].name, tempStr.data(), 20 );
                    serverWindow.updateUI(playerList, gameInfo);
                    break;
            }
        }
}

void ServerPlayer::receivePosition(unsigned char ID, short x, short y)
{
    if(playerList[ID].alive)
    {
        if(tailArray[x][y] || x < 0 || x >= 100 || y < 0 || y >= 100) //currently we are storing thing shere.. but... info is in tcpserver innit.. move things around later on.
        {
            sendDeathSignal(ID);
            playerList[ID].alive = false;
        }
        else
        {
            tailArray[x][y] = ID;
            sendPosition(ID, x, y);
        }
        checkWinConditions();

        emit drawSignal();
    }
}

ServerPlayer::~ServerPlayer()
{

}
