#include "Sequential_Archive.h"
#include "Packer.h"
#include "Reader.h"
#include "Common_Strings.h"
#include <QFileInfo>
#include <QTime>

Sequential_Archive::Sequential_Archive() {
    this->reader = NULL;
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
    if (!this->reader->Extract_Directory("/", this->Get_Folder_From_Archive_Path(sourceArchive))) return 2;
    else return 0;
}

int Sequential_Archive::Unpack(const QString &sourceArchive, const QString &destinationFolder) {
    if (!this->Open(sourceArchive)) return 1;
    if (!this->reader->Extract_Directory("/", destinationFolder)) return 2;
    else return 0;
}

int Sequential_Archive::Unpack(const QString &sourceArchive, unsigned char scrambleKey) {
    if (!this->Open(sourceArchive, scrambleKey)) return 1;
    if (!this->reader->Extract_Directory("/", this->Get_Folder_From_Archive_Path(sourceArchive))) return 2;
    else return 0;
}

int Sequential_Archive::Unpack(const QString &sourceArchive, const QString &destinationFolder, unsigned char scrambleKey) {
    if (!this->Open(sourceArchive, scrambleKey)) return 1;
    if (!this->reader->Extract_Directory("/", destinationFolder)) return 2;
    else return 0;
}

bool Sequential_Archive::Open(const QString &archive) {
    this->Close();
    this->reader = new Reader(archive);
    if (this->reader->Open()) {
        return true;
    } else {
        delete this->reader;
        this->reader = NULL;
        return false;
    }
}

bool Sequential_Archive::Open(const QString &archive, unsigned char scrambleKey) {
    this->Close();
    this->reader = new Reader(archive, scrambleKey);
    if (this->reader->Open()) {
        return true;
    } else {
        delete this->reader;
        this->reader = NULL;
        return false;
    }
}

bool Sequential_Archive::Is_Open() {
    return this->reader;
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

unsigned char Sequential_Archive::Get_Archive_Scramble_Key() {
    if (!this->reader) return 0x00;
    return this->reader->Get_Archive_Scramble_Key();
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
    return sourceFolder + "." + Common_Strings::EXTENSION;
}

QString Sequential_Archive::Get_Folder_From_Archive_Path(const QString &archivePath) {
    QFileInfo archivePathInfo(archivePath);
    return archivePathInfo.path() + "/" + archivePathInfo.baseName();
}

