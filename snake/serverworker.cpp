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
    memset(tailArray, 0, sizeof(unsigned char)*100*100);

    serverWindow.show();
}

bool ServerWorker::checkCollisions()
{
    /*if( tailArray[xPos][yPos] )
        return true;
    else
        return false;*/
    return false;
}

void ServerWorker::process()
{
}
