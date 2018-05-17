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

        QObject::connect(server, SIGNAL(drawPosition(int,int)), serverWorker, SLOT(drawPosition(int,int)) );

    }else //is client
    {
        client = new tcpClient();
        client->connect();

        clientWorker = new ClientWorker();
        clientWorker->w = this;
        clientWorker->moveToThread(thread);

        QObject::connect(clientWorker, SIGNAL(sendPosition(int,int)), client, SLOT(sendPosition(int,int)));
        QObject::connect(thread, SIGNAL(started()), clientWorker, SLOT(process()) );
    }

    thread->start();
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter canvasPainter(this);
    canvasPainter.drawImage(this->rect(), image, image.rect());

}

MainWindow::~MainWindow()
{
    //thread->quit();
    thread->wait();
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
    //delete worker2;
    delete ui;
    delete thread;
}
