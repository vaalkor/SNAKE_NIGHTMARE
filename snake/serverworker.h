#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include "mainwindow.h"
#include <QtNetwork/QUdpSocket>
#include <QTcpSocket>

class MainWindow;

class ServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit ServerWorker(QObject *parent = nullptr);
    MainWindow *w;

    bool isGameOver = false;

    int xPos = 0;
    int yPos = 0;
    bool checkCollisions();

    void drawSquare(int x, int y, QRgb color);
    void draw(bool endGame);

signals:

public slots:
    void drawPosition(int x, int y);
    void process();
};

#endif // SERVER WORKER_H
