#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "servercontrolwindow.h"

#include <QHostAddress>
#include <QDesktopWidget>

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
            if(isServer)
            {
                if(serverPlayer->tailArray[i][j].partOfWall)
                    drawSquare(i, j, qRgb(67,134,242));
                else if(serverPlayer->tailArray[i][j].id)
                    drawSquare(i, j, playerColors[ serverPlayer->tailArray[i][j].id ]);
            }
            else
            {
                if(clientPlayer->tailArray[i][j].partOfWall)
                    drawSquare(i, j, qRgb(67,134,242));
                else if(clientPlayer->tailArray[i][j].id)
                    drawSquare(i, j, playerColors[ clientPlayer->tailArray[i][j].id ]);
            }

    if(!isServer)
    {
        GameParameters &clientParameters = clientPlayer->gameParameters;

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

        if(clientParameters.sprintEnabled)
        {
            float sprintProportion = clientPlayer->gameState.sprintMeter/(float)clientParameters.sprintLength;
            brush->setColor(Qt::gray);
            painter->setBrush(*brush);
            painter->setPen(*barBackGroundPen);
            painter->drawRect(0.1*image.width(), 0.9*image.height(), 0.07*image.width(), 0.005*image.height());
            brush->setColor(Qt::green);
            painter->setBrush(*brush);
            painter->setPen(*sprintBarPen);
            painter->drawRect(0.1*image.width(), 0.9*image.height(), 0.07*image.width()*sprintProportion, 0.005*image.height());
        }

        if(clientParameters.bombsEnabled)
        {
            float bombProportion = clientPlayer->gameState.bombCharge/(float)clientParameters.bombChargeTime;
            brush->setColor(Qt::gray);
            painter->setBrush(*brush);
            painter->setPen(*barBackGroundPen);
            painter->drawRect(0.1*image.width(), 0.88*image.height(), 0.07*image.width(), 0.005*image.height());
            brush->setColor(Qt::red);
            painter->setBrush(*brush);
            painter->setPen(*bombBarPen);
            painter->drawRect(0.1*image.width(), 0.88*image.height(), 0.07*image.width()*bombProportion, 0.005*image.height());
        }
        if(clientPlayer->printWinnerName)
        {
            QPen pen( Qt::white );
            painter->setPen(pen);
            painter->setFont(QFont("Times", 30, QFont::Bold));
            QString text;
            if(clientPlayer->winnerID == clientPlayer->clientID)
                text = "YOU WIN!11!!!";
            else
                text = clientPlayer->winnerName + "\nWINS m8.";
            painter->drawText(image.rect(), Qt::AlignCenter, text);
        }
        if(clientPlayer->drawBomb)
        {
            int minX = clientPlayer->bombPosition.x() - clientParameters.bombRadius; int minY = clientPlayer->bombPosition.y() - clientParameters.bombRadius;
            int maxX = clientPlayer->bombPosition.x() + clientParameters.bombRadius; int maxY = clientPlayer->bombPosition.y() + clientParameters.bombRadius;

            if(minX < 0) minX = 0;
            if(minY < 0) minY = 0;
            if(maxX >= clientParameters.width)  maxX = clientParameters.width  -1;
            if(maxY >= clientParameters.height) maxY = clientParameters.height -1;

            for(int y=minY; y<=maxY; y++)
                for(int x=minX; x<= maxX; x++)
                    drawSquare(x,y, qRgb(255,0,0));
        }
    }

    update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(!isServer)
        clientPlayer->handleKeyPress(event);
}
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(!isServer)
        clientPlayer->handleKeyReleased(event);
}
