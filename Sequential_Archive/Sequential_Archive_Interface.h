#ifndef SEQUENTIAL_ARCHIVE_INTERFACE_H
#define SEQUENTIAL_ARCHIVE_INTERFACE_H

#include <QObject>
#include <QtPlugin>
#include <QString>

class Sequential_Archive_Interface : public QObject {
public:
    virtual void Startup(QWidget *parent, const QString &applicationLocation)=0;
    virtual void Shutdown()=0;

    //Packing and Unpacking Functions
    virtual int Pack(const QString &sourceFolder)=0;
    virtual int Pack(const QString &sourceFolder, const QString &destinationArchive)=0;
    virtual int Pack(const QString &sourceFolder, unsigned char scrambleKey)=0;
    virtual int Pack(const QString &sourceFolder, const QString &destinationArchive, unsigned char scrambleKey)=0;
    virtual int Unpack(const QString &sourceArchive)=0;
    virtual int Unpack(const QString &sourceArchive, const QString &destinationFolder)=0;
    virtual int Unpack(const QString &sourceArchive, unsigned char scrambleKey)=0;
    virtual int Unpack(const QString &sourceArchive, const QString &destinationFolder, unsigned char scrambleKey)=0;

    //Reader Functions
    virtual bool Open(const QString &archive)=0;
    virtual bool Open(const QString &archive, unsigned char scrambleKey)=0;
    virtual bool Is_Open()=0;
    virtual void Close()=0;
    virtual QString Get_Archive_Name()=0;
    virtual unsigned char Get_Archive_Scramble_Key()=0;
    virtual bool Change_Directory(const QString &directory)=0;
    virtual QStringList Get_Directories()=0;
    virtual QStringList Get_Directories(const QString &pathInArchive)=0;
    virtual QStringList Get_Files()=0;
    virtual QStringList Get_Files(const QString &pathInArchive)=0;
    virtual QByteArray Read_File(const QString &filePathInArchive)=0;
    virtual bool Extract_File(const QString &filePathInArchive, const QString &destination)=0;
    virtual bool Extract_Directory(const QString &directoryPathInArchive, const QString &destination)=0;
};

Q_DECLARE_INTERFACE(Sequential_Archive_Interface, "Sequential_Archive_Interface")

#endif // SEQUENTIAL_ARCHIVE_INTERFACE_H
