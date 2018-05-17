#include "serverclientselection.h"
#include "ui_serverclientselection.h"
#include "mainwindow.h"
#include <QNetworkInterface>
#include <QHostAddress>
#include <QList>

#include <QDebug>

ServerClientSelection::ServerClientSelection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerClientSelection)
{
    ui->setupUi(this);

    QList<QHostAddress> ipAddressList = QNetworkInterface::allAddresses();
    for(unsigned int i=0; i<ipAddressList.size(); i++)
        ui->comboBox->addItem( ipAddressList.at(i).toString() );
}

ServerClientSelection::~ServerClientSelection()
{
    delete ui;
}

void ServerClientSelection::on_clientButtonClicked_clicked()
{
    qDebug() << "client button clicked";
    MainWindow *w = new MainWindow(false);
    w->show();
}

void ServerClientSelection::on_serverButtonClicked_clicked()
{
    qDebug() << "server button clicked";
    MainWindow *w = new MainWindow(true);
    w->show();
}
