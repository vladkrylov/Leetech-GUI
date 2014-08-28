#include "encoder.h"
#include <QString>

Encoder::Encoder(QObject *parent) :
    QObject(parent)
{
    rawData = "";
    steps2mm = 0;

    // TODO - initialization from configuration file
    position = 0;
    origin = 0;
}

Encoder::~Encoder()
{

}

uint16_t Encoder::steps_to_mm(uint16_t dataInStepsUnits)
{
    return dataInStepsUnits * 2000 / 4096;
}

void Encoder::Update(QByteArray dataFromEncoder)
{
    if (QString(dataFromEncoder).mid(0, 9) == "response_"){
        rawData = dataFromEncoder;
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

void Encoder::UpdateCoordinate(QByteArray coordData)
{
    if (QString(coordData).mid(0, 9) == "response_"){
        position = ((uint8_t)coordData[9])<<8 | ((uint8_t)coordData[10]);
        steps2mm = (uint8_t)coordData[11];

        if ((steps2mm == 0) && (position < origin)) {
            origin = position;
            position = 0;
        } else {
            position += -origin;
        }
        emit MotorCoordinateUpdated(position);
    }
}

void Encoder::UpdateOrigin(QByteArray coordData)
{
//    origin = ((uint8_t)coordData[0]) | (((uint8_t)coordData[1])<<8);
    origin = ((uint8_t)coordData[9])<<8 | ((uint8_t)coordData[10]);
}

int Encoder::GetPosition()
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

uint16_t Encoder::GetOrigin()
{
    return steps_to_mm(origin);
}

void Encoder::MotorCoordinateUpdated(uint16_t) {}
