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

void CollimatorsSet::MotorCoordinateChangedSlot(int collimatorID, uint16_t newCoordinate)
{
    emit MotorCoordinateChanged(GetID(), collimatorID, newCoordinate/1000.);
}

uint16_t CollimatorsSet::GetMotorOrigin(int collimatorID)
{
    return motors[collimatorID]->GetOrigin_mm();
}

float CollimatorsSet::GetPosition(int collimatorID)
{
    return motors[collimatorID]->GetPosition_mm()/1000.;
}

uint8_t CollimatorsSet::GetSteps2mm(int collimatorID)
{
    return motors[collimatorID]->GetSteps2mm();
}

void CollimatorsSet::ResetMotorsData()
{
    for (int i = 0; i < N_motors; ++i) {
        motors[i]->ResetSteps2mm();
    }
}

void CollimatorsSet::ResetSteps2mm(int collimatorID)
{
    motors[collimatorID]->ResetSteps2mm();
}

void CollimatorsSet::SetHorizontalMaxOpening(float opening)
{
    horizontalMaxOpening = opening;
    emit HorizontalMaxOpeningChanged(GetID(), horizontalMaxOpening);
}

void CollimatorsSet::SetVerticalMaxOpening(float opening)
{
    verticalMaxOpening = opening;
    emit VerticalMaxOpeningChanged(GetID(), verticalMaxOpening);
}

float CollimatorsSet::GetHorizontalMaxOpening()
{
    return horizontalMaxOpening;
}

float CollimatorsSet::GetVerticalMaxOpening()
{
    return verticalMaxOpening;
}

void CollimatorsSet::Update(QByteArray response)
{
    QString motorIdIndicator = "motor_id=";
    QString coordIndicator = "coord=";
    uint8_t motorID;
    uint16_t coord;

    int p1 = response.indexOf(motorIdIndicator);
    if (p1 != -1) {
        motorID = uint8_t(response.at(p1 + motorIdIndicator.length())) - 1;
        if (motorID > N_motors) return;
    } else return;

    p1 = response.indexOf(coordIndicator);
    if (p1 != -1) {
         coord = (uint8_t(response.at(p1 + coordIndicator.length())))<<8 |
                 (uint8_t(response.at(p1 + coordIndicator.length() + 1)));
    } else return;

    if (response.contains("reset")) {
        motors[motorID]->UpdateOrigin(coord);
    } else {
        motors[motorID]->UpdateCoordinate(coord);
    }
}

void CollimatorsSet::UpdateAllOrigins(QByteArray response)
{
//    for (int i = 0; i < N_motors; ++i) {
//        motors[i]->UpdateOrigin(response.mid(2*i, 2));
//        qDebug() << motors[i]->GetOrigin();
//    }
}

