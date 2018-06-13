#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDialog>
#include <QString>

class Error_Messages;
class Sequential_Archive_Interface;

namespace Ui {
class Main_Window;
}

class Main_Window : public QDialog {
    Q_OBJECT

public:
    Main_Window(QWidget *parent, Sequential_Archive_Interface *sequentialArchivePlugin, Error_Messages *errorMessages);
    ~Main_Window();

private slots:
    void on_btnPack_clicked();
    void on_btnUnpack_clicked();
    void on_btnView_clicked();

private:
    Ui::Main_Window *ui;
    Error_Messages *errorMessages;
    Sequential_Archive_Interface *sequentialArchivePlugin;
    QString applicationLocation;
    QString openLocation;
};

#endif // MAIN_WINDOW_H
