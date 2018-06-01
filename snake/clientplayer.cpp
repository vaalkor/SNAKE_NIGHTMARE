#include "clientplayer.h"
#include <QDataStream>
#include <QByteArray>
#include <QDebug>

ClientPlayer::ClientPlayer(QHostAddress address_, QObject *parent) : address(address_), Player(parent)
{
    udpSocket.bind(1234); //clients listen on 1234, the server listens on 6666 mate.... cool.

    QObject::connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT( readyReadTcp()) );
    QObject::connect(&udpSocket, SIGNAL(readyRead()), this, SLOT( readyReadUdp()) );

    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(iterateGameState()));
}

void ClientPlayer::iterateGameState()
{
    isSprinting = false;

    if(inFocus)
    {
        if(GetKeyState(VK_LEFT) & 0x8000)
        {
            if(xDir != 1)
            {
                xDir = -1;
                yDir = 0;
            }
        }
        if(GetKeyState(VK_RIGHT) & 0x8000)
        {
            if(xDir != -1)
            {
                xDir = 1;
                yDir = 0;
            }
        }
        if(GetKeyState(VK_UP) & 0x8000)
        {
            if(yDir != 1)
            {
                xDir = 0;
                yDir = -1;
            }
        }
        if(GetKeyState(VK_DOWN) & 0x8000)
        {
            if(yDir != -1)
            {
                xDir = 0;
                yDir = 1;
            }
        }
        if(GetKeyState(VK_ESCAPE) & 0x8000 || GetKeyState(VK_RETURN) & 0x8000)
        {
            xPos = 50;
            yPos = 50;
            xDir = 1;
            yDir = 0;
        }
        if(GetKeyState(VK_SHIFT) & 0x8000 && gameParameters.sprintEnabled)
            isSprinting = true;
        if(GetKeyState(VK_SPACE) & 0x8000 && gameParameters.bombsEnabled)
            if(gameState.bombCharge >= gameParameters.bombChargeTime)
                sendBombMessage();
    }

    xPos += xDir;
    yPos += yDir;
    sendPosition(clientID, xPos, yPos); //this has been chagned to a function call m8....

    manageBomb();
    manageSprint();

    emit drawSignal();
}

void ClientPlayer::manageBomb()
{
    if(gameParameters.bombsEnabled)
    {
        gameState.bombCharge += gameParameters.tickLength;
        if(gameState.bombCharge > gameParameters.bombChargeTime)
            gameState.bombCharge = gameParameters.bombChargeTime;
    }
}

void ClientPlayer::manageSprint()
{
    if(gameParameters.sprintEnabled)
    {
        if(isSprinting && gameState.sprintMeter > gameParameters.tickLength) //if you are sprinting... DO IT AGAIN! //I'll need to change this to something more sensible I believe...
        {
            xPos += xDir;
            yPos += yDir;
            sendPosition(clientID, xPos, yPos); //this has been chagned to a function call m8....

            gameState.sprintMeter -= gameParameters.tickLength;
        }

        gameState.sprintMeter += gameParameters.sprintLength/(float)gameParameters.sprintRechargeLength * gameParameters.tickLength;

        if(gameState.sprintMeter > gameParameters.sprintLength)
            gameState.sprintMeter = gameParameters.sprintLength;
    }

}

void ClientPlayer::readyReadUdp()
{
    while (udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket.pendingDatagramSize());
        udpSocket.readDatagram(datagram.data(), datagram.size());
        unsigned char clientID;
        short x,y;
        QDataStream inblock(&datagram, QIODevice::ReadOnly);
        inblock >> clientID;
        inblock >> x;
        inblock >> y;
        receivePosition(clientID,x,y);
    }
}

void ClientPlayer::connect()
{
    tcpSocket.connectToHost(address, 6666);
}

