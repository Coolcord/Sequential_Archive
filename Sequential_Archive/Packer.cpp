#include "Packer.h"
#include "../Sequential_Archive_Manager/Common_Strings.h"
#include "Scrambler.h"
#include "Common_Data.h"
#include <QDir>
#include <QTextStream>
#include <QHash>
#include <assert.h>

Packer::Packer() {
    this->scrambler = new Scrambler(static_cast<unsigned char>(rand()%0x100));
}

Packer::Packer(unsigned char scrambleKey) {
    this->scrambler = new Scrambler(scrambleKey);
}

Packer::~Packer() {
    delete this->scrambler;
}

int Packer::Pack(const QString &sourceFolderLocation, const QString &destinationArchiveLocation) {
    //Open the source folder and create the destination archive file
    QFileInfo folderInfo(sourceFolderLocation);
    if (!folderInfo.exists() || !folderInfo.isReadable()) return 1; //unable to read the source folder
    if (!folderInfo.isDir()) return 2; //source folder is not a directory

    //Open the source folder and make sure it isn't empty
    QDir dir(sourceFolderLocation);
    if (dir.count() == 0) return 3; //directory is empty

    //Create the archive file and write the header
    QFile file(destinationArchiveLocation);
    if (!file.open(QFile::ReadWrite | QFile::Truncate)) return 3; //unable to create the archive file
    if (!this->Write_Archive_Header(file, folderInfo)) return 4; //unable to write the archive file header

    //Pack all of the files
    if (!this->Pack_Directory(file, sourceFolderLocation)) return 5; //unable to pack the directory

    //Write the scramble key to the end of the file if it was used
    if (!file.seek(file.size())) return 6; //unable to write scramble key
    assert(this->scrambler);
    QByteArray buffer(1, 0x00);
    buffer.data()[0] = static_cast<char>(this->scrambler->Get_Scramble_Key());
    if (file.write(buffer) != buffer.size()) return 6; //unable to write scramble key
    return 0;
}

bool Packer::Write_Archive_Header(QFile &file, const QFileInfo &sourceFolderInfo) {
    //Sequential Archive
    //Root Index Offset
    //File Name
    QByteArray header = Common_Strings::STRING_SEQUENTIAL_ARCHIVE.toUtf8() +
            this->Get_Byte_Array_From_qint64(Common_Strings::STRING_SEQUENTIAL_ARCHIVE.length()+8+sourceFolderInfo.fileName().length()) +
            sourceFolderInfo.fileName().toUtf8();
    return this->Write_Buffer_To_File(file, header, 0);
}

bool Packer::Pack_Directory(QFile &file, const QString &sourceFolderLocation) {
    QDir dir(sourceFolderLocation);
    assert(dir.count() > 0); //empty folders should never enter this function

    //Allocate space for the length of the directory names
    qint64 startingOffset = file.size();
    if (!this->Write_Buffer_To_File(file, QByteArray(8, 0x00))) return false;

    //Create a table entry for each directory
    QFileInfoList directories = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QHash<QString, qint64> directoryTable;
    foreach (QFileInfo sourceDirectory, directories) {
        if (sourceDirectory.dir().count() == 0) continue; //we won't pack empty folders

        //Write the length of the directory name and the directory name itself
        if (!this->Write_Buffer_To_File(file, this->Get_Byte_Array_From_int(sourceDirectory.fileName().size()))) return false;
        if (!this->Write_Buffer_To_File(file, sourceDirectory.fileName().toUtf8())) return false;

        //Insert the directory into the hash to update it later
        directoryTable.insert(sourceDirectory.fileName(), file.size());

        //Allocate 8 bytes for the start pointer and size value
        if (!this->Write_Buffer_To_File(file, QByteArray(8, 0x00))) return false;
    }
    if (!this->Write_Buffer_To_File(file, this->Get_Byte_Array_From_qint64(file.size()-startingOffset), startingOffset)) return false;

    //Allocate space for the length of the file names
    startingOffset = file.size();
    if (!this->Write_Buffer_To_File(file, QByteArray(8, 0x00))) return false;

    //Create a table entry for each file
    QFileInfoList files = dir.entryInfoList(QDir::Files);
    QHash<QString, qint64> fileTable;
    foreach (QFileInfo sourceFile, files) {
        //Write the length of the filename and the filename itself
        if (!this->Write_Buffer_To_File(file, this->Get_Byte_Array_From_int(sourceFile.fileName().size()))) return false;
        if (!this->Write_Buffer_To_File(file, sourceFile.fileName().toUtf8())) return false;

        //Insert the file into the hash to update it later
        fileTable.insert(sourceFile.fileName(), file.size());

        //Allocate 16 bytes for the start pointer and size value
        if (!this->Write_Buffer_To_File(file, QByteArray(16, 0x00))) return false;
    }
    if (!this->Write_Buffer_To_File(file, this->Get_Byte_Array_From_qint64(file.size()-startingOffset), startingOffset)) return false;

    //Pack each file. Update the table entry for each file along the way
    foreach (QFileInfo sourceFile, files) {
        qint64 startByte = file.size();
        qint64 sourceFileCompressedSize = 0;
        if (!this->Pack_File(file, sourceFile.filePath(), sourceFileCompressedSize)) return false;

        //Update the table entry with the new pointers
        if (!this->Write_Buffer_To_File(file, this->Get_Byte_Array_From_qint64(startByte)+this->Get_Byte_Array_From_qint64(sourceFileCompressedSize), fileTable.value(sourceFile.fileName()))) return false;
    }

    //Pack every subdirectory
    foreach (QFileInfo sourceDirectory, directories) {
        if (sourceDirectory.dir().count() == 0) continue; //ignore empty folders

        qint64 startByte = file.size();
        if (!this->Pack_Directory(file, sourceDirectory.filePath())) return false;

        //Update the table entry with the new pointers
        if (!this->Write_Buffer_To_File(file, this->Get_Byte_Array_From_qint64(startByte), directoryTable.value(sourceDirectory.fileName()))) return false;
    }

    return true;
}

