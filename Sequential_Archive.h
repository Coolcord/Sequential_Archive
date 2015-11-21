#ifndef SEQUENTIAL_ARCHIVE_H
#define SEQUENTIAL_ARCHIVE_H

#include "Sequential_Archive_Interface.h"
#include <QFile>
#include <QTextStream>
#include <QString>

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

private:
    QString applicationLocation;
    QWidget *parent;
};

#endif // SEQUENTIAL_ARCHIVE_H

