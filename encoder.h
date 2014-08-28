#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#include <QString>
#include <QByteArray>
#include <QDebug>

#define pBINARY 2

class Encoder
{
private:
    QByteArray rawData;
    uint16_t position; // in 0.488um
    uint8_t steps2mm;
    bool OffsetCompensationFinished;
    uint16_t origin;
    uint16_t steps_to_mm(uint16_t dataInStepsUnits);


public:
    Encoder();

    void Update(QByteArray);
    void UpdateCoordinate(QByteArray coordData);
    void UpdateOrigin(QByteArray coordData);

    int GetPosition();
    uint8_t GetSteps2mm();
    uint16_t GetOrigin();

    void ResetSteps2mm();
};

#endif // ENCODER_H
