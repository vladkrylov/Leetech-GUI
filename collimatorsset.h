#ifndef COLLIMATORSSET_H
#define COLLIMATORSSET_H

#include <QObject>

#include "encoder.h"

class CollimatorsSet : public QObject
{
    Q_OBJECT
public:
    explicit CollimatorsSet(int setID, QObject *parent = 0);
    ~CollimatorsSet();

    int GetID();

    void UpdateCoordinate(int motorID, QByteArray response);
    void UpdateOrigin(int motorID, QByteArray response);
    void UpdateAllOrigins(QByteArray response);

    int GetPosition(int motorID);
    uint16_t GetMotorOrigin(int motorID);
    uint8_t GetSteps2mm(int motorID);

    void ResetMotorsData();
    void ResetSteps2mm(int motorID);

private:
    int id;
    Encoder **motors;
    QString GenerateCoordinate(const QString &coord_mm, int motorID);

private slots:
    void MotorCoordinateChangedSlot(int motorID, uint16_t newCoordinate);

signals:
    void MotorCoordinateChanged(int setID, int motorID, uint16_t newCoordinate);

public slots:

};

#endif // COLLIMATORSSET_H
