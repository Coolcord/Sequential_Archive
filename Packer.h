#ifndef PACKER_H
#define PACKER_H

#include <QFile>
#include <QString>

class Packer
{
public:
    Packer();
    ~Packer();
    int Pack(const QString &sourceFolderLocation, const QString &destinationArchiveLocation);

private:
    bool Write_Archive_Header(const QString &name);
    bool Write_Directory_Header(const QString &sourceFolderLocation);
    bool Write_File_Header(const QString &sourceFileLocation);

    bool Pack_Directories(const QString &sourceFolderLocation);
    bool Pack_Files(const QString &sourceFolderLocation);
    bool Pack_File(const QString &sourceFileLocation);
    bool Pack_File_With_Buffers(QFile *sourceFile);

    QFile *file;
    QString destinationArchiveLocation;
};

#endif // PACKER_H
