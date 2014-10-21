#include "collimatorsset.h"
#include <QDebug>

const int N_motors = 4;

CollimatorsSet::CollimatorsSet(int setID, QObject *parent) :
    QObject(parent)
{
    id = setID;
    motors = new Encoder*[N_motors];
    for (int i = 0; i < N_motors; ++i) {
        motors[i] = new Encoder(i);
        connect(motors[i], SIGNAL(MotorCoordinateUpdated(int, uint16_t)), this, SLOT(MotorCoordinateChangedSlot(int,uint16_t )));
    }
}

CollimatorsSet::~CollimatorsSet()
{
    for (int i = 0; i < N_motors; ++i) {
        delete motors[i];
    }
    delete [] motors;
}

int CollimatorsSet::GetID()
{
    return id;
}

void CollimatorsSet::MotorCoordinateChangedSlot(int motorID, uint16_t newCoordinate)
{
    emit MotorCoordinateChanged(GetID(), motorID, newCoordinate);
}

uint16_t CollimatorsSet::GetMotorOrigin(int motorID)
{
    return motors[motorID]->GetOrigin();
}

int CollimatorsSet::GetPosition(int motorID)
{
    return motors[motorID]->GetPosition();
}

uint8_t CollimatorsSet::GetSteps2mm(int motorID)
{
    return motors[motorID]->GetSteps2mm();
}

void CollimatorsSet::ResetMotorsData()
{
    for (int i = 0; i < N_motors; ++i) {
        motors[i]->ResetSteps2mm();
    }
}

void CollimatorsSet::ResetSteps2mm(int motorID)
{
    motors[motorID]->ResetSteps2mm();
}

void CollimatorsSet::UpdateCoordinate(int motorID, QByteArray response)
{
    motors[motorID]->UpdateCoordinate(response);
}

void CollimatorsSet::UpdateOrigin(int motorID, QByteArray response)
{
    motors[motorID]->UpdateOrigin(response);
}

void CollimatorsSet::UpdateAllOrigins(QByteArray response)
{
    for (int i = 0; i < N_motors; ++i) {
        motors[i]->UpdateOrigin(response.mid(2*i, 2));
        qDebug() << motors[i]->GetOrigin();
    }
}