void ClientPlayer::readyReadTcp()
{
    qDebug() << "CLIENT READING TCP MESSAGE!";
        //this gets the address of the socket we are going to read from.

        while(tcpSocket.bytesAvailable())
        {
            int dataSize = tcpSocket.bytesAvailable();
            qDebug() << "Bytes available: " << tcpSocket.bytesAvailable();
            QByteArray buffer;
            buffer = tcpSocket.read(dataSize);

            while(buffer.size() < dataSize) // only part of the message has been received
            {
                tcpSocket.waitForReadyRead(); // alternatively, store the buffer and wait for the next readyRead()
                buffer.append(tcpSocket.read(dataSize - buffer.size())); // append the remaining bytes of the message
            }

            QDataStream inblock(&buffer, QIODevice::ReadOnly);

            while(dataSize)
            {
                qDebug() << "buffer count before enum read: " << buffer.count();

                unsigned char mTypeChar;
                inblock >> mTypeChar;
                MessageType mType;
                mType = static_cast<MessageType>(mTypeChar);
                dataSize -= sizeof(unsigned char);

                qDebug() << "buffer count after enum read: " << buffer.count();

                qDebug() << "message type: " << (unsigned int)mType;

                unsigned char tempID;
                short x,y;

                qDebug() << "before SWITCH STATEMENT!...bytes available: " << tcpSocket.bytesAvailable();

                switch(mType){
                    case MessageType::BOMB_ACTIVATION :
                        qDebug() << "bomb activation";
                        inblock >> x;
                        inblock >> y;
                        triggerBomb(x,y);
                        dataSize -= sizeof(short); dataSize -= sizeof(short);
                        break;
                    case MessageType::PLAYER_DIED :
                        //qDebug() << "PLAYER DIED!!!";
                        timer.stop();
                        break;
                    case MessageType::PLAYER_WON :
                        //qDebug() << "player won";
                        inblock >> tempID;
                        dataSize -= sizeof(unsigned char);
                        gameOver(tempID);
                        //qDebug() << "PLAYER " << tempID << "WON!!!!";
                        break;
                    case MessageType::GAME_BEGIN :
                        //qDebug() << "game begun";
                        startGameTimerOnScreen = false;
                        startGame();
                        emit drawSignal();
                        break;
                    case MessageType::TIMER_UPDATE :
                        short gameCounter;
                        inblock >> gameCounter;
                        startGameTimer = gameCounter;

                        //if the counter has just started then we clear the previous game state array and display the timer number
                        if(gameCounter == TIMER_LENGTH)
                        {
                            gameState.sprintMeter = gameParameters.sprintLength; //reset the sprint meter mate...
                            gameState.bombCharge = 0;
                            memset(tailArray,0, sizeof(tailArray));
                            startGameTimerOnScreen = true;
                        }

                        qDebug() << "client timer update: " << gameCounter;
                        dataSize -= sizeof(short);
                        emit drawSignal();
                        break;
                    case MessageType::PLAYER_ID_ASSIGNMENT:
                        qDebug() << "buffer count before assignment: " << buffer.count();
                        inblock >> tempID;
                        dataSize -= sizeof(unsigned char);
                        clientID = tempID;
                        break;
                    case MessageType::START_POSITION:
                        inblock >> tempID;
                        inblock >> x;
                        inblock >> y;
                        dataSize -= sizeof(unsigned char);
                        dataSize -= sizeof(short); dataSize -= sizeof(short);
                        receiveStartPosition(tempID, x, y);

                }
                qDebug() << "after switch!...bytes available: " << tcpSocket.bytesAvailable();
            }

        }
}

void ClientPlayer::startGame()
{
    gameState.gameInProgress = true;
    timer.start(33); //start the fooking timer mate!!!!! sick one sick. //remember you will also have to clear the array at the start of your countdown.
}

void ClientPlayer::gameOver(unsigned char ID)
{
    qDebug() << "player " << ID << " wins!!!!";
    gameState.gameInProgress = false;
    timer.stop();
}

//MIGHT NEED TO CHANGE THIS TO SEND ALSO HOW MANY BYTES THE NAME IS BEFORE HAND?!?!?!
void ClientPlayer::sendName(std::string name)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << (unsigned char)MessageType::NOTIFY_SERVER_OF_PLAYER_NAME;
    out << QString::fromStdString(name);

    tcpSocket.write(block);
}

void ClientPlayer::sendBombMessage()
{
    gameState.bombCharge = 0;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << (unsigned char)MessageType::BOMB_ACTIVATION;
    out << xPos;
    out << yPos;

    tcpSocket.write(block);
}

void ClientPlayer::sendPosition(unsigned char ID, short x, short y)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << ID;
    stream << x;
    stream << y;

    udpSocket.writeDatagram(buffer.data(), buffer.size(), address, 6666);
}
void ClientPlayer::receivePosition(unsigned char ID, short x, short y)
{
    tailArray[x][y] = ID;
}
void ClientPlayer::receiveStartPosition(unsigned char ID, short x, short y)
{
    if(ID == clientID)
    {
        xPos = x;
        yPos = y;
    }else
    {
        tailArray[x][y] = ID;
    }

    emit drawSignal();
}
