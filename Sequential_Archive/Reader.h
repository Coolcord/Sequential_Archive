#ifndef READER_H
#define READER_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QByteArray>

class Scrambler;

class Reader
{
public:
    Reader(const QString &archiveLocation);
    Reader(const QString &archiveLocation, unsigned char scrambleKey);
    ~Reader();
    bool Open();
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
    bool Extract_File_With_Buffer(qint64 offset, qint64 size, const QString &destination);
    bool Is_Archive_Valid();
    bool Change_Local_Directory(const QString &directory);
    bool Change_To_Parent_Directory();
    bool Change_To_Directory_Containing_File(const QString &filePathInArchive);
    bool Change_To_Root_Directory();
    bool Read_Scramble_Key(unsigned char &scrambleKey);
    QStringList Read_Index_Table_Names(qint64 offset, bool fileMode);
    qint64 Read_qint64(qint64 offset);
    qint64 Read_qint64(const QByteArray &buffer, int offset);
    int Read_int(qint64 offset);
    qint64 Read_int(const QByteArray &buffer, int offset);
    QString Get_File_Name_From_Path(const QString &fileNameWithPath);
    bool Get_File_Offset_And_Size(const QString &filePathInArchive, qint64 &offset, qint64 &size);
    QByteArray Read_Bytes(qint64 offset, qint64 size);
    QString Get_New_Path(const QString &currentPath, const QString &nextDirectory);

    QFile *file;
    Scrambler *scrambler;
    QString currentPath;
    qint64 currentDirectoryOffset;
};

#endif // READER_H
