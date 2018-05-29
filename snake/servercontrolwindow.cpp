#include "servercontrolwindow.h"
#include "ui_servercontrolwindow.h"
#include <QDebug>
#include <QAbstractListModel>
#include "tcpserver.h"

ServerControlWindow::ServerControlWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerControlWindow)
{
    ui->setupUi(this);
    ui->statusLabel->setText( QString("NO GAME IN PROGRESS") );
    ui->numPlayersInLobbyLabel->setText(QString("0 players in lobby") );
}

void ServerControlWindow::updateUI(QHash<unsigned char, PlayerInfo> &hash, GameInfo &info)
{
    ui->playerListWidget->clear();
    for(auto it=hash.begin(); it!=hash.end();it++)
        ui->playerListWidget->addItem( QString::fromStdString( it->name) );

    if(info.gameInProgress)
        ui->statusLabel->setText( QString("GAME IN PROGRESS") );
    else
        ui->statusLabel->setText( QString("NO GAME IN PROGRESS") );

    std::string playersInLobbyText = std::to_string(info.numPlayers) + " players in lobby!";
    ui->numPlayersInLobbyLabel->setText(QString::fromStdString(playersInLobbyText));
}

ServerControlWindow::~ServerControlWindow()
{
    delete ui;
}

void ServerControlWindow::reject()
{
    qDebug() << "fuck off mate!!!";
}

void ServerControlWindow::on_startGameButton_clicked()
{
    emit startGameSignal();
}

void ServerControlWindow::on_stopGameButton_clicked()
{
    emit stopCurrentGameSignal();
}

void ServerControlWindow::on_quitButton_clicked()
{
    emit rejectSignal();
}
