#ifndef SERVERCONTROLWINDOW_H
#define SERVERCONTROLWINDOW_H

#include <QDialog>

namespace Ui {
class ServerControlWindow;
}

class ServerControlWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ServerControlWindow(QWidget *parent = 0);
    ~ServerControlWindow();

private slots:
    void on_startGameButton_clicked();
    void on_stopGameButton_clicked();
    void on_quitButton_clicked();

private:
    Ui::ServerControlWindow *ui;
};

#endif // SERVERCONTROLWINDOW_H
