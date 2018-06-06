#include "serverplayer.h"

ServerPlayer::ServerPlayer(QObject *parent) : Player(parent)
{
    clientUdp = new QUdpSocket();
    server = new QTcpServer();

    clientUdp->bind(6666);

    serverWindow = new ServerControlWindow(&gameParameters);

    memset(playerPositionGrid, 0, sizeof(playerPositionGrid));

    QObject::connect(server, SIGNAL(newConnection()), this, SLOT(handleConnection()));
    QObject::connect(clientUdp, SIGNAL(readyRead()), this, SLOT(readyReadUdp()));
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(iterateStartGameCounter()));

    QObject::connect(&battleRoyaleTimer, SIGNAL(timeout()), this, SLOT(manageBattleRoyaleMode()));

    if (!server->listen(QHostAddress::Any, 6666))
        qDebug() << "could not listen on server mate..\n";
    else
        qDebug() << "Listening on port 6666, localhost mate";

    serverWindow->show();
}

void ServerPlayer::readyReadUdp()
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

    gameState.gameInProgress = false;
    serverWindow->updateUI(playerList, gameState);
}

void ServerPlayer::sendPosition(unsigned char ID, short x, short y)
{
    block.clear();
    QDataStream stream(&block, QIODevice::WriteOnly); //THIS IS VERY VERY VERY TEMPORARY AT THE MOMENT BOYS. VERY TEMPORARY INDEED... ITS SHIT ATM...

    if(gameParameters.useTcp)
        stream << (unsigned char)MessageType::POSITION_UPDATE;

    stream << ID;
    stream << x;
    stream << y;
    for(QTcpSocket *client : clients)
    {
        if(gameParameters.useTcp)
            client->write(block);
        else
            clientUdp->writeDatagram(block.data(), block.size(), client->peerAddress(), BASE_UDP_PORT+playerList[idList[client]].playerID);
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
            x = (0.15 + (tempX/(float)4)*0.7) * gameParameters.width;
            y = (0.15 + (tempY/(float)5)*0.7) * gameParameters.height;
            return;
        }
    }
}
void ServerPlayer::sendWinSignal(unsigned char ID)
{
    battleRoyaleTimer.stop();
    gameState.gameInProgress = false;
    serverWindow->updateUI(playerList, gameState);
    QString winnerName = QString::fromStdString( playerList[ID].name);

    for(QTcpSocket* socket : clients)
    {
        block.clear();
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (unsigned char)MessageType::PLAYER_WON;
        out << ID;
        out << winnerName;
        socket->write(block);
    }
}

void ServerPlayer::checkWinConditions()
{
    if(gameState.gameInProgress)
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

        if(gameState.numPlayers == 1)
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
    if(gameState.numPlayers < MAX_NUM_PLAYERS)
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

            gameState.numPlayers++;
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
            gameState.numPlayers--;
            break;
        }
    }

    serverWindow->updateUI(playerList, gameState);
    clientSocket->deleteLater();
}


void ServerPlayer::sendGameParameters()
{
    qDebug() << "sizeof(gameParameters): " << sizeof(GameParameters);
    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (unsigned char)MessageType::GAME_PARAMETERS;
    out << gameParameters;

    for(auto &socket : clients)
        socket->write(block);
}

//this starts the counter, which runs for a few seconds. each tick of the clock calls iterateGameCounter
void ServerPlayer::startGameCounterSlot()
{
    sendGameParameters();
    resetGameState();

    qDebug() << "startgame button pressed... sending start game messages...";
    if(gameState.numPlayers > 0)
    {
        //CALCULATE STARTING POSITIONS AND WRITE THEM TO THE APPROPRIATE CLIENTS
        memset(playerPositionGrid, 0, sizeof(playerPositionGrid)); //set the positions to blank.

        for(auto it=playerList.begin(); it!=playerList.end(); it++)
            it->alive = true;

        gameState.gameInProgress = true;

        serverWindow->updateUI(playerList, gameState);
        timer.start(1000);
    }
}

