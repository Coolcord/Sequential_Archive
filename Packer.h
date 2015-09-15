#ifndef PACKER_H
#define PACKER_H

#include <QFile>
#include <QString>

class Packer
{
public:
    Packer();
    ~Packer();
    int Pack(const QString &sourceFolder, const QString &destinationArchive);

private:
    bool Write_Header(QFile *file, const QString &name);

    QFile *file;
};

#endif // PACKER_H
