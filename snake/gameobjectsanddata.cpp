#include "gameobjectsanddata.h"

const std::vector<QRgb> playerColors
  = {qRgb(0,0,0) /* (0,0,0) is the background color.*/
    ,qRgb(230,25,75),   qRgb(60,180,75),    qRgb(255,225,25),   qRgb(0,130,200),    qRgb(245,130,48)
    ,qRgb(145,30,180),  qRgb(70,240,240),   qRgb(240,50,230),   qRgb(210,245,60),   qRgb(250,190,190)
    ,qRgb(0,128,128),   qRgb(230,190,255),  qRgb(170,110,40),   qRgb(255,250,200),  qRgb(128,0,0)
    ,qRgb(170,255,195), qRgb(128,128,0),    qRgb(255,215,180),  qRgb(0,0,128),      qRgb(128,128,128)	};

PlayerInfo::PlayerInfo(){} //if I don't have a defauly constructor QHash complains at me.. so that's why this is here.

PlayerInfo::PlayerInfo(unsigned char playerID_, QRgb color_, char* name_)
    :playerID(playerID_), color(color_)
{
    memset(name, 0, sizeof(char)*MAX_NAME_LENGTH+1);
    strncpy(name, name_, MAX_NAME_LENGTH);
}
