#include "serverclientselection.h"
#include "ui_serverclientselection.h"
#include "mainwindow.h"

#include <QDebug>

ServerClientSelection::ServerClientSelection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerClientSelection)
{
    ui->setupUi(this);
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
