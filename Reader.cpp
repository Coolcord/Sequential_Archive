#include "Reader.h"
#include "Scrambler.h"
#include "Common_Strings.h"
#include "Common_Data.h"
#include <QDir>
#include <assert.h>

Reader::Reader(const QString &archiveLocation) {
    this->file = new QFile(archiveLocation);
    this->scrambler = NULL;
}

Reader::Reader(const QString &archiveLocation, unsigned char scrambleKey) {
    this->file = new QFile(archiveLocation);
    this->scrambler = new Scrambler(scrambleKey);
}

Reader::~Reader() {
    delete this->file;
    delete this->scrambler;
}

bool Reader::Open() {
    if (!this->file->open(QFile::ReadOnly)) return false;
    if (!this->Is_Archive_Valid()) return false;
    if (!this->Change_Directory("/")) return false;
    return true;
}

void Reader::Close() {
    this->file->close();
}

QString Reader::Get_Archive_Name() {
    //Read the starting offset of the root table
    qint64 rootTableOffset = this->Read_qint64(Common_Strings::FORMAT_NAME.length());
    if (rootTableOffset == -1) return QString();
    qint64 nameOffset = Common_Strings::FORMAT_NAME.length()+16;

    //Now, using the starting offset, read the name
    return QString(this->Read_Bytes(nameOffset, rootTableOffset-nameOffset));
}

bool Reader::Change_Directory(const QString &directory) {
    if (directory.contains('/')) {
        foreach (QString dir, directory.split('/')) {
            if (!this->Change_Local_Directory(dir)) return false;
        }
        return true;
    } else {
        return this->Change_Local_Directory(directory);
    }
}

QStringList Reader::Get_Directories() {
    return this->Read_Index_Table_Names(this->currentDirectoryOffset, false);
}

QStringList Reader::Get_Directories(const QString &pathInArchive) {
    QString previousPath = this->currentPath;
    qint64 previousOffset = this->currentDirectoryOffset;
    if (!this->Change_Directory(pathInArchive)) return QStringList();
    QStringList files = this->Get_Directories();
    this->currentPath = previousPath;
    this->currentDirectoryOffset = previousOffset;
    return files;
}

QStringList Reader::Get_Files() {
    return this->Read_Index_Table_Names(this->Read_qint64(this->currentDirectoryOffset)+this->currentDirectoryOffset, true);
}

QStringList Reader::Get_Files(const QString &pathInArchive) {
    QString previousPath = this->currentPath;
    qint64 previousOffset = this->currentDirectoryOffset;
    if (!this->Change_Directory(pathInArchive)) return QStringList();
    QStringList files = this->Get_Files();
    this->currentPath = previousPath;
    this->currentDirectoryOffset = previousOffset;
    return files;
}

QByteArray Reader::Read_File(const QString &filePathInArchive) {
    //TODO: Write this...
}

bool Reader::Extract_File(const QString &filePathInArchive, const QString &destination) {
    //TODO: Write this...
}


bool Reader::Extract_File_With_Buffer(const QString &filePathInArchive, const QString &destination) {
    //TODO: Write this...
}

bool Reader::Extract_Directory(const QString &directoryPathInArchive, const QString &destination) {
    //Create a folder to extract the files into
    QDir dir(destination);
    if (!dir.exists() && !dir.mkdir(destination + "/" + (this->Get_File_Name_From_Path(directoryPathInArchive)))) return false;

    //Extract all of the files first
    QStringList files = this->Get_Files(directoryPathInArchive);
    foreach (QString file, files) {
        if (!this->Extract_File(directoryPathInArchive + "/" + file, destination + "/" + file)) return false;
    }

    //Then extract all of the directories
    QStringList directories = this->Get_Directories(directoryPathInArchive);
    foreach (QString directory, directories) {
        if (!this->Extract_Directory(directoryPathInArchive + "/" + directory, destination + "/" + directory)) return false;
    }

    return true;
}

bool Reader::Is_Archive_Valid() {
    //Read the header to determine if the archive is valid
    if (!this->file->seek(0)) return false;
    QByteArray headerBuffer = this->file->read(Common_Strings::FORMAT_NAME.length());
    QByteArray headerEndByteBuffer = this->file->read(1);
    if (headerBuffer.isEmpty() || headerEndByteBuffer.isEmpty()) return false; //unable to read the header

    //Check to see if the archive has been scrambled
    if (QString(headerBuffer) != Common_Strings::FORMAT_NAME) {
        unsigned char scrambleKey = 0x00;
        if (!this->Read_Scramble_Key(scrambleKey)) return false;
        this->scrambler = new Scrambler(scrambleKey);
    }

    //Check to see if the end byte is in the header
    if (this->scrambler) this->scrambler->Unscramble(headerEndByteBuffer, Common_Strings::FORMAT_NAME.length());

    return true; //the archive appears to be a valid sequential archive!
}

