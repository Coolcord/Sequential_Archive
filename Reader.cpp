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
    if (!this->Change_To_Root_Directory()) return false;
    return true;
}

void Reader::Close() {
    if (this->file) this->file->close();
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
        if (directory.startsWith('/') && !this->Change_To_Root_Directory()) return false;
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
    qint64 offset = 0;
    qint64 size = 0;
    if (!this->Get_File_Offset_And_Size(filePathInArchive, offset, size)) return QByteArray();
    return this->Read_Bytes(offset, size);
}

bool Reader::Extract_File(const QString &filePathInArchive, const QString &destination) {
    qint64 offset = 0;
    qint64 size = 0;
    if (!this->Get_File_Offset_And_Size(filePathInArchive, offset, size)) return false;
    if (offset-size > Common_Data::MAX_BUFFER_SIZE) {
        return this->Extract_File_With_Buffer(offset, size, destination);
    } else {
        QByteArray buffer = this->Read_Bytes(offset, size);
        if (buffer.isEmpty()) return false;
        QFile destinationFile(destination);
        if (!destinationFile.open(QFile::ReadWrite | QFile::Truncate)) return false;
        if (destinationFile.write(buffer) != buffer.size()) return false;
        destinationFile.close();
        return true;
    }
}


bool Reader::Extract_File_With_Buffer(qint64 offset, qint64 size, const QString &destination) {
    QFile destinationFile(destination);
    if (!destinationFile.open(QFile::ReadWrite | QFile::Truncate)) return false;
    for (qint64 bytesRead = 0; bytesRead <= size;) {
        //Determine how many bytes to read
        qint64 bytesRemaining = size - bytesRead;
        qint64 bytesToRead = 0;
        if (bytesRemaining > Common_Data::MAX_BUFFER_SIZE) {
            bytesToRead = Common_Data::MAX_BUFFER_SIZE;
        } else {
            bytesToRead = bytesRemaining;
        }

        //Read the bytes into a buffer and write them to disk
        QByteArray buffer = this->Read_Bytes(offset+bytesRead, bytesToRead);
        if (buffer.isEmpty()) return false;
        if (destinationFile.write(buffer) != buffer.size()) {
            destinationFile.remove(); //delete what was written as it is invalid
            return false;
        }
    }
    destinationFile.close();
    return true;
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
    //Read the scramble key if it wasn't specified
    if (!this->scrambler) {
        unsigned char scrambleKey = 0x00;
        if (!this->Read_Scramble_Key(scrambleKey)) return false;
        this->scrambler = new Scrambler(scrambleKey);
    }

    //Read the header to determine if the archive is valid
    QByteArray header = this->Read_Bytes(0, Common_Strings::FORMAT_NAME.length());
    return (header == Common_Strings::FORMAT_NAME.toUtf8()); //if the header matches up, then assume that the archive is valid
}

bool Reader::Change_Local_Directory(const QString &directory) {
    if (directory.isEmpty()) return true; //base case
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
            if (this->currentPath == "/") {
                this->currentPath += directory;
            } else {
                this->currentPath += "/" + directory;
            }
            return true;
        }
        i += 8;
    }
    return false;
}

bool Reader::Change_To_Directory_Containing_File(const QString &filePathInArchive) {
    assert(filePathInArchive.contains('/'));
    QFileInfo filePathInfo(filePathInArchive);
    return this->Change_Directory(filePathInfo.path());
}

bool Reader::Change_To_Root_Directory() {
    qint64 rootOffset = this->Read_qint64(Common_Strings::FORMAT_NAME.length());
    if (rootOffset == -1) {
        return false;
    } else {
        this->currentDirectoryOffset = rootOffset;
        this->currentPath = "/";
        return true;
    }
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
        number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+i]));
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
        number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+i]));
    }
    return number;
}

QString Reader::Get_File_Name_From_Path(const QString &fileNameWithPath) {
    return fileNameWithPath.split('/').last();
}

bool Reader::Get_File_Offset_And_Size(const QString &filePathInArchive, qint64 &offset, qint64 &size) {
    //Change to the folder in question first if specified
    if (filePathInArchive.endsWith("/")) return false;
    QString fileName = this->Get_File_Name_From_Path(filePathInArchive);
    if (filePathInArchive.contains('/')) {
        if (!this->Change_To_Directory_Containing_File(filePathInArchive)) return false;
    }

    //Scan through the file index to get the offset of the file in question
    qint64 fileIndexOffset = this->Read_qint64(this->currentDirectoryOffset) + this->currentDirectoryOffset;
    QByteArray nameBuffer = this->Read_Bytes(fileIndexOffset+8, this->Read_qint64(fileIndexOffset));
    for (int i = 0; i < nameBuffer.size();) {
        int nameLength = this->Read_int(nameBuffer, i);
        i += 4;
        QString name = "";
        for (int j = 0; j < nameLength; ++j) {
            name += nameBuffer.at(i+j);
        }
        i += nameLength;

        //If this is the file in question, read it into a buffer!
        if (name == fileName) {
            offset = this->Read_qint64(nameBuffer, i);
            size = this->Read_qint64(nameBuffer, i+8);
            return true;
        }
        i += 16;
    }
    return false;
}

QByteArray Reader::Read_Bytes(qint64 offset, qint64 size) {
    assert(this->file);
    assert(this->file->isOpen() && this->file->isReadable());
    assert(this->file->size() >= offset+size);
    if (!this->file->seek(offset)) return QByteArray();

    //Read the bytes from the archive
    QByteArray buffer = this->file->read(size);
    if (buffer.isEmpty()) return QByteArray();

    //Unscramble the bytes if necessary
    if (this->scrambler) this->scrambler->Unscramble(buffer, offset);
    return buffer;
}
