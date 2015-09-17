#ifndef READER_H
#define READER_H

#include <QString>
#include <QFile>
#include <QByteArray>

class Reader
{
public:
    Reader(const QString &archiveLocation);
    ~Reader();
    QByteArray ReadFile(const QString &fileLocation);

private:
    QFile *file;
};

#endif // READER_H
