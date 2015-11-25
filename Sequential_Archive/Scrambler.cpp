#include "Scrambler.h"
#include <assert.h>

Scrambler::Scrambler(unsigned char scrambleKey) {
    this->scrambleKey = scrambleKey;
}

unsigned char Scrambler::Get_Scramble_Key() {
    return this->scrambleKey;
}

void Scrambler::Scramble(QByteArray &buffer, qint64 offsetInFile) {
    for (int i = 0; i < buffer.size(); ++i) {
        this->ScrambleByte(buffer, i, offsetInFile);
    }
}

void Scrambler::Unscramble(QByteArray &buffer, qint64 offsetInFile) {
    for (int i = 0; i < buffer.size(); ++i) {
        this->UnscrambleByte(buffer, i, offsetInFile);
    }
}

void Scrambler::ScrambleByte(QByteArray &buffer, int index, qint64 offsetInFile) {
    buffer.data()[index] = static_cast<char>((buffer.data()[index]+this->GetScrambleAmount(index, offsetInFile))%0x100);
}

void Scrambler::UnscrambleByte(QByteArray &buffer, int index, qint64 offsetInFile) {
    unsigned char decrementAmount = this->GetScrambleAmount(index, offsetInFile);
    unsigned char byte = static_cast<unsigned char>(buffer.data()[index]);
    byte -= decrementAmount; //the size of the unsigned char will handle wrapping for us
    buffer.data()[index] = static_cast<char>(byte);
}

unsigned char Scrambler::GetScrambleAmount(int index, qint64 offsetInFile) {
    return (offsetInFile+index+this->scrambleKey)%0x100;
}
