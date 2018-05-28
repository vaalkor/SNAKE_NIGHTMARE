#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpserver.h"
#include "tcpclient.h"

MainWindow::MainWindow(bool isServer_, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isServer(isServer_)
{
    ui->setupUi(this);
    image = QImage(500, 500, QImage::Format_RGB32);

    thread = new QThread();

    if(isServer) //is server
    {
        server = new tcpServer();
        serverWorker = new ServerWorker();
        serverWorker->w = this;

        QObject::connect(server, SIGNAL(receivePositionSignal(int,int)), this, SLOT(serverReceivePositionSlot(int,int)) );

    }else //is client
    {
        client = new tcpClient();

        clientWorker = new ClientWorker();
        clientWorker->w = this;
        clientWorker->moveToThread(thread);

        QObject::connect(clientWorker, SIGNAL(sendPosition(int,int)), client, SLOT(sendPosition(int,int)));
        //QObject::connect(client, SIGNAL(receivePositionSignal(int,int)), clientWorker, SLOT(receivePositionSlot(int,int)));
        //QObject::connect(client, SIGNAL(receivePositionSignal(int,int)), clientWorker, SLOT(randomSlot(int,int)) );
        //QObject::connect(client, &tcpClient::receivePositionSignal, clientWorker, &ClientWorker::receivePositionSlot);

        QObject::connect(client, SIGNAL(receivePositionSignal(int,int)), this, SLOT(clientReceivePositionSlot(int,int)) );

        QObject::connect(this, &MainWindow::focusChanged, clientWorker, &ClientWorker::focusChanged); //for some reason this connection is not actually working.. oh well... what the fuck.. i dont get it son.. i dont get it...

        QObject::connect(thread, SIGNAL(started()), clientWorker, SLOT(process()) );
        QObject::connect(thread, SIGNAL(finished()), this, SLOT(threadFinished()));
        QObject::connect(clientWorker, SIGNAL(drawSignal()), this, SLOT(drawSlot()));
        QObject::connect(clientWorker, SIGNAL(sendKillAcknowledgement()), this, SLOT(receivedKillAcknowledgement()));

    }

    //setAttribute(Qt::WA_DeleteOnClose); //THIS WORKS BUT I HAVEN'T DEALT WITH DELETING PROPERLY YET. SHIT DEADLOCKS SON...

    thread->start();
}

void MainWindow::threadFinished()
{
    qDebug() << "thread finished mate...";
}

void MainWindow::serverReceivePositionSlot(int x, int y)
{
    serverWorker->xPos = x;
    serverWorker->yPos = y;
    serverWorker->tailArray[x][y] = true;
    draw(false);
}

void MainWindow::clientReceivePositionSlot(int x, int y)
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
    {
        //clientWorker->mutex.lock();
        clientWorker->kill = true;
        //clientWorker->mutex.unlock();
    }
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
        client->sendTcpMessage();
    }

}


void MainWindow::drawSlot()
{
    draw(false);
}

void MainWindow::drawSquare(int x, int y, QRgb color)
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
