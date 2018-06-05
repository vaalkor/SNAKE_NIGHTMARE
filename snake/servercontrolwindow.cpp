#include "servercontrolwindow.h"
#include "ui_servercontrolwindow.h"
#include "gameobjectsanddata.h"

#include <QDebug>
#include <QAbstractListModel>
#include <QApplication>
#include <QCoreApplication>
#include <QDesktopWidget>


ServerControlWindow::ServerControlWindow(GameParameters *params_, QWidget *parent)
    : gameParameters(params_), QDialog(parent), ui(new Ui::ServerControlWindow)
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

void ServerControlWindow::on_enableSprintCheckbox_clicked(bool checked)
{
    gameParameters->sprintEnabled = checked;
}

void ServerControlWindow::on_enableBombsCheckbox_clicked(bool checked)
{
    gameParameters->bombsEnabled = checked;
}

void ServerControlWindow::on_speedInputSlider_sliderMoved(int position)
{
    float proportion = (1.0 - ( position/100.0 ) ) ;
    gameParameters->tickLength = MIN_TICKRATE + proportion * (MAX_TICKRATE - MIN_TICKRATE);
}

void ServerControlWindow::on_enablePUBGModeCheckbox_clicked(bool checked)
{
    gameParameters->PUBGmodeEnabled = checked;
}

void ServerControlWindow::on_enableRevengeModeCheckbox_clicked(bool checked)
{
    gameParameters->revengeModeEnabled = checked;
}

void ServerControlWindow::on_sprintLengthInputBox_textEdited(const QString &arg1)
{
    float value = arg1.toFloat();
    if(value == 0)
        value = 1;
    value *= 1000;  //seconds->milliseconds
    gameParameters->sprintLength = value;
}

void ServerControlWindow::on_sprintRechargeLengthInputBox_textEdited(const QString &arg1)
{
    float value = arg1.toFloat();
    if(value == 0)
        value = 20;
    value *= 1000;  //seconds->milliseconds
    gameParameters->sprintRechargeLength = value;
}

void ServerControlWindow::on_bombRechargeLengthInputBox_textEdited(const QString &arg1)
{
    float value = arg1.toFloat();
    if(value == 0)
        value = 10;
    value *= 1000;  //seconds->milliseconds
    gameParameters->bombChargeTime = value;
}

void ServerControlWindow::on_PUBGCircleTimeInputBox_textEdited(const QString &arg1)
{
    float value = arg1.toFloat();
    if(value == 0)
        value = 2.5;
    value*=1000;    //seconds->milliseconds
    gameParameters->PUBGCircleTime = value;
}

void ServerControlWindow::on_PUBGWallIncreaseInputBox_textEdited(const QString &arg1)
{
    float value = arg1.toInt();
    if(value == 0)
        value = 2;
    gameParameters->PUBGWallIncrease = value;
}
