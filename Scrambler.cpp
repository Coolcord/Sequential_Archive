#include "Scrambler.h"
#include <assert.h>

Scrambler::Scrambler() {

}

Scrambler::~Scrambler() {

}

void Scrambler::Scramble(QByteArray *buffer, qint64 offsetInFile, int amount) {
    assert(buffer);
    for (int i = 0; i < buffer->size(); ++i) {
        this->ScrambleByte(buffer, i, offsetInFile, amount);
    }
}

void Scrambler::Unscramble(QByteArray *buffer, qint64 offsetInFile, int amount) {
    assert(buffer);
    for (int i = 0; i < buffer->size(); ++i) {
        this->UnscrambleByte(buffer, i, offsetInFile, amount);
    }
}

void Scrambler::ScrambleByte(QByteArray *buffer, int index, qint64 offsetInFile, int amount) {
    buffer->data()[index] = static_cast<char>(static_cast<unsigned char>((buffer->data()[index]+this->GetScrambleAmount(index, offsetInFile, amount))%0x100));
}

void Scrambler::UnscrambleByte(QByteArray *buffer, int index, qint64 offsetInFile, int amount) {
    int decrementAmount = this->GetScrambleAmount(index, offsetInFile, amount);
    int byte = static_cast<int>(static_cast<unsigned char>(buffer->data()[index]));
    if (byte < decrementAmount) {
        while (byte != 0x00) {
            --byte;
            --decrementAmount;
        }
        assert(decrementAmount >= 0);
        byte = 0xFF - decrementAmount;
    } else {
        byte -= decrementAmount;
    }
    buffer->data()[index] = static_cast<char>(byte);
}

int Scrambler::GetScrambleAmount(int index, qint64 offsetInFile, int amount) {
    return static_cast<int>((offsetInFile+amount+index)%0x100);
}
