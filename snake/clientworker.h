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

    int appleX = 0;
    int appleY = 0;
    bool applePickedUp = false;

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
};

#endif // CLIENTWORKER_H
