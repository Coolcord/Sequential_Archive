#include "Reader.h"
#include "../Sequential_Archive_Manager/Common_Strings.h"
#include "Scrambler.h"
#include "Common_Data.h"
#include <QDir>
#include <assert.h>

Reader::Reader(const QString &archiveLocation) {
    this->file = new QFile(archiveLocation);
    this->scrambler = nullptr;
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
    qint64 rootTableOffset = this->Read_qint64(Common_Strings::STRING_SEQUENTIAL_ARCHIVE.length());
    if (rootTableOffset == -1) return QString();
    qint64 nameOffset = Common_Strings::STRING_SEQUENTIAL_ARCHIVE.length()+16;

    //Now, using the starting offset, read the name
    return QString(this->Read_Bytes(nameOffset, rootTableOffset-nameOffset));
}

unsigned char Reader::Get_Archive_Scramble_Key() {
    //Read the scramble key if it wasn't specified
    if (!this->scrambler) {
        unsigned char scrambleKey = 0x00;
        if (!this->Read_Scramble_Key(scrambleKey)) return false;
        this->scrambler = new Scrambler(scrambleKey);
    }
    return this->scrambler->Get_Scramble_Key();
}

bool Reader::Change_Directory(const QString &directory) {
    if (directory.contains('/')) {
        if (directory.startsWith('/') && !this->Change_To_Root_Directory()) return false;
        foreach (QString dir, directory.split('/')) {
            if (!this->Change_Local_Directory(dir)) return false;
        }
        return true;
    } else if (directory == "..") {
        return this->Change_To_Parent_Directory();
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
    return this->Read_File(offset, size);
}

QByteArray Reader::Read_File(qint64 offset, qint64 size) {
    return qUncompress(this->Read_Bytes(offset, size));
}

bool Reader::Extract_File(const QString &filePathInArchive, const QString &destination) {
    qint64 offset = 0;
    qint64 size = 0;
    if (!this->Get_File_Offset_And_Size(filePathInArchive, offset, size)) return false;
    if (size > Common_Data::MAX_BUFFER_SIZE) {
        return this->Extract_File_With_Buffer(offset, size, destination);
    } else {
        QByteArray buffer = this->Read_File(offset, size);
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
    for (qint64 bytesRemaining = size; bytesRemaining > 0;) {
        //Determine how many bytes to read
        qint64 bytesToRead = 0;
        if (bytesRemaining > Common_Data::MAX_BUFFER_SIZE) {
            bytesToRead = Common_Data::MAX_BUFFER_SIZE;
        } else {
            bytesToRead = bytesRemaining;
        }

        //Read the bytes into a buffer and write them to disk
        QByteArray buffer = this->Read_File(offset, bytesToRead);
        offset += bytesToRead;
        bytesRemaining -= bytesToRead;
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
    if (!dir.exists() && !dir.mkdir(dir.path())) return false;

    //Extract all of the files first
    QStringList files = this->Get_Files(directoryPathInArchive);
    foreach (QString file, files) {
        if (!this->Extract_File(this->Get_New_Path(directoryPathInArchive, file), this->Get_New_Path(destination, file))) return false;
    }

    //Then extract all of the directories
    QStringList directories = this->Get_Directories(directoryPathInArchive);
    foreach (QString directory, directories) {
        if (!this->Extract_Directory(this->Get_New_Path(directoryPathInArchive, directory), this->Get_New_Path(destination, directory))) return false;
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
    QByteArray header = this->Read_Bytes(0, Common_Strings::STRING_SEQUENTIAL_ARCHIVE.length());
    return (header == Common_Strings::STRING_SEQUENTIAL_ARCHIVE.toUtf8()); //if the header matches up, then assume that the archive is valid
}

bool Reader::Change_Local_Directory(const QString &directory) {
    if (directory.isEmpty()) return true; //base case
    QByteArray nameBuffer = this->Read_Bytes(this->currentDirectoryOffset+8, this->Read_qint64(this->currentDirectoryOffset)-8);
    for (int i = 0; i < nameBuffer.size();) {
        int nameLength = this->Read_int(nameBuffer, i);
        i += 4;
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

bool Reader::Change_To_Parent_Directory() {
    if (this->currentPath == "/") return true; //nothing to do
    return this->Change_Directory(QFileInfo(this->currentPath).path());
}

bool Reader::Change_To_Directory_Containing_File(const QString &filePathInArchive) {
    assert(filePathInArchive.contains('/'));
    QFileInfo filePathInfo(filePathInArchive);
    return this->Change_Directory(filePathInfo.path());
}

bool Reader::Change_To_Root_Directory() {
    qint64 rootOffset = this->Read_qint64(Common_Strings::STRING_SEQUENTIAL_ARCHIVE.length());
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
    if (!this->file->seek(this->file->size()-1)) return false;
    QByteArray scrambleKeyBuffer = this->file->read(1);
    if (scrambleKeyBuffer.isEmpty()) return false;

    //Save the scramble key to memory
    scrambleKey = static_cast<unsigned char>(scrambleKeyBuffer.data()[0]);
    return true;
}

QStringList Reader::Read_Index_Table_Names(qint64 offset, bool fileMode) {
    QByteArray nameBuffer = this->Read_Bytes(offset+8, this->Read_qint64(offset)-8);
    QStringList nameList;
    for (int i = 0; i < nameBuffer.size();) {
        int nameLength = this->Read_int(nameBuffer, i);
        i += 4;
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
    qint64 number = static_cast<int>(static_cast<unsigned char>(buffer.data()[0]))*0x100000000000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[1]))*0x001000000000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[2]))*0x000010000000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[3]))*0x000000100000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[4]))*0x000000001000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[5]))*0x000000000010000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[6]))*0x000000000000100;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[7]));
    return number;
}

qint64 Reader::Read_qint64(const QByteArray &buffer, int offset) {
    if (buffer.size() < offset+8) return false;
    qint64 number = static_cast<int>(static_cast<unsigned char>(buffer.data()[offset]))*0x100000000000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+1]))*0x001000000000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+2]))*0x000010000000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+3]))*0x000000100000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+4]))*0x000000001000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+5]))*0x000000000010000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+6]))*0x000000000000100;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+7]));
    return number;
}

int Reader::Read_int(qint64 offset) {
    QByteArray buffer = this->Read_Bytes(offset, 4);
    if (buffer.isEmpty()) return -1; //we're only using positive numbers (signed to make the libraries happy), so return -1 on error
    int number = static_cast<int>(static_cast<unsigned char>(buffer.data()[0]))*0x1000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[1]))*0x0010000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[2]))*0x0000100;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[3]));
    return number;
}

int Reader::Read_int(const QByteArray &buffer, qint64 offset) {
    if (buffer.size() < offset+4) return false;
    int number = static_cast<int>(static_cast<unsigned char>(buffer.data()[offset]))*0x1000000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+1]))*0x0010000;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+2]))*0x0000100;
    number += static_cast<int>(static_cast<unsigned char>(buffer.data()[offset+3]));
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
    QByteArray nameBuffer = this->Read_Bytes(fileIndexOffset+8, this->Read_qint64(fileIndexOffset)-8);
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

QString Reader::Get_New_Path(const QString &currentPath, const QString &nextDirectory) {
    QString newPath = currentPath;
    if (!currentPath.endsWith('/')) newPath += "/";
    newPath += nextDirectory;
    return newPath;
}