bool Reader::Change_Local_Directory(const QString &directory) {
    QByteArray nameBuffer = this->Read_Bytes(this->currentDirectoryOffset+8, this->Read_qint64(this->currentDirectoryOffset));
    for (int i = 0; i < nameBuffer.size();) {
        int nameLength = this->Read_int(nameBuffer, i);
        QString name = "";
        for (int j = 0; j < nameLength; ++j) {
            name += nameBuffer.at(i+j);
        }
        i += nameLength;

        //If this is the directory in question, change to it!
        if (name == directory) {
            this->currentDirectoryOffset = this->Read_qint64(nameBuffer, i);
            this->currentPath += "/" + directory;
            return true;
        }
        i += 8;
    }
    return false;
}

bool Reader::Read_Scramble_Key(unsigned char &scrambleKey) {
    assert(this->file);
    assert(this->file->isOpen() && this->file->isReadable());
    assert(!this->scrambler);

    //Read the key from the end of the file
    if (!this->file->seek(this->file->size()-2)) return false;
    QByteArray scrambleKeyBuffer = this->file->read(1);
    if (scrambleKeyBuffer.isEmpty()) return false;

    //Save the scramble key to memory
    scrambleKey = static_cast<unsigned char>(scrambleKeyBuffer.data()[0]);
    return true;
}

QStringList Reader::Read_Index_Table_Names(qint64 offset, bool fileMode) {
    QByteArray nameBuffer = this->Read_Bytes(offset+8, this->Read_qint64(offset));
    QStringList nameList;
    for (int i = 0; i < nameBuffer.size();) {
        int nameLength = this->Read_int(nameBuffer, i);
        QString name = "";
        for (int j = 0; j < nameLength; ++j) {
            name += nameBuffer.at(i+j);
        }
        nameList.append(name);
        i += nameLength + 8; //skip past the starting offset
        if (fileMode) i += 8; //files have a size as well, so skip that if necessary
    }
    return nameList;
}

qint64 Reader::Read_qint64(qint64 offset) {
    QByteArray buffer = this->Read_Bytes(offset, 8);
    if (buffer.isEmpty()) return -1; //we're only using positive numbers (signed to make the libraries happy), so return -1 on error
    qint64 number = 0;
    for (int i = 0; i < 8; ++i) {
        number += static_cast<int>(static_cast<unsigned char>(buffer.data()[i]));
    }
    return number;
}

qint64 Reader::Read_qint64(const QByteArray &buffer, int offset) {
    if (buffer.size() < offset+8) return false;
    int number = 0;
    for (int i = 0; i < 8; ++i) {
        number += static_cast<int>(static_cast<unsigned char>(buffer.data()[i]));
    }
    return number;
}

int Reader::Read_int(qint64 offset) {
    QByteArray buffer = this->Read_Bytes(offset, 4);
    if (buffer.isEmpty()) return -1; //we're only using positive numbers (signed to make the libraries happy), so return -1 on error
    int number = 0;
    for (int i = 0; i < 4; ++i) {
        number += static_cast<int>(static_cast<unsigned char>(buffer.data()[i]));
    }
    return number;
}

qint64 Reader::Read_int(const QByteArray &buffer, int offset) {
    if (buffer.size() < offset+4) return false;
    int number = 0;
    for (int i = 0; i < 4; ++i) {
        number += static_cast<int>(static_cast<unsigned char>(buffer.data()[i]));
    }
    return number;
}

QString Reader::Get_File_Name_From_Path(const QString &path) {
    return path.split('/').last();
}

QByteArray Reader::Read_Bytes(qint64 offset, qint64 size) {
    assert(this->file);
    assert(this->file->isOpen() && this->file->isReadable());
    if (!this->file->seek(offset)) return QByteArray();

    //Read the bytes from the archive
    QByteArray buffer = this->file->read(size);
    if (buffer.isEmpty()) return QByteArray();

    //Unscramble the bytes if necessary
    if (this->scrambler) this->scrambler->Unscramble(buffer, offset);
    return buffer;
}
