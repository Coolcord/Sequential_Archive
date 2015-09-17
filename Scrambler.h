#ifndef SCRAMBLER_H
#define SCRAMBLER_H

#include <QByteArray>

class Scrambler
{
public:
    Scrambler();
    ~Scrambler();
    void Scramble(QByteArray *buffer, qint64 offsetInFile, int amount);
    void Unscramble(QByteArray *buffer, qint64 offsetInFile, int amount);

private:
    void ScrambleByte(QByteArray *buffer, int index, qint64 offsetInFile, int amount);
    void UnscrambleByte(QByteArray *buffer, int index, qint64 offsetInFile, int amount);
    int GetScrambleAmount(int index, qint64 offsetInFile, int amount);
};

#endif // SCRAMBLER_H
