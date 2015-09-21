#include "Reader.h"
#include "Scrambler.h"
#include "Common_Strings.h"
#include "Common_Data.h"
#include <QLinkedList>
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

    //TODO: Write this...
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

    //TODO: Write this...
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
    if (!this->file->seek(this->file->size()-1)) return false;
    QByteArray scrambleKeyBuffer = this->file->read(1);
    if (scrambleKeyBuffer.isEmpty()) return false;

    //Save the scramble key to memory
    scrambleKey = static_cast<unsigned char>(scrambleKeyBuffer.data()[0]);
    return true;
}

//Avoid using this function unless absolutely necessary!
//I may remove this later if it is not needed
QByteArray Reader::Read_Until_End_Byte(qint64 offset) {
    if (!this->file->seek(offset)) return false;

    //We are using a linked list here as its append is constant time
    //QByteArray's append is linear time, which we want to avoid
    QLinkedList byteList<char>();

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

    //Copy the unscrambled data into a proper byte array
    QByteArray buffer(byteList.size(), ' ');
    qint64 i = 0;
    foreach (char c, byteList) {
        buffer.data()[i] = c;
        ++i;
    }
    assert(i == buffer.size()-1);
    return buffer;
}

