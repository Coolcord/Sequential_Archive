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
    QStringList Get_Directories(const QString &pathInArchive);
    QStringList Get_Files(const QString &pathInArchive);
    QByteArray Read_File(const QString &filePathInArchive);
    bool Extract_File(const QString &filePathInArchive, const QString &destination);
    bool Extract_Directory(const QString &directoryPathInArchive, const QString &destination);

private:
    bool Is_Archive_Valid();
    bool Read_Scramble_Key(unsigned char &scrambleKey);
    QByteArray Read_Until_End_Byte(qint64 offset); //try to avoid using this function!

    QFile *file;
    Scrambler *scrambler;
};

#endif // READER_H
