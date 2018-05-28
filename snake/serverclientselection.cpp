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
    bool testingMode = ui->clientButtonClicked->isChecked();
    QHostAddress address = QHostAddress(ui->IPSelectionBox->text());
    new MainWindow(false, testingMode, address);

}

void ServerClientSelection::on_serverButtonClicked_clicked()
{
    qDebug() << "server button clicked";
    MainWindow *w = new MainWindow(true);
    w->show();
}

void ServerClientSelection::focusInEvent(QFocusEvent *event)
{
    qDebug() << "FocusInEvent called!";
}
void ServerClientSelection::focusOutEvent(QFocusEvent *event)
{
    qDebug() << "FocusOutEvent called!";
}

