#ifndef UNPACKER_H
#define UNPACKER_H

#include <QFile>
#include <QByteArray>
#include <QString>

class Scrambler;

class Unpacker
{
public:
    Unpacker();
    ~Unpacker();
    int Unpack(const QString &sourceArchiveLocation, const QString &destinationFolderLocation);

private:
    QByteArray Read_From_File_Into_Buffer(QFile &file, qint64 startingOffset, qint64 size);

    Scrambler *scrambler;
};

#endif // UNPACKER_H
