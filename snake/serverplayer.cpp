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

void ServerPlayer::sendTcpBlock(ClientStatus clientStatus, QByteArray &block)
{
    if(clientStatus == ClientStatus::ALL_CLIENTS)
    {
        for(QTcpSocket *socket : clients)
            socket->write(block);
    }else if(clientStatus == ClientStatus::IN_CURRENT_GAME_CLIENTS)
    {
         for(QTcpSocket *socket : clients)
             if(playerList[idList[socket]].inCurrentGame)
                 socket->write(block);
    }

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

    sendTcpBlock(ClientStatus::IN_CURRENT_GAME_CLIENTS, block);

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

    if(gameParameters.useTcp)
        sendTcpBlock(ClientStatus::IN_CURRENT_GAME_CLIENTS, block);
    else
        for(QTcpSocket *client : clients)
            clientUdp->writeDatagram(block.data(), block.size(), client->peerAddress(), BASE_UDP_PORT+playerList[idList[client]].playerID);

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
            x = (0.3 + (tempX/(float)4)*0.7) * gameParameters.width;
            y = (0.15 + (tempY/(float)5)*0.7) * gameParameters.height;
            return;
        }
    }
}

void ServerPlayer::manageCupMode(unsigned char ID)
{
    playerList[ID].score ++;
    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (unsigned char)MessageType::SCORE_UPDATE;
    out << ID;
    out << playerList[ID].score;

    sendTcpBlock(ClientStatus::ALL_CLIENTS, block);

    //if a player has reached the maximum score reset all of the scores and resend them to the clients mate...
    for(auto it=playerList.begin(); it!=playerList.end();it++)
        if(it->score == gameParameters.winLimit)
        {
            for(auto it2=playerList.begin(); it2!=playerList.end();it2++)
            {
                it2->score = 0;
                block.clear();
                QDataStream out(&block, QIODevice::WriteOnly);
                out << (unsigned char)MessageType::SCORE_UPDATE;
                out << it2->playerID;
                out << it2->score;
                sendTcpBlock(ClientStatus::ALL_CLIENTS, block);
            }
            break;
        }
}

void ServerPlayer::sendWinSignal(unsigned char ID)
{
    if(gameParameters.cupMode)
        manageCupMode(ID);

    battleRoyaleTimer.stop();
    gameState.gameInProgress = false;
    serverWindow->updateUI(playerList, gameState);
    QString winnerName = QString::fromStdString( playerList[ID].name);

    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (unsigned char)MessageType::PLAYER_WON;
    out << ID;
    out << winnerName;

    sendTcpBlock(ClientStatus::IN_CURRENT_GAME_CLIENTS, block);
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
            tempClient->setSocketOption(QAbstractSocket::LowDelayOption,1);

            //iterate through the players already in the game and send their ID's to the newly connected player.
            for(auto it=playerList.begin(); it!=playerList.end(); it++)
            {
                block.clear();
                QDataStream out(&block, QIODevice::WriteOnly);
                out << (unsigned char)MessageType::PLAYER_CONNECTED;
                out << it->playerID;
                tempClient->write(block);
            }

            clients.push_back( tempClient );

            QObject::connect(tempClient, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
            QObject::connect(tempClient, SIGNAL(readyRead()), this, SLOT(readyReadTcp()));

            unsigned char tempID = calculateNewPlayerID();

            playerList[tempID] = PlayerInfo(tempID, "");
            idList[tempClient] = tempID;

            //inform that person of their new ID and their starting position...
            block.clear();
            QDataStream out(&block, QIODevice::WriteOnly);

            out << (unsigned char)MessageType::PLAYER_ID_ASSIGNMENT;
            out << tempID;
            tempClient->write(block);

            block.clear();
            QDataStream out2(&block, QIODevice::WriteOnly);
            out2 << (unsigned char)MessageType::PLAYER_CONNECTED;
            out2 << tempID;

            sendTcpBlock(ClientStatus::ALL_CLIENTS, block);

            gameState.numPlayers++;
        }
    }
}
unsigned char ServerPlayer::calculateNewPlayerID()
{
    unsigned char tempID = 1; //this starts at 1 so I can just use 0 for having NO color in an array spot. a bit dirty. whatever.
    for(auto it=playerList.begin(); it!=playerList.end(); it++)
        for(auto it2=playerList.begin(); it2!=playerList.end(); it2++)
            if( it2->playerID == tempID)
                tempID++;
    return tempID;
}
void ServerPlayer::clientDisconnected()
{
    qDebug() << "Client disconnected.";
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(QObject::sender()); //this will apparently give us the pointer to the socket that was disconnected mate...
    unsigned char tempID = idList[clientSocket];
    playerList.remove( tempID );
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

    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (unsigned char)MessageType::PLAYER_DISCONNECTED;
    out << tempID;

    sendTcpBlock(ClientStatus::ALL_CLIENTS, block);

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

    sendTcpBlock(ClientStatus::ALL_CLIENTS, block);
}

