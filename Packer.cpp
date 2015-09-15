#include "Packer.h"
#include "Common_Strings.h"
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

int Packer::Pack(const QString &sourceFolder, const QString &destinationArchive) {
    //Open the source folder and create the destination archive file
    QFileInfo folderInfo(sourceFolder);
    if (!folderInfo.exists() || !folderInfo.isReadable()) return 1; //unable to read the source folder
    if (!folderInfo.isDir()) return 2; //source folder is not a directory

    //Open the source folder and make sure it isn't empty
    QDir dir(sourceFolder);
    if (dir.count() == 0) return 3; //directory is empty

    //Create the archive file and write the header
    QFile file(destinationArchive);
    if (!file.open(QFile::ReadWrite | QFile::Truncate)) return 3; //unable to create the archive file
    if (!this->Write_Header(&file, folderInfo.fileName())) return 4; //unable to write the archive file header

    //TODO: Iterate though all of the folders and allocate space for their start and end pointers
    foreach (QString directory, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot)) {

    }
    assert(false);
    return 0;
}

bool Packer::Write_Header(QFile *file, const QString &name) {
    assert(file);
    if (!this->file->seek(0)) return false;
    QString header = Common_Strings::FORMAT_NAME + "\n" + name + "\n";
    return (this->file->write(header.toUtf8().data()) != -1);
}
