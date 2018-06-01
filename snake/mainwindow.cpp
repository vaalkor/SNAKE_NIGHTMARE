#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QDesktopWidget>
#include "servercontrolwindow.h"

MainWindow::MainWindow(bool isServer_, std::string name_, bool testingMode_, QHostAddress serverAddress_, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isServer(isServer_),
    name(name_),
    testingMode(testingMode_),
    serverAddress(serverAddress_)
{
    ui->setupUi(this);
    image = QImage(500, 500, QImage::Format_RGB32);
    painter = new QPainter(&image);
    sprintBarPen = new QPen(Qt::green);
    bombBarPen = new QPen(Qt::red);
    barBackGroundPen = new QPen(Qt::gray);
    brush = new QBrush(Qt::SolidPattern);

    QRect screen = QApplication::desktop()->screenGeometry(0);

    if(isServer) //is server
    {
        serverPlayer = new ServerPlayer();

        QObject::connect(serverPlayer->serverWindow, &ServerControlWindow::rejectSignal, this, &QMainWindow::close);
        QObject::connect(serverPlayer->serverWindow, SIGNAL(startGameSignal()), serverPlayer, SLOT(startGameCounterSlot()));
        QObject::connect(serverPlayer, SIGNAL(drawSignal()), this, SLOT(drawSlot()));

        move(QPoint( screen.width()*0.75-width()/2.0, screen.height()*0.75-height()/2.0 ));

    }else //is client
    {
        clientPlayer = new ClientPlayer(serverAddress);

        QObject::connect(&clientPlayer->tcpSocket, SIGNAL(connected()), this, SLOT(clientConnectionSuccess()));
        QObject::connect(&clientPlayer->tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientConnectionFailure(QAbstractSocket::SocketError)));
        clientPlayer->connect();

        move(QPoint(screen.width()*0.5-width()/2.0, screen.height()*0.75-height()/2.0));
    }
}

void MainWindow::clientConnectionSuccess()
{
    show();

    QObject::connect(clientPlayer, SIGNAL(drawSignal()), this, SLOT(drawSlot()) );

    clientPlayer->sendName(name);

    qDebug() << "client connection success";
}
void MainWindow::clientConnectionFailure(QAbstractSocket::SocketError err)
{
    qDebug() << "client connection failure: " << err;
    //add a few more cases to this m8?
    if(err == QAbstractSocket::NetworkError)
        deleteLater();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter canvasPainter(this);
    canvasPainter.drawImage(this->rect(), image, image.rect());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "goodbye!";
    /*if(!isServer)
    {
        clientWorker->kill = true;
        clientWorker->waitCondition.wakeAll();
    }
    else*/
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
            clientPlayer->inFocus = true;
        }else if(e->type() == QEvent::WindowDeactivate)
        {
            qDebug() << "mainwindow unfocused"; //THIS IS NOT WHAT WE WANT.. WE WANT SIGNALS AND SLOTS BUT THIS IS A TEMPORARY MEASURE MATE!
            clientPlayer->inFocus = false;
        }
    }
    return true;
}

MainWindow::~MainWindow()
{
    qDebug() << "isServer: " << isServer << "... DESTRUCTOR CALLED...";
    if(isServer)
        serverPlayer->deleteLater();
    else
        clientPlayer->deleteLater();

    delete sprintBarPen;
    delete bombBarPen;
    delete barBackGroundPen;
    delete painter;
    delete ui;
}

void MainWindow::drawSquare(short x, short y, QRgb color)
{
    for(int i=x*5; i<x*5+5; i++)
        for(int j=y*5; j<y*5+5; j++)
            image.setPixel(i,j, color);

    update();

}

//THIS IS A COMPLETE MESS!!! NEEDS TO BE CLEANED UP....
void MainWindow::drawSlot()
{
    image.fill( qRgb(0,0,0));
    for(unsigned int i=0; i<100; i++)
        for(unsigned int j=0; j<100; j++)
            if(isServer && serverPlayer->tailArray[i][j])
                drawSquare(i, j, playerColors[ serverPlayer->tailArray[i][j] ]);
            else if(!isServer && clientPlayer->tailArray[i][j])
                drawSquare(i, j, playerColors[ clientPlayer->tailArray[i][j] ]);

    if(!isServer)
    {
        drawSquare(clientPlayer->xPos, clientPlayer->yPos, qRgb(255,255,255));
        if(clientPlayer->startGameTimerOnScreen)
        {
            painter->setBrush(Qt::NoBrush);
            QPen pen( playerColors[ clientPlayer->clientID ] );
            pen.setWidthF(3);
            painter->setPen(pen);
            painter->setFont(QFont("Times", 40, QFont::Bold));
            std::string tempNum = std::to_string(clientPlayer->startGameTimer);
            painter->drawText(image.rect(), Qt::AlignCenter, QString::fromStdString(tempNum));

            //when you change the width and height to be VARIABLE. you will have to do some more involved normalization...
            painter->drawEllipse(QPoint( clientPlayer->xPos*5, clientPlayer->yPos*5), 15, 15);
        }

        if(clientPlayer->gameParameters.sprintEnabled)
        {
            float sprintProportion = clientPlayer->gameState.sprintMeter/(float)clientPlayer->gameParameters.sprintLength;
            brush->setColor(Qt::gray);
            painter->setBrush(*brush);
            painter->setPen(*barBackGroundPen);
            painter->drawRect(0.1*image.width(), 0.9*image.height(), 0.07*image.width(), 0.005*image.height());
            brush->setColor(Qt::green);
            painter->setBrush(*brush);
            painter->setPen(*sprintBarPen);
            painter->drawRect(0.1*image.width(), 0.9*image.height(), 0.07*image.width()*sprintProportion, 0.005*image.height());
        }
    }

    update();
}
