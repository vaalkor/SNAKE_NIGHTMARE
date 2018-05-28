#include "servercontrolwindow.h"
#include "ui_servercontrolwindow.h"

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

void ServerControlWindow::on_startGameButton_clicked()
{

}

void ServerControlWindow::on_stopGameButton_clicked()
{

}

void ServerControlWindow::on_quitButton_clicked()
{

}
