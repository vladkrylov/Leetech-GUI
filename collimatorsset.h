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

    void Update(QByteArray response);
    void UpdateAllOrigins(QByteArray response);

    float GetPosition(int collimatorID);
    uint16_t GetMotorOrigin(int collimatorID);
    uint8_t GetSteps2mm(int collimatorID);
    void ResetMotorsData();
    void ResetSteps2mm(int collimatorID);

    void SetHorizontalMaxOpening(float opening);
    void SetVerticalMaxOpening(float opening);
    float GetHorizontalMaxOpening();
    float GetVerticalMaxOpening();

signals:
    void MotorCoordinateChanged(int boxID, int collimatorID, float newCoordinate);
    void HorizontalMaxOpeningChanged(int boxID, float opening);
    void VerticalMaxOpeningChanged(int boxID, float opening);

private:
    int id;
    Encoder **motors;
    float horizontalMaxOpening;
    float verticalMaxOpening;
    QString GenerateCoordinate(const QString &coord_mm, int collimatorID);

private slots:
    void MotorCoordinateChangedSlot(int collimatorID, uint16_t newCoordinate);

public slots:

};

#endif // COLLIMATORSSET_H
