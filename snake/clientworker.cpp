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

ClientWorker::ClientWorker(QObject *parent) : QObject(parent)
{
    memset(tailArray, 0, sizeof(bool)*100*100);
}

void ClientWorker::process()
{

    int xDir = 1;
    int yDir = 0;

    while(!kill)
    {
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
                    yDir = -1;
                    xDir = 0;
                }
            }
            if(GetKeyState(VK_DOWN) & 0x8000)
            {
                if(yDir != -1)
                {
                    yDir = 1;
                    xDir = 0;
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
            short xPosNew = xPos + xDir;
            short yPosNew = yPos + yDir;

            if( xPosNew >= 100 || xPosNew < 0 || yPosNew < 0 || yPosNew >= 100)
            {
                isGameOver = true;
            }
            else
            {
                xPos = xPosNew;
                yPos = yPosNew;
                emit drawSignal();
                emit sendPosition(xPos, yPos);
            }

        }
        emit drawSignal();
        Sleep(33);

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
