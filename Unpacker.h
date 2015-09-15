#ifndef UNPACKER_H
#define UNPACKER_H

#include <QFile>
#include <QString>

class Unpacker
{
public:
    Unpacker();
    ~Unpacker();
    int Unpack(const QString &sourceArchive, const QString &destinationFolder);

private:
    QFile *file;
};

#endif // UNPACKER_H
