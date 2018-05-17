#ifndef SERVERCLIENTSELECTION_H
#define SERVERCLIENTSELECTION_H

#include <QDialog>

namespace Ui {
class ServerClientSelection;
}

class ServerClientSelection : public QDialog
{
    Q_OBJECT

public:
    explicit ServerClientSelection(QWidget *parent = 0);
    ~ServerClientSelection();

private slots:

    void on_clientButtonClicked_clicked();

    void on_serverButtonClicked_clicked();

private:
    Ui::ServerClientSelection *ui;
};

#endif // SERVERCLIENTSELECTION_H