//might need to add some more stuff here fookin ell
void ServerPlayer::resetGameState()
{
    memset(tailArray, 0, sizeof(tailArray));
    startGameCounter = TIMER_LENGTH;
    gameState.wallEncroachment = 0;
    battleRoyaleTimer.stop();
    timer.stop();
}

void ServerPlayer::iterateStartGameCounter()
{

    if(startGameCounter == 0)   //start the game!...
    {
        block.clear();
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (unsigned char)MessageType::GAME_BEGIN;

        for(auto &socket : clients)
            socket->write(block);

        serverWindow->updateUI(playerList, gameState);

        timer.stop();
        if(gameParameters.PUBGmodeEnabled)
            battleRoyaleTimer.start( gameParameters.PUBGCircleTime );
    }else                       //send the counter...
    {
        block.clear();
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (unsigned char)MessageType::TIMER_UPDATE;
        out << startGameCounter;

        for(auto &socket : clients)
            socket->write(block);  //1+2+1+

        if(startGameCounter == TIMER_LENGTH) //putting this here is a shitty solution but it works for the moment. I think I'll add a signal to inform the client that it should clear it's tail array instead of relying on this very stateful solution
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

            while(dataSize)
            {
                unsigned char mTypeChar;
                inblock >> mTypeChar;
                MessageType mType;
                mType = static_cast<MessageType>(mTypeChar);
                dataSize -= sizeof(unsigned char);

                //qDebug() << (unsigned int)mType;
                short x,y;
                if(mType == MessageType::BOMB_ACTIVATION)
                {
                    qDebug() << "bomb activation";
                    inblock >> x;
                    inblock >> y;
                    triggerBomb(x,y);
                    sendBombMessage(x,y);
                    dataSize -= sizeof(short);dataSize -= sizeof(short);
                }else if(mType == MessageType::NOTIFY_SERVER_OF_PLAYER_NAME)
                {
                    //qDebug() << "notify of name...";
                    QString data;
                    inblock >> data;
                    std::string tempStr = data.toStdString();
                    strncpy( playerList[ idList[clientSocket] ].name, tempStr.data(), MAX_NAME_LENGTH );
                    serverWindow->updateUI(playerList, gameState);
                    dataSize=0;
                }else if(mType == MessageType::POSITION_UPDATE)
                {
                    unsigned char ID;
                    inblock >> ID;
                    inblock >> x;
                    inblock >> y;
                    receivePosition(ID, x, y);
                    dataSize -= sizeof(unsigned char);
                    dataSize -= sizeof(short);dataSize -= sizeof(short);
                }
            }
        }
}

void ServerPlayer::manageBattleRoyaleMode()
{
    gameState.wallEncroachment += gameParameters.PUBGWallIncrease; //make this a parameter later on mate....
    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (unsigned char)MessageType::BATTLE_ROYALE_WALL_UPDATE;
    out << gameState.wallEncroachment;

    for(auto &socket : clients)
        socket->write(block);

    updateBattleRoyaleMode();
}

//you could make this a bit more OO mate.... yeah...
void ServerPlayer::sendBombMessage(short x, short y)
{
    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (unsigned char)MessageType::BOMB_ACTIVATION;
    out << x;
    out << y;

    for(auto &socket : clients)
        socket->write(block);
}

void ServerPlayer::receivePosition(unsigned char ID, short x, short y)
{
    if(playerList[ID].alive)
    {
        if(tailArray[x][y].id || tailArray[x][y].partOfWall || x < 0 || x >= 100 || y < 0 || y >= 100) //currently we are storing thing shere.. but... info is in tcpserver innit.. move things around later on.
        {
            sendDeathSignal(ID);
            playerList[ID].alive = false;
        }
        else
        {
            tailArray[x][y].id = ID;
            sendPosition(ID, x, y);
        }
        checkWinConditions();

        emit drawSignal();
    }
}

ServerPlayer::~ServerPlayer()
{
    serverWindow->deleteLater();
    clientUdp->deleteLater();
    server->deleteLater();
}
