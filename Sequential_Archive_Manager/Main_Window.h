#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDialog>

namespace Ui {
class Main_Window;
}

class Main_Window : public QDialog
{
    Q_OBJECT

public:
    Main_Window(QWidget *parent);
    ~Main_Window();

private slots:
    void on_btnPack_clicked();
    void on_btnUnpack_clicked();
    void on_btnView_clicked();

private:
    Ui::Main_Window *ui;
};

#endif // MAIN_WINDOW_H
