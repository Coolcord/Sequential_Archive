#ifndef SCRAMBLER_H
#define SCRAMBLER_H

#include <QByteArray>

class Scrambler
{
public:
    Scrambler(unsigned char scrambleKey);
    ~Scrambler() {}
    unsigned char Get_Scramble_Key();
    void Scramble(QByteArray &buffer, qint64 offsetInFile);
    void Unscramble(QByteArray &buffer, qint64 offsetInFile);

private:
    void ScrambleByte(QByteArray &buffer, int index, qint64 offsetInFile);
    void UnscrambleByte(QByteArray &buffer, int index, qint64 offsetInFile);
    unsigned char GetScrambleAmount(int index, qint64 offsetInFile);

    unsigned char scrambleKey;
};

#endif // SCRAMBLER_H
