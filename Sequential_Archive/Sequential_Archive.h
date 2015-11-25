#ifndef SEQUENTIAL_ARCHIVE_H
#define SEQUENTIAL_ARCHIVE_H

#include "Sequential_Archive_Interface.h"
#include <QFile>
#include <QTextStream>
#include <QString>

class Reader;

class Sequential_Archive : public Sequential_Archive_Interface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID  "Sequential_Archive_Interface")
    Q_INTERFACES(Sequential_Archive_Interface)

public:
    Sequential_Archive();
    void Startup(QWidget *parent, const QString &applicationLocation);
    void Shutdown();

    //Packing and Unpacking Functions
    int Pack(const QString &sourceFolder);
    int Pack(const QString &sourceFolder, const QString &destinationArchive);
    int Pack(const QString &sourceFolder, unsigned char scrambleKey);
    int Pack(const QString &sourceFolder, const QString &destinationArchive, unsigned char scrambleKey);
    int Unpack(const QString &sourceArchive);
    int Unpack(const QString &sourceArchive, const QString &destinationFolder);
    int Unpack(const QString &sourceArchive, unsigned char scrambleKey);
    int Unpack(const QString &sourceArchive, const QString &destinationFolder, unsigned char scrambleKey);

    //Reader Functions
    bool Open(const QString &archive);
    bool Open(const QString &archive, unsigned char scrambleKey);
    void Close();
    QString Get_Archive_Name();
    unsigned char Get_Archive_Scramble_Key();
    bool Change_Directory(const QString &directory);
    QStringList Get_Directories();
    QStringList Get_Directories(const QString &pathInArchive);
    QStringList Get_Files();
    QStringList Get_Files(const QString &pathInArchive);
    QByteArray Read_File(const QString &filePathInArchive);
    bool Extract_File(const QString &filePathInArchive, const QString &destination);
    bool Extract_Directory(const QString &directoryPathInArchive, const QString &destination);

private:
    QString Get_Archive_Name_From_Source_Folder(const QString &sourceFolder);
    QString Get_Folder_From_Archive_Path(const QString &archivePath);

    QString applicationLocation;
    QWidget *parent;
    Reader *reader;
};

#endif // SEQUENTIAL_ARCHIVE_H

