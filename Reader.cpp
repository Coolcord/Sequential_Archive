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
    return this->file->open(QFile::ReadOnly);
}

void Reader::Close() {
    this->file->close();
}

QString Reader::Get_Archive_Name() {
    assert(this->file);
    assert(this->file->isOpen() && this->file->isReadable());

    //Read the starting offset of the root table
    if (!this->file->seek(Common_Strings::FORMAT_NAME.length())) return false;
    //TODO: Write this...
    //this->Read_Bytes(Common_Strings::FORMAT_NAME.length(), 8);
}

QStringList Reader::Get_Directories(const QString &pathInArchive) {
    assert(this->file);
    assert(this->file->isOpen() && this->file->isReadable());

    //TODO: Write this...
}

QStringList Reader::Get_Files(const QString &pathInArchive) {
    assert(this->file);
    assert(this->file->isOpen() && this->file->isReadable());

    //TODO: Write this...
}

QByteArray Reader::Read_File(const QString &filePathInArchive) {
    assert(this->file);
    assert(this->file->isOpen() && this->file->isReadable());

    //TODO: Write this...
}

bool Reader::Extract_File(const QString &filePathInArchive, const QString &destination) {
    assert(this->file);
    assert(this->file->isOpen() && this->file->isReadable());

    //TODO: Write this...
}

bool Reader::Extract_Directory(const QString &directoryPathInArchive, const QString &destination) {
    assert(this->file);
    assert(this->file->isOpen() && this->file->isReadable());

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

bool Reader::Extract_File_With_Buffer(const QString &filePathInArchive, const QString &destination) {

}

bool Reader::Is_Archive_Valid() {
    assert(this->file);
    assert(this->file->isOpen() && this->file->isReadable());

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
    if (static_cast<unsigned char>(headerEndByteBuffer.data()[0]) != Common_Data::END_SECTION) return false;

    return true; //the archive appears to be a valid sequential archive!
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

QString Reader::Get_File_Name_From_Path(const QString &path) {
    return path.split('/').last();
}

QByteArray Reader::Read_Bytes(qint64 offset, qint64 size) {
    if (!this->file->seek(offset)) return QByteArray();

    //Read the bytes from the archive
    QByteArray buffer = this->file->read(size);
    if (buffer.isEmpty()) return QByteArray();

    //Unscramble the bytes if necessary
    if (this->scrambler) this->scrambler->Unscramble(buffer, offset);
    return buffer;
}

//Avoid using this function unless absolutely necessary!
//I may remove this later if it is not needed
QByteArray Reader::Read_Until_End_Byte(qint64 offset) {
    if (!this->file->seek(offset)) return QByteArray();

    //We are using a QString here as its append is constant time
    //QByteArray's append is linear time, which we want to avoid
    QString byteList = "";

    //Read into the byteList until either the end byte is found or we are at the end of the file
    while (true) {
        if (this->file->atEnd()) break;
        QByteArray buffer = this->file->read(1);
        if (buffer.isEmpty()) break;
        if (this->scrambler) this->scrambler->Unscramble(buffer, offset);
        if (static_cast<unsigned char>(buffer.data()[0]) == Common_Data::END_SECTION) break;
        byteList.append(buffer.data()[0]);
        ++offset;
    }

    return QByteArray(byteList.toUtf8().data());
}

