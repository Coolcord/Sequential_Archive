#include "Main_Window.h"
#include "ui_Main_Window.h"
#include "../Sequential_Archive/Sequential_Archive_Interface.h"
#include "Common_Strings.h"
#include "Error_Messages.h"
#include "Viewer.h"
#include <assert.h>
#include <QFileDialog>

Main_Window::Main_Window(QWidget *parent, Sequential_Archive_Interface *sequentialArchivePlugin, Error_Messages *errorMessages) :
    QDialog(parent, Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::Main_Window)
{
    assert(sequentialArchivePlugin);
    assert(errorMessages);
    this->sequentialArchivePlugin = sequentialArchivePlugin;
    this->errorMessages = errorMessages;
    this->applicationLocation = QApplication::applicationDirPath();
    ui->setupUi(this);
}

Main_Window::~Main_Window() {
    delete ui;
}

void Main_Window::on_btnPack_clicked() {
    QString folderLocation = QFileDialog::getExistingDirectory(this, "Select a folder to pack", this->applicationLocation);
    if (folderLocation.isEmpty()) return;
    QFileInfo folderInfo(folderLocation);
    QString outputArchiveLocation = folderInfo.path()+folderInfo.fileName()+Common_Strings::STRING_EXTENSION;
    if (QFileInfo(outputArchiveLocation).exists()) {
        outputArchiveLocation = QFileDialog::getSaveFileName(this, "Choose where to save the "+Common_Strings::STRING_SEQUENTIAL_ARCHIVE,
                                                               folderInfo.path(), Common_Strings::STRING_EXTENSION_FILTERS);
    }
    if (outputArchiveLocation.isEmpty()) return;
    int result = this->sequentialArchivePlugin->Pack(folderLocation, outputArchiveLocation);
    switch (result) {
    case 0:     this->errorMessages->Show_Information("Successfully packed "+folderInfo.fileName()+"!"); return;
    case 1:     this->errorMessages->Show_Read_Error(folderInfo.fileName()); return;
    default:
    case 2:     this->errorMessages->Show_Error("Unable to pack "+folderInfo.fileName()+"!"); return;
    case 3:     this->errorMessages->Show_Error("Folder is empty! There is nothing to pack!");
    }
}

void Main_Window::on_btnUnpack_clicked() {
    QString archiveLocation = QFileDialog::getOpenFileName(this, "Open a "+Common_Strings::STRING_SEQUENTIAL_ARCHIVE,
                                                           this->applicationLocation, Common_Strings::STRING_EXTENSION_FILTERS);
    if (archiveLocation.isEmpty()) return;
    QFileInfo archiveInfo(archiveLocation);
    QString outputFolderLocation = archiveInfo.path()+"/"+archiveInfo.completeBaseName();
    if (QDir(outputFolderLocation).exists()) {
        outputFolderLocation = QFileDialog::getExistingDirectory(this, "Choose where to extract "+archiveInfo.fileName(), archiveInfo.path());
    }
    if (outputFolderLocation.isEmpty()) return;
    int result = this->sequentialArchivePlugin->Unpack(archiveLocation, outputFolderLocation);
    switch (result) {
    case 0:     this->errorMessages->Show_Information("Successfully unpacked "+archiveInfo.fileName()+"!"); return;
    case 1:     this->errorMessages->Show_Read_Error(archiveInfo.fileName()); return;
    default:
    case 2:     this->errorMessages->Show_Error("Unable to unpack "+archiveInfo.fileName()+"!"); return;
    }
}

void Main_Window::on_btnView_clicked() {
    QString archiveLocation = QFileDialog::getOpenFileName(this, Common_Strings::STRING_SEQUENTIAL_ARCHIVE_MANAGER,
                                                        this->applicationLocation, Common_Strings::STRING_EXTENSION_FILTERS);
    if (archiveLocation.isEmpty()) return;
    if (!this->sequentialArchivePlugin->Open(archiveLocation)) {
        QFileInfo archiveInfo(archiveLocation);
        this->errorMessages->Show_Error("Unable to open "+archiveInfo.fileName()+"! Make sure that it is a valid "+Common_Strings::STRING_SEQUENTIAL_ARCHIVE+"!");
        return;
    }
    Viewer viewer(this, this->sequentialArchivePlugin);
    viewer.exec();
    this->sequentialArchivePlugin->Close();
}