bool Packer::Pack_File(QFile &file, const QString &sourceFileLocation, qint64 &compressedSize) {
    QFile sourceFile(sourceFileLocation);
    if (!sourceFile.open(QFile::ReadOnly)) return false;

    //Determine if the entire file can fit into one buffer
    qint64 fileSize = sourceFile.size();
    if (fileSize > Common_Data::MAX_BUFFER_SIZE) {
        return this->Pack_File_With_Buffers(file, sourceFile, fileSize, compressedSize);
    } else {
        return this->Compress_And_Write_Buffer_To_File(file, sourceFile.readAll(), compressedSize);
    }
}

bool Packer::Pack_File_With_Buffers(QFile &file, QFile &sourceFile, qint64 fileSize, qint64 &compressedSize) {
    while (fileSize > 0) {
        qint64 bufferSize = Common_Data::MAX_BUFFER_SIZE;
        if (fileSize < Common_Data::MAX_BUFFER_SIZE) bufferSize = fileSize;
        if (!this->Compress_And_Write_Buffer_To_File(file, sourceFile.read(bufferSize), compressedSize)) return false;
        fileSize -= bufferSize;
    }
    return true;
}

QByteArray Packer::Get_Byte_Array_From_qint64(qint64 number) {
    QByteArray byteArray(8, 0x00);
    byteArray.data()[0] = static_cast<char>(static_cast<unsigned char>(((static_cast<quint64>(number)&0xFF00000000000000)>>56)&0xFF));
    byteArray.data()[1] = static_cast<char>(static_cast<unsigned char>((number&0x00FF000000000000)>>48));
    byteArray.data()[2] = static_cast<char>(static_cast<unsigned char>((number&0x0000FF0000000000)>>40));
    byteArray.data()[3] = static_cast<char>(static_cast<unsigned char>((number&0x000000FF00000000)>>32));
    byteArray.data()[4] = static_cast<char>(static_cast<unsigned char>((number&0x00000000FF000000)>>24));
    byteArray.data()[5] = static_cast<char>(static_cast<unsigned char>((number&0x0000000000FF0000)>>16));
    byteArray.data()[6] = static_cast<char>(static_cast<unsigned char>((number&0x000000000000FF00)>>8));
    byteArray.data()[7] = static_cast<char>(static_cast<unsigned char>((number&0x00000000000000FF)));
    return byteArray;
}

QByteArray Packer::Get_Byte_Array_From_int(int number) {
    QByteArray byteArray(4, 0x00);
    byteArray.data()[0] = static_cast<char>(static_cast<unsigned char>(((static_cast<unsigned int>(number)&0xFF000000)>>24)&0xFF));
    byteArray.data()[1] = static_cast<char>(static_cast<unsigned char>((number&0x00FF0000)>>16));
    byteArray.data()[2] = static_cast<char>(static_cast<unsigned char>((number&0x0000FF00)>>8));
    byteArray.data()[3] = static_cast<char>(static_cast<unsigned char>((number&0x000000FF)));
    return byteArray;
}

bool Packer::Compress_And_Write_Buffer_To_File(QFile &file, QByteArray buffer, qint64 &compressedSize) {
    buffer = qCompress(buffer);
    compressedSize = buffer.size();
    return this->Write_Buffer_To_File(file, buffer);
}

bool Packer::Write_Buffer_To_File(QFile &file, QByteArray buffer) {
    return this->Write_Buffer_To_File(file, buffer, file.size());
}

bool Packer::Write_Buffer_To_File(QFile &file, QByteArray buffer, qint64 offset) {
    if (!file.seek(offset)) return false;
    assert(this->scrambler);
    this->scrambler->Scramble(buffer, offset);
    return (file.write(buffer) == buffer.size());
}
