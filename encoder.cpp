#include "encoder.h"

Encoder::Encoder()
{
    rawData = "";\
    steps2mm = 0;
}

void Encoder::Update(QByteArray dataFromEncoder)
{
    rawData = dataFromEncoder;
    position = ((uint8_t)rawData[0])<<4 | (((uint8_t)rawData[1])>>4);

    steps2mm = (uint8_t)rawData[5];

    position += steps2mm * 4096;
}

void Encoder::UpdateOrigin(QByteArray dataFromEncoder)
{
    origin = ((uint8_t)dataFromEncoder[0]) | (((uint8_t)dataFromEncoder[1])<<8);
}

int Encoder::GetPosition()
{
    return position * 2000/4095;
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
    return origin;
}