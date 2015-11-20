#include "Sequential_Archive.h"
#include "Packer.h"
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

}

int Sequential_Archive::Pack(const QString &sourceFolder) {
    //Create the archive file with the same name as the source folder
    QFileInfo directoryInfo(sourceFolder);
    if (!directoryInfo.exists() || !directoryInfo.isReadable()) return 1; //unable to read the source folder
    QString destinationArchive = directoryInfo.path() + directoryInfo.fileName() + "." + Common_Strings::EXTENSION;

    Packer packer;
    return packer.Pack(sourceFolder, destinationArchive);
}

int Sequential_Archive::Pack(const QString &sourceFolder, const QString &destinationArchive) {
    Packer packer;
    return packer.Pack(sourceFolder, destinationArchive);
}

int Sequential_Archive::PackAndScramble(const QString &sourceFolder, unsigned char scrambleKey) {

}

int Sequential_Archive::PackAndScramble(const QString &sourceFolder, const QString &destinationArchive, unsigned char scrambleKey) {

}

int Sequential_Archive::Unpack(const QString &sourceArchive) {

}

int Sequential_Archive::Unpack(const QString &sourceArchive, const QString &destinationFolder) {

}

int Sequential_Archive::UnscrambleAndUnpack(const QString &sourceArchive, unsigned char scrambleKey) {

}

int Sequential_Archive::UnscrambleAndUnpack(const QString &archive, const QString &destinationFolder, unsigned char scrambleKey) {

}

