#include "tests.h"
#include "ip_connection.h"

Tests::Tests(IP_Connection *testPCB, QObject *parent):
    QObject(parent)
{
    PCB = testPCB;
}

void Tests::Test(int motorID)
{
    QString data_to_send = QString("move_motor")
            + QString::number(motorID+1)
            + "_00000"
            + "t"
            + "_steps2mm=0";

    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
}

void Tests::TestPulsesForOscilloscope(int motorID)
{
    QString data_to_send = QString("move_motor")
            + QString::number(motorID+1)
            + "_00000"
            + "z"
            + "_steps2mm=0";

    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
}

