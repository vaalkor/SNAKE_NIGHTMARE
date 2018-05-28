#include "servercontrolwindow.h"
#include "ui_servercontrolwindow.h"
#include <QDebug>

ServerControlWindow::ServerControlWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerControlWindow)
{
    ui->setupUi(this);
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

}

void ServerControlWindow::on_stopGameButton_clicked()
{

}

void ServerControlWindow::on_quitButton_clicked()
{
    emit rejectSignal();
}
