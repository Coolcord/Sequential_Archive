#ifndef PACKER_H
#define PACKER_H

#include <QFile>
#include <QByteArray>
#include <QFileInfo>
#include <QString>

class Scrambler;

class Packer
{
public:
    Packer();
    Packer(unsigned char scrambleKey);
    ~Packer();
    int Pack(const QString &sourceFolderLocation, const QString &destinationArchiveLocation);

private:
    bool Write_Archive_Header(QFile &file, const QFileInfo &sourceFolderInfo);
    bool Pack_Directory(QFile &file, const QString &sourceFolderLocation);
    bool Pack_File(QFile &file, const QString &sourceFileLocation, qint64 &compressedSize);
    bool Pack_File_With_Buffers(QFile &file, QFile &sourceFile, qint64 fileSize, qint64 &compressedSize);
    QByteArray Get_Byte_Array_From_qint64(qint64 number);
    QByteArray Get_Byte_Array_From_int(int number);
    bool Compress_And_Write_Buffer_To_File(QFile &file, QByteArray buffer, qint64 &compressedSize);
    bool Write_Buffer_To_File(QFile &file, QByteArray buffer);
    bool Write_Buffer_To_File(QFile &file, QByteArray buffer, qint64 offset);

    Scrambler *scrambler;
};

#endif // PACKER_H
