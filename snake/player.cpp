#include "player.h"

Player::Player(QObject *parent) : QObject(parent)
{
    std::default_random_engine rd( std::chrono::high_resolution_clock::now().time_since_epoch().count() );
    gen = std::mt19937(rd());
    dist = std::uniform_real_distribution<>(0.0,1.0);

    memset(tailArray, 0 , sizeof(unsigned char)*100*100);
}

void Player::resizeTailArray()
{
//this will resize the tail array to the values specified by the game info class... cba to do it for now though...
}
