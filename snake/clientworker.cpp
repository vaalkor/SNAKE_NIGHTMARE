#include "clientworker.h"
#include <iostream>
#include "stdlib.h"
#include "stdio.h"
#include "Windows.h"
#include <chrono>
#include <thread>
#include <list>
#include <random>
#include <QGuiApplication>
#include "math.h"

ClientWorker::ClientWorker(bool testingMode_, unsigned int seed_, QObject *parent) : testingMode(testingMode_), seed(seed_), QObject(parent)
{
    /*tailArray = static_cast<bool**>(malloc(sizeof(bool*)*100));
    for(unsigned int i=0; i<100; i++)
    {
        tailArray[i] = static_cast<bool*>( malloc(sizeof(bool)*100) );
        memset(tailArray[i], 0, sizeof(bool)*100);
    }*/

    std::default_random_engine rd( seed );
    gen = std::mt19937(rd());
    dist = std::uniform_real_distribution<>(0.0, 1.0);

    memset(tailArray, 0, sizeof(unsigned char)*100*100);
    qDebug() << "3: TESTINGMODE: " << testingMode;
    if(testingMode)
    {
        currentDirection = dist(gen)*4;
        xDir = xDirDirections[ currentDirection ];
        yDir = yDirDirections[ currentDirection ];
    }
}

void ClientWorker::process()
{

    while(!kill)
    {
        if(gameInProgress)
        {
            if(inFocus && !testingMode)
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
                    isGameOver = false;
                    xPos = 50;
                    yPos = 50;
                    xDir = 1;
                    yDir = 0;
                }
            }

            if(isGameOver)
            {
                emit drawSignal();
            }
            else
            {
                xPos += xDir;
                yPos += yDir;

                emit drawSignal();
                emit sendPosition(xPos, yPos);

            }
            timeSinceLastDirectionChange += 33;
            if(testingMode && timeSinceLastDirectionChange >= maintainDirectionLength)
            {
                timeSinceLastDirectionChange = 0;
                currentDirection += (dist(gen)*4 - 1);
                currentDirection = abs( currentDirection%4 );
                xDir = xDirDirections[ currentDirection ];
                yDir = yDirDirections[ currentDirection ];
                maintainDirectionLength = 100+dist(gen)*400;
            }

            emit drawSignal();
            Sleep(33);
        }
        else //if we go into here there is no longer a game in progress, and this thread will wait until the server tells the client to wake it up again.
        {
            mutex.lock();
            waitCondition.wait(&mutex);
            mutex.unlock();
        }

    }

    //emit a signal that notifies the main thread that this thread has finished processing and is ready to be destroyed...
    emit sendKillAcknowledgement();
}

//this is for receiving an update to the tail array from the server
void ClientWorker::receivePositionSlot(short x, short y)
{
    tailArray[x][y] = true;

}

void ClientWorker::focusChanged(bool value)
{
    inFocus = value;
}

ClientWorker::~ClientWorker()
{

}
