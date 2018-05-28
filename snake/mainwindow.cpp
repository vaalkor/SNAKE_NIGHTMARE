#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpserver.h"
#include "tcpclient.h"
#include <QHostAddress>

MainWindow::MainWindow(bool isServer_, bool testingMode_, QHostAddress serverAddress_, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isServer(isServer_),
    testingMode(testingMode_),
    serverAddress(serverAddress_)
{
    ui->setupUi(this);
    image = QImage(500, 500, QImage::Format_RGB32);

    thread = new QThread();

    if(isServer) //is server
    {
        server = new tcpServer();
        serverWorker = new ServerWorker();
        serverWorker->w = this;

        QObject::connect(server, SIGNAL(receivePositionSignal(short,short)), this, SLOT(serverReceivePositionSlot(short,short)) );

    }else //is client
    {
        clientWorker = new ClientWorker();

        client = new tcpClient(serverAddress);
        QObject::connect(client->tcpSocket, SIGNAL(connected()), this, SLOT(clientConnectionSuccess()));
        QObject::connect(client->tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientConnectionFailure(QAbstractSocket::SocketError)));
        client->connect();

    }
}

void MainWindow::clientConnectionSuccess()
{
    show();

    clientWorker->moveToThread(thread);

    QObject::connect(clientWorker, SIGNAL(sendPosition(short,short)), client, SLOT(sendPosition(short,short)));

    QObject::connect(client, SIGNAL(receivePositionSignal(short,short)), this, SLOT(clientReceivePositionSlot(short,short)) );

    QObject::connect(this, &MainWindow::focusChanged, clientWorker, &ClientWorker::focusChanged); //for some reason this connection is not actually working.. oh well... what the fuck.. i dont get it son.. i dont get it...

    QObject::connect(thread, SIGNAL(started()), clientWorker, SLOT(process()) );

    QObject::connect(clientWorker, SIGNAL(drawSignal()), this, SLOT(drawSlot()));
    QObject::connect(clientWorker, SIGNAL(sendKillAcknowledgement()), this, SLOT(receivedKillAcknowledgement()));

    thread->start();

    qDebug() << "client connection success";
}
void MainWindow::clientConnectionFailure(QAbstractSocket::SocketError err)
{
    qDebug() << "client connection failure: " << err;

    if(err == QAbstractSocket::NetworkError)
        deleteLater();
}

void MainWindow::serverReceivePositionSlot(short x, short y)
{
    serverWorker->xPos = x;
    serverWorker->yPos = y;
    serverWorker->tailArray[x][y] = true;
    draw(false);
}

void MainWindow::clientReceivePositionSlot(short x, short y)
{
    clientWorker->tailArray[x][y] = true;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter canvasPainter(this);
    canvasPainter.drawImage(this->rect(), image, image.rect());

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "goodbye!";
    if(!isServer)
        clientWorker->kill = true;
    else
        deleteLater();
}

void MainWindow::receivedKillAcknowledgement()
{
    qDebug() << "received kill acknowledge mate...";
    thread->quit();
    thread->wait();
    deleteLater();
}

bool MainWindow::event(QEvent *e)
{
    QWidget::event(e); //call the base event function. this is needed because it calls the paint event function

    if(!isServer)
    {
        if(e->type() == QEvent::WindowActivate)
        {
            qDebug() << "mainwindow focused";
            //emit focusChanged(true);
            clientWorker->inFocus = true;
        }else if(e->type() == QEvent::WindowDeactivate)
        {
            qDebug() << "mainwindow unfocused"; //THIS IS NOT WHAT WE WANT.. WE WANT SIGNALS AND SLOTS BUT THIS IS A TEMPORARY MEASURE MATE!
            clientWorker->inFocus = false;
            //emit focusChanged(false);
        }
    }
    return true;
}

MainWindow::~MainWindow()
{
    qDebug() << "isServer: " << isServer << "... DESTRUCTOR CALLED...";
    if(isServer)
    {
        delete serverWorker;
        delete server;
    }
    else
    {
        delete clientWorker;
        delete client;
    }
    delete ui;
    delete thread;
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space)
    {
        qDebug() << "ye pressed C ye bloody basterdo...";
        if(!isServer)
            client->sendTcpMessage();
        else
            server->sendTcpMessage();
    }

}


void MainWindow::drawSlot()
{
    draw(false);
}

void MainWindow::drawSquare(short x, short y, QRgb color)
{
    for(int i=x*5; i<x*5+5; i++)
        for(int j=y*5; j<y*5+5; j++)
            image.setPixel(i,j, color);

    update();

}

//THIS IS A COMPLETE MESS!!! NEEDS TO BE CLEANED UP....
void MainWindow::draw(bool endGame)
{
    if(endGame)
        image.fill( qRgb(255,0,0));
    else
    {
        image.fill( qRgb(0,0,0));
        for(unsigned int i=0; i<100; i++)
            for(unsigned int j=0; j<100; j++)
                if(isServer && serverWorker->tailArray[i][j])
                    drawSquare(i,j,qRgb(255,255,255));
                else if(!isServer && clientWorker->tailArray[i][j])
                    drawSquare(i,j,qRgb(255,255,255));

        if(!isServer)
            drawSquare(clientWorker->xPos, clientWorker->yPos, qRgb(255,255,255));
    }

    update();
}
