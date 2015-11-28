#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QSplitter>
#include <QTreeView>
#include <QTextBrowser>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSave_as_triggered();

    void on_actionClose_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_Sequential_Archive_Manager_triggered();

    void on_actionExtract_All_triggered();

    void on_actionExtract_Selected_triggered();

    void on_actionShow_In_Explorer_triggered();

    void on_actionShow_in_triggered();

    void on_actionPack_triggered();

    void on_actionUnpack_triggered();

    void on_actionAdd_Files_triggered();

    void on_actionRemove_Selected_triggered();

private:
    Ui::MainWindow *ui;
    QSplitter *splitter;
    QTreeView *treeView;
    QTextBrowser *textBrowser;
};

#endif // MAINWINDOW_H
