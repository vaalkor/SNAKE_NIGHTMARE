#ifndef SERVERCONTROLWINDOW_H
#define SERVERCONTROLWINDOW_H

#include <QDialog>
#include <QHash>
#include "gameobjectsanddata.h"

namespace Ui {
class ServerControlWindow;
}

class ServerControlWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ServerControlWindow(GameParameters *params_, QWidget *parent = 0);

    virtual void reject();
    void updateUI(QHash<unsigned char, PlayerInfo> &hash, GameState &state);

    GameParameters *gameParameters;

    ~ServerControlWindow();

signals:
    rejectSignal();
    startGameSignal();
    stopCurrentGameSignal();

private slots:
    void on_startGameButton_clicked();
    void on_quitButton_clicked();
    void on_enableSprintCheckbox_clicked(bool checked);
    void on_enableBombsCheckbox_clicked(bool checked);
    void on_speedInputSlider_sliderMoved(int position);

    void on_enablePUBGModeCheckbox_clicked(bool checked);

    void on_enableRevengeModeCheckbox_clicked(bool checked);

private:
    Ui::ServerControlWindow *ui;
};

#endif // SERVERCONTROLWINDOW_H
