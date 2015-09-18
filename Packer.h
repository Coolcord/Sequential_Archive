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
    Packer(bool scramble);
    Packer(bool scramble, int amount);
    ~Packer();
    int Pack(const QString &sourceFolderLocation, const QString &destinationArchiveLocation);

private:
    bool Write_Archive_Header(const QFileInfo &sourceFolderInfo);
    bool Pack_Directory(const QString &sourceFolderLocation);
    bool Pack_File(const QString &sourceFileLocation);
    bool Pack_File_With_Buffers(const QFile &sourceFile, qint64 fileSize);
    int Get_Index_Table_Size(const QString &sourceFolderLocation);
    bool Write_Buffer_To_File(const QFile &file, QByteArray &buffer);
    bool Write_Buffer_To_File(const QFile &file, QByteArray &buffer, qint64 offset);

    Scrambler *scrambler;
    QString destinationArchiveLocation;
};

#endif // PACKER_H
