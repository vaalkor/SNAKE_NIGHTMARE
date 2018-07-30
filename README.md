# SNAKE_NIGHTMARE
This is my messy mutliplayer snake game written in C++ making extensive use of QT

## This is essentially a LAN multiplayer verison of the great BMTron web game.
BMTron is a browser based game in which up to 4 players control snakes, all with
persistent tails that get increasingly longer, and attempt to not bump into each 
other or the sides of the map. The last one alive wins. 

I and many of my friends have played this game extensively. It was a staple of the 
sixth form common room many years ago and has remained popular ever since. 
This is a QT C++ based multiplayer version with a few additions. 

### SNAKE_NIGHTMARE features
- Supports up to 20 players
- Features a sprint mode and a rechargable bomb
- Sprint length, regen time and bomb recharge time and radius can be altered by the server in the lobby between games for varied gameplay
- A PUBG mode (I should probably rename that at some point) in which the border of the map encroaches at server specified time intervals. Each time the border enchroaches the tails all reset.
- A cup mode. A scoreboard can be displayed by holding down shift on the keyboard.

### Why make a 2d game in QT?
I pretty much fell in love with QT during my final year of uni and when I finished I 
decided to make this game in it for some reason. There are other libraries that seem
to be more suited to C++ game development but all I needed was a simple way of drawing
pixels and some nice TCP and UDP socket abstractions to work with. QT, in my experience,
has been a joy to work with. I wanted to make this game for ages and at the time I was
looking for any reason to use QT so it made sense at the time

**Disclaimer:** This game was written purely for my amusement and, seeing as I haven't actually
played it with humans yet, and thus haven't extensively tested it, will probably break at some 
point. As such it comes with absolutely no guarantees of working at all. 
