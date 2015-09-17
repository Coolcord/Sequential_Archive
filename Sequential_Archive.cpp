#include "Sequential_Archive.h"
#include "Packer.h"
#include "Unpacker.h"

Sequential_Archive::Sequential_Archive() {
    this->parent = NULL;
    this->applicationLocation = QString();
}

void Sequential_Archive::Startup(QWidget *parent, const QString &applicationLocation) {
    this->parent = parent;
    this->applicationLocation = applicationLocation;
}

void Sequential_Archive::Shutdown() {

}

int Sequential_Archive::Pack(const QString &sourceFolder) {

}

int Sequential_Archive::Pack(const QString &sourceFolder, const QString &destinationArchive) {
    Packer packer;
    return packer.Pack(sourceFolder, destinationArchive);
}

int Sequential_Archive::PackAndScramble(const QString &sourceFolder) {

}

int Sequential_Archive::PackAndScramble(const QString &sourceFolder, const QString &destinationArchive) {

}

int Sequential_Archive::Unpack(const QString &sourceArchive) {

}

int Sequential_Archive::Unpack(const QString &sourceArchive, const QString &destinationFolder) {
    Unpacker unpacker;
    return unpacker.Unpack(sourceArchive, destinationFolder);
}

int Sequential_Archive::UnscrambleAndUnpack(const QString &sourceArchive) {

}

int Sequential_Archive::UnscrambleAndUnpack(const QString &archive, const QString &destinationFolder) {

}

