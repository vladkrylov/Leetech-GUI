#ifndef COLLIMATORSSET_H
#define COLLIMATORSSET_H

#include <QObject>

#include "encoder.h"

class CollimatorsSet : public QObject
{
    Q_OBJECT
public:
    explicit CollimatorsSet(QObject *parent = 0);
    ~CollimatorsSet();

    void UpdateCoordinate(int motorID, QByteArray response);
    void UpdateOrigin(int motorID, QByteArray response);
    void UpdateAllOrigins(QByteArray response);

    int GetPosition(int motorID);
    uint16_t GetMotorOrigin(int motorID);
    uint8_t GetSteps2mm(int motorID);

    void ResetMotorsData();
    void ResetSteps2mm(int motorID);

private:
    Encoder **motors;
    QString GenerateCoordinate(const QString &coord_mm, int motorID);

signals:
    void Motor1CoordinateChanged(uint16_t );
    void Motor2CoordinateChanged(uint16_t );
    void Motor3CoordinateChanged(uint16_t );
    void Motor4CoordinateChanged(uint16_t );

public slots:

};

#endif // COLLIMATORSSET_H
