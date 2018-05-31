#ifndef SERVERCONTROLWINDOW_H
#define SERVERCONTROLWINDOW_H

#include <QDialog>
#include <QHash>
#include "tcpserver.h"

namespace Ui {
class ServerControlWindow;
}

class ServerControlWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ServerControlWindow(QWidget *parent = 0);

    virtual void reject();
    void updateUI(QHash<unsigned char, PlayerInfo> &hash, GameInfo &info);

    ~ServerControlWindow();

signals:
    rejectSignal();
    startGameSignal();
    stopCurrentGameSignal();

private slots:
    void on_startGameButton_clicked();
    void on_quitButton_clicked();

private:
    Ui::ServerControlWindow *ui;
};

#endif // SERVERCONTROLWINDOW_H
