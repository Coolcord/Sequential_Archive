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
    bool Write_Archive_Header(QFile &file, const QFileInfo &sourceFolderInfo);
    bool Pack_Directory(QFile &file, const QString &sourceFolderLocation);
    bool Pack_File(QFile &file, const QString &sourceFileLocation);
    bool Pack_File_With_Buffers(QFile &file, QFile &sourceFile, qint64 fileSize);
    int Get_Index_Table_Size(const QString &sourceFolderLocation);
    bool Write_Buffer_To_File(QFile &file, QByteArray &buffer);
    bool Write_Buffer_To_File(QFile &file, QByteArray &buffer, qint64 offset);
    bool Write_Buffer_To_File(QFile &file, const QString &buffer);
    bool Write_Buffer_To_File(QFile &file, const QString &buffer, qint64 offset);

    Scrambler *scrambler;
    QString destinationArchiveLocation;
};

#endif // PACKER_H
