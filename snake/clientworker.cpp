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

SnakePiece::SnakePiece(int x_, int y_) : x(x_), y(y_) {}

ClientWorker::ClientWorker(QObject *parent) : QObject(parent)
{
    memset(tailArray, 0, sizeof(bool)*100*100);
}

bool ClientWorker::checkCollisions()
{
    //NOT DONE YET MATE!
    return false; //implement later!
}

void ClientWorker::process()
{

    int xDir = 1;
    int yDir = 0;

    while(1)
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
                std::cout << "esacpe!\n";
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
            xPos = xPos + xDir;
            yPos = yPos + yDir;

            if( xPos >= 100 || xPos < 0 || yPos < 0 || yPos >= 100 || checkCollisions())
            {
                isGameOver = true;
            }

            else
            {
                emit drawSignal();
                emit sendPosition(xPos, yPos);
            }

        }
        emit drawSignal();
        Sleep(33);

    }
}

void ClientWorker::randomSlot(int x, int y)
{
    qDebug() << x << "/" << y;
}

void ClientWorker::receivePositionSlot(int x, int y)
{
    tailArray[x][y] = true;
}

void ClientWorker::focusChanged(bool value)
{
    qDebug() << "focus changed";
    inFocus = value;
}
