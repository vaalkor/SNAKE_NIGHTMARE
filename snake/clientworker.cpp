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
        std::list<SnakePiece>::iterator it;
        for(it = list.begin(); it != list.end(); it++)
            drawSquare(it->x, it->y, qRgb(255,255,255));

        drawSquare(appleX, appleY, qRgb(255,0,0));
    }



    w->update();
}


bool ClientWorker::checkCollisions()
{
    int x = list.begin()->x;
    int y = list.begin()->y;

    std::list<SnakePiece>::iterator it = list.begin();
    std::advance(it, 1);

    for(it; it != list.end(); it++)
        if(x == it->x && y == it->y)
            return true;
    return false;
}

/*void worker::sendMessage()
{
    std::cout << "sending data mate!\n";
    QByteArray Data;
    Data.append("chicken dippers mate\n");
    socket->writeDatagram(Data, QHostAddress::LocalHost,1234);
    socket->flush();
}

void worker::readyRead()
{
    std::cout << "Reading data mate!!\n";
    QByteArray Buffer;
    Buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;
    socket->readDatagram(Buffer.data(), Buffer.size(), &sender, &senderPort);
    qDebug() << "message from: " << sender.toString();
    qDebug() << "sender port: " << senderPort;
    qDebug() << "message: " << Buffer;
}*/

void ClientWorker::process()
{

    list.emplace_front(50,50);
    list.emplace_front(51, 50);
    list.emplace_front(52, 50);
    list.emplace_front(53, 50);
    list.emplace_front(54, 50);

    int xDir = 1;
    int yDir = 0;

    unsigned int currentTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    std::default_random_engine rd( currentTime );

    std::mt19937 gen = std::mt19937(rd());
    std::uniform_real_distribution<> dist = std::uniform_real_distribution<>(0.0, 1.0);

    appleX = dist(gen)*100;
    appleY = dist(gen)*100;

    while(1)
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
            list.clear();
            list.emplace_front(50,50);
            xDir = 1;
            yDir = 0;
        }

        if(isGameOver)
        {
            draw(true);
        }
        else
        {
            int newX = list.begin()->x + xDir;
            int newY = list.begin()->y + yDir;

            if(!applePickedUp)
                list.pop_back();
            else
                applePickedUp = false;

            list.emplace_front(newX, newY);

            if( newX == appleX && newY == appleY)
            {
                applePickedUp = true;
                appleX = dist(gen)*100;
                appleY = dist(gen)*100;
            }


            if( newX >= 100 || newX < 0 || newY < 0 || newY >= 100 || checkCollisions())
            {
                isGameOver = true;
            }

            else
            {
                draw(false);
                emit sendPosition(newX, newY);
            }

        }

        Sleep(33);

    }
}
