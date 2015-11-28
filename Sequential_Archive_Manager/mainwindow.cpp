#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    this->ui->setupUi(this);

    this->splitter = new QSplitter(this);
    this->ui->horizontalLayout->addWidget(this->splitter);
    this->treeView = new QTreeView(this->splitter);
    this->splitter->addWidget(this->treeView);
    this->textBrowser = new QTextBrowser(this->splitter);
    this->splitter->addWidget(this->textBrowser);
}

MainWindow::~MainWindow() {
    delete this->ui;
    delete this->splitter;
    delete this->treeView;
    delete this->textBrowser;
}

void MainWindow::on_actionNew_triggered()
{

}

void MainWindow::on_actionOpen_triggered()
{

}

void MainWindow::on_actionSave_triggered()
{

}

void MainWindow::on_actionSave_as_triggered()
{

}

void MainWindow::on_actionClose_triggered()
{

}

void MainWindow::on_actionExit_triggered() {
    this->close();
}

void MainWindow::on_actionAbout_Sequential_Archive_Manager_triggered()
{

}

void MainWindow::on_actionExtract_All_triggered()
{

}

void MainWindow::on_actionExtract_Selected_triggered()
{

}

void MainWindow::on_actionShow_In_Explorer_triggered()
{

}

void MainWindow::on_actionShow_in_triggered()
{

}

void MainWindow::on_actionPack_triggered()
{

}

void MainWindow::on_actionUnpack_triggered()
{

}

void MainWindow::on_actionAdd_Files_triggered()
{

}

void MainWindow::on_actionRemove_Selected_triggered()
{

}
