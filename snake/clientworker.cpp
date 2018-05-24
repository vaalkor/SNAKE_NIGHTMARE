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


void ClientWorker::drawSquare(int x, int y, QRgb color)
{
    for(int i=x*5; i<x*5+5; i++)
        for(int j=y*5; j<y*5+5; j++)
            w->image.setPixel(i,j, color);

    w->update();

}

void ClientWorker::draw(bool endGame)
{
    if(endGame)
        w->image.fill( qRgb(255,0,0));
    else
    {
        w->image.fill( qRgb(0,0,0));
        for(unsigned int i=0; i<100; i++)
            for(unsigned int j=0; j<100; j++)
                if(tailArray[i][j])
                    drawSquare(i,j,qRgb(255,255,255));

        drawSquare(xPos, yPos, qRgb(255,255,255));
    }

    w->update();
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

    unsigned int currentTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    std::default_random_engine rd( currentTime );

    std::mt19937 gen = std::mt19937(rd());
    std::uniform_real_distribution<> dist = std::uniform_real_distribution<>(0.0, 1.0);

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
            draw(true);
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
                draw(false);
                emit sendPosition(xPos, yPos);
            }

        }

        Sleep(33);

    }
}

void ClientWorker::receivePosition(int x, int y)
{
    tailArray[x][y] = true;
}

void ClientWorker::focusChanged(bool value)
{
    qDebug() << "focus changed";
    inFocus = value;
}
