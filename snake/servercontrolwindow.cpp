#include "servercontrolwindow.h"
#include "ui_servercontrolwindow.h"
#include <QDebug>
#include <QAbstractListModel>

ServerControlWindow::ServerControlWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerControlWindow)
{
    ui->setupUi(this);
}

void ServerControlWindow::updateNameList(QHash<unsigned char, PlayerInfo> &hash)
{
    ui->playerListWidget->clear();
    for(auto it=hash.begin(); it!=hash.end();it++)
        ui->playerListWidget->addItem( QString::fromStdString( it->name) );
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
