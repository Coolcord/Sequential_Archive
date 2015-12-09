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
    this->archiveLocation = QString();
    this->currentArchivePath = "/";
    this->tmpDir = NULL;

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
    if (this->tmpDir) this->tmpDir->remove();
    delete this->tmpDir;
}

void MainWindow::on_actionNew_triggered() {
    if (!this->Handle_Unsaved_Changes()) return;

    if (this->tmpDir) this->tmpDir->remove();
    delete this->tmpDir;
    this->archiveLocation = QString();
    this->currentArchivePath = "/";
    this->Create_New_Tmp_Dir();
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
    this->Save();
}

void MainWindow::on_actionSave_as_triggered() {
    if (this->Get_New_Save_Location()) this->Save();
}

void MainWindow::on_actionClose_triggered() {
    this->Close_Archive();
    this->sequentialArchive->Close();
}

void MainWindow::on_actionExit_triggered() {
    this->Close_Archive();
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
    if (this->sequentialArchive->Extract_Directory("/", destination)) {
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

void MainWindow::on_actionPack_triggered() {
    this->Pack();
}

void MainWindow::on_actionUnpack_triggered() {
    this->Unpack();
}

void MainWindow::on_actionAdd_Files_triggered() {
    assert(this->archiveMode != CLOSED);
    bool repack = this->archiveMode == PACKED;

    QStringList files = QFileDialog::getOpenFileNames(this, "Add files", QApplication::applicationDirPath(),
                                                        "All files (*.*)");
    if (files.isEmpty()) return; //the user did not provide any files to add

    if (repack) this->Unpack(); //unpack if necessary
    foreach (QString file, files) {
        //TODO: Write this...
    }
    if (repack) this->Pack(); //repack if necessary
}

void MainWindow::on_actionRemove_Selected_triggered() {
    assert(this->archiveMode != CLOSED);
    //TODO: Write this...
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

bool MainWindow::Create_New_Tmp_Dir() {
    this->tmpDir = new QTemporaryDir();
    if (!this->tmpDir->isValid()) {
        delete this->tmpDir;
        QMessageBox::critical(this, Common_Strings::MANAGER_NAME, "Unable to create a temporary directory to work in!",
                                  Common_Strings::OK);
        return false;
    }
    return true;
}

bool MainWindow::Unpack() {
    assert(this->archiveMode == PACKED);

    //Create a new temporary directory
    delete this->tmpDir;
    this->tmpDir = new QTemporaryDir();

    //TODO: Add a progress bar...
    if (this->sequentialArchive->Extract_Directory("/", this->tmpDir->path())) {
        this->Change_Archive_Mode(UNPACKED);
        QMessageBox::information(this, Common_Strings::MANAGER_NAME, "Successfully unpacked " + QFileInfo(archiveLocation).fileName() + "!",
                                  Common_Strings::OK);
        return true;
    } else {
        this->tmpDir->remove();
        delete this->tmpDir;
        this->tmpDir = NULL;
        QMessageBox::critical(this, Common_Strings::MANAGER_NAME, "Unable to unpack " + QFileInfo(archiveLocation).fileName() + "!",
                                  Common_Strings::OK);
        return false;
    }
}

bool MainWindow::Pack() {
    assert(this->archiveMode == UNPACKED);
    assert(this->tmpDir);

    //Ask the user where to save the archive if it wasn't already specified
    if (this->archiveLocation.isEmpty() && !this->Get_New_Save_Location()) return true;

    //TODO: Add a progress bar...
    if (this->sequentialArchive->Pack(this->tmpDir->path(), this->archiveLocation)) {
        this->tmpDir->remove();
        delete this->tmpDir;
        this->Change_Archive_Mode(PACKED);
        QMessageBox::information(this, Common_Strings::MANAGER_NAME, "Successfully packed " + QFileInfo(archiveLocation).fileName() + "!",
                                 Common_Strings::OK);
        return true;
    } else {
        QMessageBox::critical(this, Common_Strings::MANAGER_NAME, "Unable to pack " + QFileInfo(archiveLocation).fileName() + "!",
                              Common_Strings::OK);
        return false;
    }
}

bool MainWindow::Save() {
    assert(this->archiveMode != CLOSED);
    if (this->archiveMode == PACKED) return true; //the archive is already saved
    return this->Pack();
}

void MainWindow::Close_Archive() {
    this->archiveLocation = QString();
    this->currentArchivePath = "/";
    this->Change_Archive_Mode(CLOSED);
}

bool MainWindow::Get_New_Save_Location() {
    QString archiveLocation = QFileDialog::getSaveFileName(this, "Save Location", QApplication::applicationDirPath(),
                                                            Common_Strings::FORMAT_NAME + "s {*." + Common_Strings::EXTENSION + ");;All files (*.*)");
    if (archiveLocation.isEmpty()) return false; //the user aborted the save
    this->archiveLocation = archiveLocation;
    return true;
}

bool MainWindow::Handle_Unsaved_Changes() {
    if (this->archiveMode != CLOSED) {
        int reply = QMessageBox::question(this, Common_Strings::MANAGER_NAME, "You have unsaved changes! Would you like to save them?",
                                          QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) return false;
        if (reply == QMessageBox::Yes) this->Save();
    }

    return true;
}
