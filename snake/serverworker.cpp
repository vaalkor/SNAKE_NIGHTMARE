#include "serverworker.h"
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

ServerWorker::ServerWorker(QObject *parent) : QObject(parent)
{
    memset(tailArray, 0, sizeof(bool)*100*100);
}


void ServerWorker::drawSquare(int x, int y, QRgb color)
{
    for(int i=x*5; i<x*5+5; i++)
        for(int j=y*5; j<y*5+5; j++)
            w->image.setPixel(i,j, color);

    w->update();

}

void ServerWorker::draw(bool endGame)
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


bool ServerWorker::checkCollisions()
{
    if( tailArray[xPos][yPos] )
        return true;
    else
        return false;
}

void ServerWorker::process()
{
}
