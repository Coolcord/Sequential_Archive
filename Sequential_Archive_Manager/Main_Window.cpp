#include "Main_Window.h"
#include "ui_mainwindow.h"
#include "../Sequential_Archive/Sequential_Archive_Interface.h"
#include "Manager_Common_Strings.h"
#include <QFileDialog>
#include <QMessageBox>
#include <assert.h>

MainWindow::MainWindow(Sequential_Archive_Interface *sequentialArchive, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    this->ui->setupUi(this);
    this->sequentialArchive = sequentialArchive;
    this->sequentialArchive->Startup(this, QApplication::applicationDirPath());

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
    this->sequentialArchive->Shutdown();
    delete this->sequentialArchive;
}

void MainWindow::on_actionNew_triggered() {

}

void MainWindow::on_actionOpen_triggered() {
    QString fileLocation = QFileDialog::getOpenFileName(this, "Open a " + Common_Strings::FORMAT_NAME, QApplication::applicationDirPath(),
                                                        Common_Strings::FORMAT_NAME + "s {*." + Common_Strings::EXTENSION + ");;All files (*.*)");
    if (!this->sequentialArchive->Open(fileLocation)) {
        QMessageBox::critical(this, Common_Strings::MANAGER_NAME, "Unable to open " + QFileInfo(fileLocation).fileName() + "!",
                                  Common_Strings::OK);
    }
    this->archiveMode = PACKED;
}

void MainWindow::on_actionSave_triggered() {

}

void MainWindow::on_actionSave_as_triggered()
{

}

void MainWindow::on_actionClose_triggered() {
    this->sequentialArchive->Close();
}

void MainWindow::on_actionExit_triggered() {
    this->sequentialArchive->Close();
    this->close();
}

void MainWindow::on_actionAbout_Sequential_Archive_Manager_triggered()
{

}

void MainWindow::on_actionExtract_All_triggered() {

}

void MainWindow::on_actionExtract_Selected_triggered() {

}

void MainWindow::on_actionShow_In_Explorer_triggered() {

}

void MainWindow::on_actionShow_in_triggered() {

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