//might need to add some more stuff here fookin ell
void ServerPlayer::resetGameState()
{
    for(auto it=playerList.begin(); it!=playerList.end(); it++)
    {
        it->alive = true;
        it->inCurrentGame = true;
        it->ready = false;
    }

    memset(playerPositionGrid, 0, sizeof(playerPositionGrid)); //set the positions to blank.
    memset(tailArray, 0, sizeof(tailArray));
    startGameCounter = TIMER_LENGTH;
    gameState.wallEncroachment = 0;
    gameState.numReady = 0;
    battleRoyaleTimer.stop();
    timer.stop();
}

//this starts the counter, which runs for a few seconds. each tick of the clock calls iterateGameCounter
void ServerPlayer::startGameCounterSlot()
{
    qDebug() << "startgame button pressed... sending start game messages...";
    if(gameState.numPlayers > 0)
    {
        resetGameState();
        sendGameParameters();

        gameState.gameInProgress = true;

        serverWindow->updateUI(playerList, gameState);
        timer.start(1000);
    }
}

void ServerPlayer::sendStartingPositions()
{
    for(QTcpSocket *socket : clients)
    {
        short x,y;
        calculateStartingPosition(x,y);
        block.clear();
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (unsigned char)MessageType::START_POSITION;
        out << idList[socket]; //send the id of that player...
        out << x << y;

        //now write out that starting position to all clients in the game...
        sendTcpBlock(ClientStatus::IN_CURRENT_GAME_CLIENTS, block);
    }
}

void ServerPlayer::iterateStartGameCounter()
{

    if(startGameCounter == 0)   //start the game!...
    {
        block.clear();
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (unsigned char)MessageType::GAME_BEGIN;

        sendTcpBlock(ClientStatus::IN_CURRENT_GAME_CLIENTS, block);

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

        sendTcpBlock(ClientStatus::IN_CURRENT_GAME_CLIENTS, block);

        if(startGameCounter == TIMER_LENGTH) //putting this here is a shitty solution but it works for the moment. I think I'll add a signal to inform the client that it should clear it's tail array instead of relying on this very stateful solution
            sendStartingPositions();

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
                }else if(mType == MessageType::NOTIFY_NAME)
                {
                    QString name;
                    inblock >> name;
                    std::string tempStr = name.toStdString();
                    strncpy( playerList[ idList[clientSocket] ].name, tempStr.data(), MAX_NAME_LENGTH );
                    serverWindow->updateUI(playerList, gameState);
                    dataSize -= (name.length()*sizeof(QChar)+4); //this is EXTREMELY GYPO AS FUCK!!!

                    //now send the name to all connected clients...
                    for(auto it=playerList.begin();it!=playerList.end();it++)
                    {
                        tempStr = it->name;
                        name = QString::fromStdString(tempStr);
                        block.clear();
                        QDataStream out(&block, QIODevice::WriteOnly);
                        out << (unsigned char)MessageType::NOTIFY_NAME;
                        out << it->playerID;
                        out << name;
                        sendTcpBlock(ClientStatus::ALL_CLIENTS, block);
                    }
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
                else if(mType == MessageType::PLAYER_READY)
                {
                    qDebug() << "player ready mate!!!!!! ID = " << playerList[ idList[clientSocket]].playerID;
                    playerList[ idList[clientSocket]].ready = true;
                    gameState.numReady++;
                    checkPlayersReady();
                }
            }
        }
}

void ServerPlayer::checkPlayersReady()
{
    qDebug() << "PLAYERS READY: " << gameState.numReady << "numplayers: " << gameState.numPlayers;
    if(gameParameters.autoStartWhenPlayersReady)
    {
        if(gameState.numReady == gameState.numPlayers)
            startGameCounterSlot();
    }
}

void ServerPlayer::manageBattleRoyaleMode()
{
    gameState.wallEncroachment += gameParameters.PUBGWallIncrease; //make this a parameter later on mate....
    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (unsigned char)MessageType::BATTLE_ROYALE_WALL_UPDATE;
    out << gameState.wallEncroachment;

    sendTcpBlock(ClientStatus::IN_CURRENT_GAME_CLIENTS, block);

    updateBattleRoyaleModeState();
}

//you could make this a bit more OO mate.... yeah...
void ServerPlayer::sendBombMessage(short x, short y)
{
    block.clear();
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (unsigned char)MessageType::BOMB_ACTIVATION;
    out << x;
    out << y;

    sendTcpBlock(ClientStatus::IN_CURRENT_GAME_CLIENTS, block);
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
