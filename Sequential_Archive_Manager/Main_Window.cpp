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
    this->tmpLocation = QString();
    this->archiveLocation = QString();
    this->currentArchivePath = "/";

    this->splitter = new QSplitter(this);
    this->ui->horizontalLayout->addWidget(this->splitter);
    this->treeView = new QTreeView(this->splitter);
    this->splitter->addWidget(this->treeView);
    this->textBrowser = new QTextBrowser(this->splitter);
    this->splitter->addWidget(this->textBrowser);

    this->Change_Archive_Mode(CLOSED);
}

MainWindow::~MainWindow() {
    delete this->ui;
    delete this->splitter;
    delete this->treeView;
    delete this->textBrowser;
    this->sequentialArchive->Shutdown();
    delete this->sequentialArchive;
    delete this->tmpDir;
}

void MainWindow::on_actionNew_triggered() {
    delete this->tmpDir;
    this->tmpDir = new QTemporaryDir();
    if (!this->tmpDir->isValid()) {
        delete this->tmpDir;
        QMessageBox::critical(this, Common_Strings::MANAGER_NAME, "Unable to create a temporary directory to work in!",
                                  Common_Strings::OK);
        return;
    }

    this->Change_Archive_Mode(UNPACKED);
}

void MainWindow::on_actionOpen_triggered() {
    QString archiveLocation = QFileDialog::getOpenFileName(this, "Open a " + Common_Strings::FORMAT_NAME, QApplication::applicationDirPath(),
                                                        Common_Strings::FORMAT_NAME + "s {*." + Common_Strings::EXTENSION + ");;All files (*.*)");
    if (archiveLocation.isEmpty()) return; //nothing to do. The user did not select anything
    if (!this->sequentialArchive->Open(archiveLocation)) {
        QMessageBox::critical(this, Common_Strings::MANAGER_NAME, "Unable to open " + QFileInfo(archiveLocation).fileName() + "!",
                                  Common_Strings::OK);
        return;
    }
    this->archiveLocation = archiveLocation;
    this->currentArchivePath = "/";
    this->Change_Archive_Mode(PACKED);
}

void MainWindow::on_actionSave_triggered() {
    switch (this->archiveMode) {
    default:
    case CLOSED:    assert(false);
    case PACKED:    return; //nothing to do
    case UNPACKED:
        //TODO: Write this...
        return;
    }
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
    QString destination = QFileDialog::getExistingDirectory(this, "Select a folder to extract to", QApplication::applicationDirPath());
    if (destination.isEmpty()) return; //nothing to do. The user didn't select a folder

    //TODO: Add a progress bar...
    if (this->sequentialArchive->Extract_Directory("/7", destination)) {
        QMessageBox::information(this, Common_Strings::MANAGER_NAME, "Successfully extracted " + QFileInfo(archiveLocation).fileName() + "!",
                                  Common_Strings::OK);
    } else {
        QMessageBox::critical(this, Common_Strings::MANAGER_NAME, "Unable to extract " + QFileInfo(archiveLocation).fileName() + "!",
                                  Common_Strings::OK);
    }
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

void MainWindow::Change_Archive_Mode(int archiveMode) {
    this->archiveMode = archiveMode;
    switch (this->archiveMode) {
    default:
        assert(false);
    case CLOSED:
        this->ui->actionSave->setEnabled(false);
        this->ui->actionSave_as->setEnabled(false);
        this->ui->actionClose->setEnabled(false);
        this->ui->actionPack->setEnabled(false);
        this->ui->actionUnpack->setEnabled(false);
        this->ui->actionAdd_Files->setEnabled(false);
        this->ui->actionRemove_Selected->setEnabled(false);
        this->ui->actionShow_In_Explorer->setEnabled(false);
        this->ui->actionShow_in->setEnabled(false);
        this->ui->actionExtract_All->setEnabled(false);
        this->ui->actionExtract_Selected->setEnabled(false);
        this->treeView->setEnabled(false);
        this->textBrowser->setEnabled(false);
        return;
    case PACKED:
        this->ui->actionSave->setEnabled(true);
        this->ui->actionSave_as->setEnabled(true);
        this->ui->actionClose->setEnabled(true);
        this->ui->actionPack->setEnabled(false);
        this->ui->actionUnpack->setEnabled(true);
        this->ui->actionAdd_Files->setEnabled(true);
        this->ui->actionRemove_Selected->setEnabled(true);
        this->ui->actionShow_In_Explorer->setEnabled(true);
        this->ui->actionShow_in->setEnabled(true);
        this->ui->actionExtract_All->setEnabled(true);
        this->ui->actionExtract_Selected->setEnabled(true);
        this->treeView->setEnabled(true);
        this->textBrowser->setEnabled(true);
        return;
    case UNPACKED:
        this->ui->actionSave->setEnabled(true);
        this->ui->actionSave_as->setEnabled(true);
        this->ui->actionClose->setEnabled(true);
        this->ui->actionPack->setEnabled(true);
        this->ui->actionUnpack->setEnabled(false);
        this->ui->actionAdd_Files->setEnabled(true);
        this->ui->actionRemove_Selected->setEnabled(true);
        this->ui->actionShow_In_Explorer->setEnabled(true);
        this->ui->actionShow_in->setEnabled(true);
        this->ui->actionExtract_All->setEnabled(true);
        this->ui->actionExtract_Selected->setEnabled(true);
        this->treeView->setEnabled(true);
        this->textBrowser->setEnabled(true);
        return;
    }
}