#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include "mainwindow.h"
#include <QtNetwork/QUdpSocket>
#include <QTcpSocket>
#include <QMutex>
#include <QWaitCondition>

class MainWindow;

class ServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit ServerWorker(QObject *parent = nullptr);
    MainWindow *w;

    bool isGameOver = false;

    QMutex mutex;
    QWaitCondition waitCondition;

    int xPos = 0;
    int yPos = 0;
    bool checkCollisions();

    void drawSquare(int x, int y, QRgb color);
    void draw(bool endGame);

    bool tailArray[100][100]; //this is constant size at the moment but it will end up being of variable size mate..

signals:

public slots:
    void process();
};

#endif // SERVER WORKER_H
