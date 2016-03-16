#include "encoder.h"

Encoder::Encoder(int motorID, QObject *parent) :
    QObject(parent)
{
    id = motorID;
    rawData = "";
    steps2mm = 0;

    // TODO - initialization from configuration file
    position = 0;
    origin = 0;
}

Encoder::~Encoder()
{

}

int Encoder::GetID()
{
    return id;
}

uint16_t Encoder::steps_to_mm(uint16_t dataInStepsUnits)
{
    return dataInStepsUnits * 2000 / 4096;
}

void Encoder::Update(QByteArray dataFromMaster)
{
    if (QString(dataFromMaster).mid(0, 9) == "response_"){
        rawData = dataFromMaster;
        position = ((uint8_t)rawData[9])<<4 | (((uint8_t)rawData[10])>>4);

        steps2mm = (uint8_t)rawData[14];

        if ((steps2mm == 0) && (position < origin)) {
            origin = position;
            position = 0;
        } else {
            position += -origin;
        }
    }
}

void Encoder::UpdateCoordinate(uint16_t coord)
{
    if (coord != COORD_ERROR) {
        position = coord;
        steps2mm = position/4096;

        if (position < origin) {
            origin = position;
            position = 0;
        } else {
            position -= origin;
        }
        emit MotorCoordinateUpdated(GetID(), GetPosition_mm());
    } else {
        position = COORD_ERROR;
        origin = 0;
        steps2mm = 0;
        emit MotorCoordinateUpdated(GetID(), COORD_ERROR);
    }


}

void Encoder::UpdateOrigin(uint16_t coord)
{
    if (coord != COORD_ERROR) {
        position = 0;
        origin = coord;
        emit MotorCoordinateUpdated(GetID(), GetPosition_mm());
    } else {
        position = COORD_ERROR;
        origin = 0;
        emit MotorCoordinateUpdated(GetID(), COORD_ERROR);
    }
    steps2mm = 0;
}

uint16_t Encoder::GetPosition_mm()
{
    return steps_to_mm(position);
}

uint8_t Encoder::GetSteps2mm()
{
    return steps2mm;
}

void Encoder::ResetSteps2mm()
{
    steps2mm = 0;
}

uint16_t Encoder::GetOrigin_mm()
{
    return steps_to_mm(origin);
}


