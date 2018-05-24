#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H

#include <QObject>
#include "mainwindow.h"
#include <QtNetwork/QUdpSocket>
#include <QTcpSocket>

class SnakePiece{
public:
    SnakePiece(int x, int y_);

    int x;
    int y;
};

class MainWindow;

class ClientWorker : public QObject
{
    Q_OBJECT
public:
    explicit ClientWorker(QObject *parent = nullptr);
    MainWindow *w;

    bool isGameOver = false;

    int xPos = 50;
    int yPos = 50;

    bool inFocus = true;

    bool tailArray[100][100]; //this is constant size at the moment but it will end up being of variable size mate..

    std::list<SnakePiece> list;
    bool checkCollisions();

    void drawSquare(int x, int y, QRgb color);
    void draw(bool endGame);
    //void sendMessage();

signals:
    void sendPosition(int x, int y);

public slots:
    //void readyRead();
    void process();
    void focusChanged(bool value);
    void receivePosition(int x, int y);
};

#endif // CLIENTWORKER_H
