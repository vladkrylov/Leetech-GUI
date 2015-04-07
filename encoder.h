#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <QObject>

#include <QString>
#include <QByteArray>
#include <QDebug>

#define pBINARY 2

class Encoder : public QObject
{
    Q_OBJECT
private:
    int id;

    QByteArray rawData;
    uint16_t position; // in 0.488um
    uint8_t steps2mm;
    bool OffsetCompensationFinished;
    uint16_t origin;
    uint16_t steps_to_mm(uint16_t dataInStepsUnits);

public:
    explicit Encoder(int motorID, QObject *parent = 0);
    ~Encoder();

    int GetID();

    void Update(QByteArray dataFromMaster);
    void UpdateCoordinate(uint16_t coord);
    void UpdateOrigin(uint16_t coord);

    uint16_t GetPosition();
    uint8_t GetSteps2mm();
    uint16_t GetOrigin();

    void ResetSteps2mm();

signals:
    void MotorCoordinateUpdated(int motorID, uint16_t newCoordinate);
public slots:

};

#endif // ENCODER_H
