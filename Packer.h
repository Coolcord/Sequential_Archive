#ifndef PACKER_H
#define PACKER_H

#include <QFile>
#include <QFileInfo>
#include <QString>

class Packer
{
public:
    Packer();
    ~Packer();
    int Pack(const QString &sourceFolderLocation, const QString &destinationArchiveLocation);

private:
    bool Write_Archive_Header(const QFileInfo &sourceFolderInfo);
    bool Pack_Directory(const QString &sourceFolderLocation);
    bool Pack_File(const QString &sourceFileLocation);
    bool Pack_File_With_Buffers(QFile *sourceFile, qint64 fileSize);
    int Get_Index_Table_Size(const QString &sourceFolderLocation);

    QFile *file;
    QString destinationArchiveLocation;
};

#endif // PACKER_H
