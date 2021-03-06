#include "Main_Window.h"
#include "ui_Main_Window.h"
#include "../Sequential_Archive/Sequential_Archive_Interface.h"
#include "Common_Strings.h"
#include "Error_Messages.h"
#include "Version.h"
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
    this->openLocation = this->applicationLocation;
    this->sequentialArchivePlugin->Startup(this, this->applicationLocation);
    ui->setupUi(this);
    this->setWindowTitle(Common_Strings::STRING_SAM+" "+Version::VERSION);
}

Main_Window::~Main_Window() {
    delete ui;
}

void Main_Window::on_btnPack_clicked() {
    QString folderLocation = QFileDialog::getExistingDirectory(this, "Select a folder to pack", this->openLocation);
    if (folderLocation.isEmpty()) return;
    QFileInfo folderInfo(folderLocation);
    this->openLocation = folderInfo.path();
    QString outputArchiveLocation = folderInfo.path()+"/"+folderInfo.fileName()+Common_Strings::STRING_EXTENSION;
    if (QFileInfo(outputArchiveLocation).exists()) {
        if (!this->ui->cbOverwrite->isChecked() || !QFile(outputArchiveLocation).remove()) {
            outputArchiveLocation = QFileDialog::getSaveFileName(this, "Choose where to save the "+Common_Strings::STRING_SEQUENTIAL_ARCHIVE,
                                                                   this->openLocation, Common_Strings::STRING_EXTENSION_FILTERS);
        }
    }
    if (outputArchiveLocation.isEmpty()) return;
    int result = this->sequentialArchivePlugin->Pack(folderLocation, outputArchiveLocation);
    switch (result) {
    case 0:     this->errorMessages->Show_Information("Successfully packed "+folderInfo.fileName()+"!"); break;
    case 1:     this->errorMessages->Show_Read_Error(folderInfo.fileName()); break;
    default:
    case 2:     this->errorMessages->Show_Error("Unable to pack "+folderInfo.fileName()+"!"); break;
    case 3:     this->errorMessages->Show_Error("Folder is empty! There is nothing to pack!"); break;
    }
}

void Main_Window::on_btnUnpack_clicked() {
    QString archiveLocation = QFileDialog::getOpenFileName(this, "Open a "+Common_Strings::STRING_SEQUENTIAL_ARCHIVE,
                                                           this->openLocation, Common_Strings::STRING_EXTENSION_FILTERS);
    if (archiveLocation.isEmpty()) return;
    QFileInfo archiveInfo(archiveLocation);
    this->openLocation = archiveInfo.path();
    QString outputFolderLocation = archiveInfo.path()+"/"+archiveInfo.completeBaseName();
    if (QDir(outputFolderLocation).exists()) {
        if (!this->ui->cbOverwrite->isChecked() || !QDir(outputFolderLocation).removeRecursively()) {
            outputFolderLocation = QFileDialog::getExistingDirectory(this, "Choose where to extract "+archiveInfo.fileName(), this->openLocation);
        }
    }
    if (outputFolderLocation.isEmpty()) return;
    int result = this->sequentialArchivePlugin->Unpack(archiveLocation, outputFolderLocation);
    switch (result) {
    case 0:     this->errorMessages->Show_Information("Successfully unpacked "+archiveInfo.fileName()+"!"); break;
    case 1:     this->errorMessages->Show_Read_Error(archiveInfo.fileName()); break;
    default:
    case 2:     this->errorMessages->Show_Error("Unable to unpack "+archiveInfo.fileName()+"!"); break;
    }
}

void Main_Window::on_btnView_clicked() {
    QString archiveLocation = QFileDialog::getOpenFileName(this, Common_Strings::STRING_SEQUENTIAL_ARCHIVE_MANAGER,
                                                        this->openLocation, Common_Strings::STRING_EXTENSION_FILTERS);
    if (archiveLocation.isEmpty()) return;
    QFileInfo archiveInfo(archiveLocation);
    this->openLocation = archiveInfo.path();
    Viewer viewer(this, this->sequentialArchivePlugin, archiveInfo.fileName());
    if (!this->sequentialArchivePlugin->Open(archiveLocation) || !viewer.Populate_Window()) {
        this->errorMessages->Show_Unable_To_Open_Error(archiveInfo.fileName());
        return;
    }
    viewer.exec();
    this->sequentialArchivePlugin->Close();
}
