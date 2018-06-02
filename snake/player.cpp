#include "player.h"

Player::Player(QObject *parent) : QObject(parent)
{
    std::default_random_engine rd( std::chrono::high_resolution_clock::now().time_since_epoch().count() );
    gen = std::mt19937(rd());
    dist = std::uniform_real_distribution<>(0.0,1.0);

    memset(tailArray, 0 , sizeof(unsigned char)*100*100);
}

void Player::triggerBomb(short x, short y)
{
    int minX = x-gameParameters.bombRadius; int minY = y-gameParameters.bombRadius;
    int maxX = x+gameParameters.bombRadius; int maxY = y+gameParameters.bombRadius;

    if(minX < 0) minX = 0;
    if(minY < 0) minY = 0;
    if(maxX >= gameParameters.width)  maxX = gameParameters.width  -1;
    if(maxY >= gameParameters.height) maxY = gameParameters.height -1;

    for(unsigned int y=minY; y<=maxY; y++)
        for(unsigned int x=minX; x<= maxX; x++)
            tailArray[x][y] = 0;
}

void Player::resizeTailArray()
{
    //this will resize the tail array to the values specified by the game info class... cba to do it for now though...
}
