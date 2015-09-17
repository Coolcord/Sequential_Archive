#include "Packer.h"
#include "Common_Strings.h"
#include "Common_Data.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QHash>
#include <assert.h>

Packer::Packer() {
    this->file = NULL;
}

Packer::~Packer() {

}

int Packer::Pack(const QString &sourceFolderLocation, const QString &destinationArchiveLocation) {
    this->destinationArchiveLocation = destinationArchiveLocation;

    //Open the source folder and create the destination archive file
    QFileInfo folderInfo(sourceFolderLocation);
    if (!folderInfo.exists() || !folderInfo.isReadable()) return 1; //unable to read the source folder
    if (!folderInfo.isDir()) return 2; //source folder is not a directory

    //Open the source folder and make sure it isn't empty
    QDir dir(sourceFolderLocation);
    if (dir.count() == 0) return 3; //directory is empty

    //Create the archive file and write the header
    this->file = new QFile(destinationArchiveLocation);
    if (this->file->open(QFile::ReadWrite | QFile::Truncate)) return 3; //unable to create the archive file
    if (!this->Write_Archive_Header(folderInfo.fileName())) return 4; //unable to write the archive file header

    return this->Pack_Directory(sourceFolderLocation);
}

bool Packer::Write_Archive_Header(const QString &name) {
    assert(this->file);
    if (!this->file->seek(0)) return false;
    QString header = Common_Strings::FORMAT_NAME + "\n" + name + "\n";
    return (this->file->write(header.toUtf8().data()) != -1);
}

bool Packer::Pack_Directory(const QString &sourceFolderLocation) {
    QDir dir(sourceFolderLocation);
    assert(dir.count() > 0); //empty folders should never enter this function

    //Create a table entry for each directory
    QFileInfoList directories = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    QHash<QString, qint64> directoryTable;
    foreach (QFileInfo directory, directories) {
        if (directory.dir().count() == 0) continue; //we won't pack empty folders

        //Insert the directory into the hash to update it later
        directoryTable.insert(directory.fileName(), this->file->pos());

        //Allocate 16 bytes for the start and end pointers
        if (this->file->write(QByteArray(16, ' ')) != 16) return false;
        if (this->file->write(directory.fileName().toUtf8().data()) == -1) return false;
    }
    QByteArray endByte(1, static_cast<char>(Common_Data::END_SECTION));
    if (!this->file->write(endByte) != 1) return false; //write the end byte

    //Create a table entry for each file
    QFileInfoList files = dir.entryInfoList(QDir::Files);
    QHash<QString, qint64> fileTable;
    foreach (QFileInfo file, files) {
        //Insert the file into the hash to update it later
        fileTable.insert(file.fileName(), this->file->pos());

        //Allocate 16 bytes for the start and end pointers
        if (this->file->write(QByteArray(16, ' ')) != 16) return false;
        if (this->file->write(file.fileName().toUtf8().data()) == -1) return false;
    }
    if (!this->file->seek(this->file->size())) return false;
    if (!this->file->write(endByte) != 1) return false; //write the end byte

    //Pack each file. Update the table entry for each file along the way
    foreach (QFileInfo file, files) {
        qint64 startByte = this->file->pos();
        if (!this->Pack_File(file.path())) return false;
        qint64 endByte = this->file->pos();

        //Update the table entry with the new pointers
        if (!this->file->seek(fileTable.value(file.fileName()))) return false;
        if (this->file->write(QString::number(startByte).toUtf8().data()) != 8) return false;
        if (this->file->write(QString::number(endByte).toUtf8().data()) != 8) return false;
    }

    //Pack every subdirectory
    if (!this->file->seek(this->file->size())) return false;
    foreach (QFileInfo directory, directories) {
        if (directory.dir().count() == 0) continue; //ignore empty folders

        //TODO: Directories need a start and end pointer.
        //      However, these pointers will only refer to the index table.
        //      The root directory does not need a start pointer.

        if (!this->Pack_Directory(directory.path())) return false;
    }
    return true;
}

bool Packer::Pack_File(const QString &sourceFileLocation) {
    QFile sourceFile(sourceFileLocation);
    if (!sourceFile.open(QFile::ReadOnly)) return false;

    //Determine if the entire file can fit into one buffer
    qint64 fileSize = sourceFile.size();
    if (fileSize > Common_Data::MAX_BUFFER_SIZE) {
        return this->Pack_File_With_Buffers(&sourceFile, fileSize);
    } else {
        return (this->file->write(sourceFile.readAll()) == fileSize);
    }
}

bool Packer::Pack_File_With_Buffers(QFile *sourceFile, qint64 fileSize) {
    assert(sourceFile);
    while (fileSize > 0) {
        qint64 bufferSize = Common_Data::MAX_BUFFER_SIZE;
        if (fileSize < Common_Data::MAX_BUFFER_SIZE) bufferSize = fileSize;
        if (this->file->write(sourceFile->read(bufferSize)) != bufferSize) return false;
        fileSize -= bufferSize;
    }
    return true;
}
