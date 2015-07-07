#include "trajectory.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>

Trajectory::Trajectory(QObject *parent) :
    QObject(parent)
{
    length = 0;
    state = UNKNOWN;
    indicators = QStringList() << "times" << "usignal" << "coords";
    indicators << "set_id=" << "motor_id=" << "dest=" << "prec=";

    times = NULL;
    uSignal = NULL;
    coordinates = NULL;
}

Trajectory::~Trajectory()
{
    CleanArrays();
}

uint16_t Trajectory::Convert2_8to16(char msb, char lsb)
{
    uint16_t mspart = ((uint8_t)msb)<<8;
    uint16_t lspart = (uint8_t)lsb;
    return mspart + lspart;
}

void Trajectory::AddData(const QString indicator, QByteArray newData)
{
    if (indicator == "set_id=") {
        uint8_t setIDn = (uint8_t)newData.at(0);
        switch (setIDn) {
        case 0:
            setID = "Entrance";
            break;
        case 1:
            setID = "Exit1";
            break;
        case 2:
            setID = "Exit2";
            break;
        default:
            setID = "Unknown";
            break;
        }
        return;
    }
    if (indicator == "motor_id=") {
        motorID = (uint8_t)newData.at(0);
        return;
    }
    if (indicator == "dest=") {
        destination = Convert2_8to16(newData.at(0), newData.at(1));
        return;
    }
    // check for trajectory arrays
    int len2 = newData.size()/2;

    // we expect the new Data of even size because every trajectory array element
    // has a size of 2 bytes
    if (newData.size() != len2*2) {
        qDebug() <<"Wrong data: odd length of data array.";
        return;
    }

    // if first data array received
    if (length == 0) {
         length = len2;
    }

    // if length of received data is not equal to length of other data arrays
    if (length != len2) {
        qDebug() <<"Wrong data: different lengths of data arrays.";
        return;
    }

    uint16_t *array;
    if (indicator == "times") {
        state |= TIMES_RECEIVED;
        times = new uint16_t[length];
        array = times;
    } else if (indicator == "usignal") {
        state |= USIGNAL_RECEIVED;
        uSignal = new uint16_t[length];
        array = uSignal;
    } else if (indicator == "coords") {
        state |= COORDINATES_RECEIVED;
        coordinates = new uint16_t[length];
        array = coordinates;
    } else {
        qDebug() <<"Unknown data recieved.";
        return;
    }
    SetArray(array, &newData);
}


void Trajectory::SetArray(uint16_t* array, QByteArray* newData)
{
    for(int i=0; i<length; i++) {
        array[i] = Convert2_8to16(newData->at(2*i), newData->at(2*i+1));
    }
}


void Trajectory::CleanArrays()
{
    state = UNKNOWN;
    length = 0;

    if (times) delete[] times;
    if (uSignal) delete[] uSignal;
    if (coordinates) delete[] coordinates;
}

int Trajectory::AllDataReceived()
{
    return ((state & TIMES_RECEIVED) &&
            (state & USIGNAL_RECEIVED) &&
            (state & COORDINATES_RECEIVED));
}

void Trajectory::WriteToFile()
{
    if (AllDataReceived()) {
        QString filename = setID
                        + QString::number(motorID)
                        + "_U="
                        + QString::number(uSignal[0])
                        + "_dest="
                        + QString::number(destination)
                        + ".yaml";
        QFile file(filename);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream ya(&file);

        ya << "t: [" << times[0];
        for(int i=1; i<length; i++) {
            ya << ", " << times[i];
        }
        ya << "]" << endl;


        ya << "u: [" << uSignal[0];
        for(int i=1; i<length; i++) {
            ya << ", " << uSignal[i];
        }
        ya << "]" << endl;


        ya << "x: [" << coordinates[0];
        for(int i=1; i<length; i++) {
            ya << ", " << coordinates[i];
        }
        ya << "]" << endl;

        file.close();
        CleanArrays();
    }
}


