#include "Packer.h"
#include "Common_Strings.h"
#include "Common_Data.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <assert.h>

Packer::Packer() {
    this->file = NULL;
}

Packer::~Packer() {

}

int Packer::Pack(const QString &sourceFolderLocation, const QString &destinationArchiveLocation) {
    this->destinationArchiveLocation = destinationArchiveLocation;

    //Open the source folder and create the destination archive file
    QFileInfo folderInfo(sourceFolder);
    if (!folderInfo.exists() || !folderInfo.isReadable()) return 1; //unable to read the source folder
    if (!folderInfo.isDir()) return 2; //source folder is not a directory

    //Open the source folder and make sure it isn't empty
    QDir dir(sourceFolder);
    if (dir.count() == 0) return 3; //directory is empty

    //Create the archive file and write the header
    this->file = new QFile(destinationArchive);
    if (this->file->open(QFile::ReadWrite | QFile::Truncate)) return 3; //unable to create the archive file
    if (!this->Write_Archive_Header(folderInfo.fileName())) return 4; //unable to write the archive file header


    assert(false);
    return 0;
}

bool Packer::Write_Archive_Header(const QString &name) {
    assert(this->file);
    if (!this->file->seek(0)) return false;
    QString header = Common_Strings::FORMAT_NAME + "\n" + name + "\n";
    return (this->file->write(header.toUtf8().data()) != -1);
}

bool Packer::Write_Directory_Header(const QString &sourceFolderLocation) {

}

bool Packer::Write_File_Header(const QString &sourceFileLocation) {

}

bool Packer::Pack_Directories(const QString &sourceFolderLocation) {
    QDir dir(sourceFolderLocation);
    if (dir.count() == 0) return true; //nothing to do! We won't pack empty folders

    QFileInfoList directories = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo directory, directories) {
        if (!this->Pack_Files(directory.path()) || !this->Pack_Directories(directory.path())) {
            return false;
        }
    }
}

bool Packer::Pack_Files(const QString &sourceFolderLocation) {
    QDir dir(sourceFolderLocation);
    if (dir.count() == 0) return true; //nothing to do! We won't pack empty folders

    //Get all of the files in the directory
    QFileInfoList files = dir.entryInfoList(QDir::Files);

    //Create a table entry for each file
    foreach (QFileInfo file, files) {
        //TODO: Write this....
    }
    QByteArray endByte(1, static_cast<char>(Common_Data::END_SECTION));
    this->file->write(endByte); //write the end byte

    //Pack each file. Update the table entry for each file along the way
    foreach (QFileInfo file, files) {
        qint64 startByte = this->file->pos();
        if (!this->Pack_File(file.path())) return false;
        qint64 endByte = this->file->pos();
        //Update the table entry with the new values
        //TODO: Write this...
    }

    return true;
}

bool Packer::Pack_File(const QString &sourceFileLocation) {
    QFile sourceFile(sourceFileLocation);
    if (!sourceFile.open(QFile::ReadOnly)) return false;

    //Determine if the entire file can fit into one buffer
    qint64 fileSize = sourceFile.size();
    if (fileSize > Common_Data::MAX_BUFFER_SIZE) {
        return this->Pack_File_With_Buffers(&sourceFile);
    } else {
        return (this->file->write(sourceFile.readAll()) == fileSize);
    }
}

bool Packer::Pack_File_With_Buffers(QFile *sourceFile) {
    assert(sourceFile);

}
