#include "Packer.h"
#include "Common_Strings.h"
#include "Common_Data.h"
#include <QDir>
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
    if (!this->Write_Archive_Header(folderInfo)) return 4; //unable to write the archive file header

    if (!this->Pack_Directory(sourceFolderLocation)) return 5; //unable to pack the directory
    return 0;
}

bool Packer::Write_Archive_Header(const QFileInfo &sourceFolderInfo) {
    assert(this->file);
    if (!this->file->seek(0)) return false;
    QString header = Common_Strings::FORMAT_NAME + "\n" +
            QString::number(this->Get_Index_Table_Size(sourceFolderInfo.filePath())) + sourceFolderInfo.fileName() + static_cast<char>(Common_Data::END_SECTION);
    return (this->file->write(header.toUtf8().data()) != -1);
}

bool Packer::Pack_Directory(const QString &sourceFolderLocation) {
    QDir dir(sourceFolderLocation);
    assert(dir.count() > 0); //empty folders should never enter this function

    //Create a table entry for each directory
    QFileInfoList directories = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
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
        if (!this->file->seek(this->file->size())) return false;
        qint64 startByte = this->file->pos();
        if (!this->Pack_File(file.filePath())) return false;

        //Update the table entry with the new pointers
        if (!this->file->seek(fileTable.value(file.fileName()))) return false;
        if (this->file->write(QString::number(startByte).toUtf8().data()) != 8) return false;
        if (this->file->write(QString::number(startByte+file.size()).toUtf8().data()) != 8) return false;
    }

    //Pack every subdirectory
    foreach (QFileInfo directory, directories) {
        if (directory.dir().count() == 0) continue; //ignore empty folders
        if (!this->file->seek(this->file->size())) return false;

        qint64 startByte = this->file->pos();
        if (!this->Pack_Directory(directory.filePath())) return false;

        //Update the table entry with the new pointers
        if (!this->file->seek(directoryTable.value(directory.fileName()))) return false;
        if (this->file->write(QString::number(startByte).toUtf8().data()) != 8) return false;
        if (this->file->write(QString::number(startByte+this->Get_Index_Table_Size(directory.filePath())).toUtf8().data()) != 8) return false;
    }

    //Seek back to the end of the file
    if (!this->file->seek(this->file->size())) return false;
    return true;
}

bool Packer::Pack_File(const QString &sourceFileLocation) {
    QFile sourceFile(sourceFileLocation);
    if (!sourceFile.open(QFile::ReadOnly)) return false;
    assert(this->file->pos() == this->file->size());

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

int Packer::Get_Index_Table_Size(const QString &sourceFolderLocation) {
    QDir dir = QDir(sourceFolderLocation);
    assert(dir.exists());
    int indexTableSize = 0;

    //Add the size of the index table using the filenames in the folder
    foreach (QFileInfo entry, QDir(sourceFolderLocation).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
        indexTableSize += 16; //start pointer and size
        indexTableSize += entry.fileName().length();
    }
    return indexTableSize;
}
