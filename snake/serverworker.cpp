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
    //socket = new QUdpSocket(this);
    //socket->bind(QHostAddress::LocalHost, 1234);
    //connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
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
        drawSquare(xPos, yPos, qRgb(255,255,255));
    }

    w->update();
}


bool ServerWorker::checkCollisions()
{
    return false;
}

void ServerWorker::drawPosition(int x, int y)
{
    xPos = x;
    yPos = y;
    draw(false);
}

void ServerWorker::process()
{
}
