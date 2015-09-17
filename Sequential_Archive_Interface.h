#ifndef SEQUENTIAL_ARCHIVE_INTERFACE_H
#define SEQUENTIAL_ARCHIVE_INTERFACE_H

#include <QObject>
#include <QtPlugin>
#include <QString>

class Sequential_Archive_Interface : public QObject {
public:
    virtual void Startup(QWidget *parent, const QString &applicationLocation)=0;
    virtual void Shutdown()=0;
    virtual int Pack(const QString &sourceFolder)=0;
    virtual int Pack(const QString &sourceFolder, const QString &destinationArchive)=0;
    virtual int PackAndScramble(const QString &sourceFolder)=0;
    virtual int PackAndScramble(const QString &sourceFolder, const QString &destinationArchive)=0;
    virtual int Unpack(const QString &sourceArchive)=0;
    virtual int Unpack(const QString &sourceArchive, const QString &destinationFolder)=0;
    virtual int UnscrambleAndUnpack(const QString &sourceArchive)=0;
    virtual int UnscrambleAndUnpack(const QString &sourceArchive, const QString &destinationFolder)=0;
};

Q_DECLARE_INTERFACE(Sequential_Archive_Interface, "Sequential_Archive_Interface")

#endif // SEQUENTIAL_ARCHIVE_INTERFACE_H
