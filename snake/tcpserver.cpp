#include "tcpserver.h"
#include <QDataStream>
#include <QMessageBox>
#include <QTcpSocket>
#include <iostream>
#include <QAbstractSocket>

tcpServer::tcpServer(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);

    QObject::connect(server, SIGNAL(newConnection()), this, SLOT(handleConnection()) );
    //connect(tcpServer, &QTcpServer::newConnection, this, &tcpconnect::handleConnection);

    if (!server->listen(QHostAddress::Any, 6666))
    {
        qDebug() << "could not listen on server mate..\n";
        //QMessageBox::critical(this, tr("Fortune Server"), tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
    }
    else
        qDebug() << "Listening on port 1234, localhost mate";

}

void tcpServer::handleConnection()
{
    qDebug() << "Here we go mate we are handling a fooking connection son\n";

    client = server->nextPendingConnection();
    client->setSocketOption(QAbstractSocket::LowDelayOption,1);
    in.setDevice(client);
    in.setVersion(QDataStream::Qt_4_0);
    QObject::connect(client, SIGNAL(disconnected()), client, SLOT(deleteLater()));
    QObject::connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));

}

void tcpServer::sendDataToClient()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << count;
    count++;

    client->write(block);
}

void tcpServer::readyRead()
{
    qDebug() << "in readyread son";
    while(client->bytesAvailable())
    {
        int dataSize = client->bytesAvailable();
        int x,y;
        //client->read((char*)&dataSize, sizeof(int));
        QByteArray buffer;
        buffer = client->read(dataSize);

        /*while(buffer.size() < dataSize) // only part of the message has been received
        {
            client->waitForReadyRead(); // alternatively, store the buffer and wait for the next readyRead()
            buffer.append(client->read(dataSize - buffer.size())); // append the remaining bytes of the message
        }*/

        QDataStream inblock(&buffer, QIODevice::ReadOnly);

        inblock >> x;
        inblock >> y;

        qDebug() << "x/y: " << x << "/" << y;
        emit drawPosition(x,y);
    }



    /*in.startTransaction();

    int x,y;
    in >> x;
    in >> y;
    qDebug() << "x/y: " << x << "/" << y;

    //emit drawPosition(x,y);

    if(!in.commitTransaction())
        return;*/
}
