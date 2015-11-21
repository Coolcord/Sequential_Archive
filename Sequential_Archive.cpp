#include "Sequential_Archive.h"
#include "Packer.h"
#include "Reader.h"
#include "Common_Strings.h"
#include <QFileInfo>
#include <QTime>

Sequential_Archive::Sequential_Archive() {
    this->parent = NULL;
    this->applicationLocation = QString();
}

void Sequential_Archive::Startup(QWidget *parent, const QString &applicationLocation) {
    this->parent = parent;
    this->applicationLocation = applicationLocation;
    qsrand(QTime::currentTime().msecsSinceStartOfDay());
}

void Sequential_Archive::Shutdown() {
    this->Close();
}

int Sequential_Archive::Pack(const QString &sourceFolder) {
    Packer packer;
    return packer.Pack(sourceFolder, this->Get_Archive_Name_From_Source_Folder(sourceFolder));
}

int Sequential_Archive::Pack(const QString &sourceFolder, const QString &destinationArchive) {
    Packer packer;
    return packer.Pack(sourceFolder, destinationArchive);
}

int Sequential_Archive::Pack(const QString &sourceFolder, unsigned char scrambleKey) {
    Packer packer(scrambleKey);
    return packer.Pack(sourceFolder, this->Get_Archive_Name_From_Source_Folder(sourceFolder));
}

int Sequential_Archive::Pack(const QString &sourceFolder, const QString &destinationArchive, unsigned char scrambleKey) {
    Packer packer(scrambleKey);
    return packer.Pack(sourceFolder, destinationArchive);
}

int Sequential_Archive::Unpack(const QString &sourceArchive) {
    if (!this->Open(sourceArchive)) return 1;
}

int Sequential_Archive::Unpack(const QString &sourceArchive, const QString &destinationFolder) {

}

int Sequential_Archive::Unpack(const QString &sourceArchive, unsigned char scrambleKey) {

}

int Sequential_Archive::Unpack(const QString &archive, const QString &destinationFolder, unsigned char scrambleKey) {

}

bool Sequential_Archive::Open(const QString &archive) {
    delete this->reader;
    this->reader = new Reader(archive);
    return this->reader->Open();
}

bool Sequential_Archive::Open(const QString &archive, unsigned char scrambleKey) {
    delete this->reader;
    this->reader = new Reader(archive, scrambleKey);
    return this->reader->Open();
}

void Sequential_Archive::Close() {
    if (this->reader) this->reader->Close();
    delete this->reader;
    this->reader = NULL;
}

QString Sequential_Archive::Get_Archive_Name() {
    if (!this->reader) return QString();
    return this->reader->Get_Archive_Name();
}

bool Sequential_Archive::Change_Directory(const QString &directory) {
    if (!this->reader) return false;
    return this->reader->Change_Directory(directory);
}

QStringList Sequential_Archive::Get_Directories() {
    if (!this->reader) return QStringList();
    return this->reader->Get_Directories();
}

QStringList Sequential_Archive::Get_Directories(const QString &pathInArchive) {
    if (!this->reader) return QStringList();
    return this->reader->Get_Directories(pathInArchive);
}

QStringList Sequential_Archive::Get_Files() {
    if (!this->reader) return QStringList();
    return this->reader->Get_Files();
}

QStringList Sequential_Archive::Get_Files(const QString &pathInArchive) {
    if (!this->reader) return QStringList();
    return this->reader->Get_Files(pathInArchive);
}

QByteArray Sequential_Archive::Read_File(const QString &filePathInArchive) {
    if (!this->reader) return QByteArray();
    return this->reader->Read_File(filePathInArchive);
}

bool Sequential_Archive::Extract_File(const QString &filePathInArchive, const QString &destination) {
    if (!this->reader) return false;
    return this->reader->Extract_File(filePathInArchive, destination);
}

bool Sequential_Archive::Extract_Directory(const QString &directoryPathInArchive, const QString &destination) {
    if (!this->reader) return false;
    return this->reader->Extract_Directory(directoryPathInArchive, destination);
}

QString Sequential_Archive::Get_Archive_Name_From_Source_Folder(const QString &sourceFolder) {
    QFileInfo directoryInfo(sourceFolder);
    return (directoryInfo.path() + directoryInfo.fileName() + "." + Common_Strings::EXTENSION);
}

QString Sequential_Archive::Get_Folder_From_Archive_Path(const QString &archivePath) {
    //QFileInfo directoryInfo(archivePath);
    //QString folder = directoryInfo.path();
    //TODO: Write this...

}

