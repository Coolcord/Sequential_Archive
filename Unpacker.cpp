#include "Unpacker.h"
#include "Scrambler.h"

Unpacker::Unpacker() {
    this->scrambler = NULL;
}

Unpacker::~Unpacker() {
    delete this->scrambler;
}

int Unpacker::Unpack(const QString &sourceArchiveLocation, const QString &destinationFolderLocation) {

}

QByteArray Unpacker::Read_From_File_Into_Buffer(QFile &file, qint64 startingOffset, qint64 size) {

}

