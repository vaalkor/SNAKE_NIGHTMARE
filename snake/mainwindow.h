#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPen>
#include <QGraphicsScene>
#include <QPainter>
#include <QRgb>

#include "serverplayer.h"
#include "clientplayer.h"

namespace Ui {
class MainWindow;
}

class ClientWorker;
class ServerWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool isServer_, std::string name_="", bool testingMode_ = false, QHostAddress serverAddress_ = QHostAddress("000.000.000.000"), QWidget *parent = 0);

    virtual void paintEvent(QPaintEvent *event);
    virtual void closeEvent(QCloseEvent *event);

    ServerPlayer *serverPlayer;
    ClientPlayer *clientPlayer;

    QHostAddress serverAddress;

    bool isServer;
    bool testingMode;
    std::string name;

    void draw();
    void drawSquare(short x, short y, QRgb color);

    virtual bool event(QEvent *e);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    QImage image;
    QPainter *painter;
    QPen *sprintBarPen;
    QPen *bombBarPen;
    QPen *barBackGroundPen;
    QBrush *brush;

    ~MainWindow();
signals:
public slots:
    void drawSlot();
    void clientConnectionSuccess();
    void clientConnectionFailure(QAbstractSocket::SocketError err);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
