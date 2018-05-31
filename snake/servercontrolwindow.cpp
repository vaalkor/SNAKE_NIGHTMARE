#include "servercontrolwindow.h"
#include "ui_servercontrolwindow.h"
#include <QDebug>
#include <QAbstractListModel>
#include <QApplication>
#include <QCoreApplication>
#include <QDesktopWidget>

ServerControlWindow::ServerControlWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerControlWindow)
{
    ui->setupUi(this);
    ui->statusLabel->setText( QString("NO GAME IN PROGRESS") );
    ui->numPlayersInLobbyLabel->setText(QString("0 players in lobby") );

    QRect screen = static_cast<QApplication *>( QCoreApplication::instance() )->desktop()->screenGeometry(0);
    move(QPoint( screen.width()*0.75-width()/2.0, screen.height()*0.25-height()/2.0 ));
}

void ServerControlWindow::updateUI(QHash<unsigned char, PlayerInfo> &hash, GameState &state)
{
    ui->playerListWidget->clear();
    for(auto it=hash.begin(); it!=hash.end();it++)
        ui->playerListWidget->addItem( QString::fromStdString( it->name) );

    if(state.gameInProgress)
        ui->statusLabel->setText( QString("GAME IN PROGRESS") );
    else
        ui->statusLabel->setText( QString("NO GAME IN PROGRESS") );

    std::string playersInLobbyText = std::to_string(state.numPlayers) + " players in lobby!";
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
void ServerControlWindow::on_quitButton_clicked()
{
    emit rejectSignal();
}
