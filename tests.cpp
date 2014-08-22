#include "tests.h"

#include "controller.h"
#include "ip_connection.h"
#include <QThread>

Tests::Tests(Controller *c, IP_Connection *testPCB, QObject *parent):
    QObject(parent)
{
    PCB = testPCB;
    control = c;
}

void Tests::Test(int motorID)
{
    QString data_to_send = QString("test_motor")
            + QString::number(motorID+1);

//    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
}

void Tests::TestPulsesForOscilloscope()
{
    QString data_to_send = QString("test_pulses");

    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
}

void Tests::TestForce(int numOfRepeats, int timePerMoving, int motorID)
{
    int period = 0;
    int width = 0;

    width = 120;
    period = 245;

    for (; period < 280; period++) {
        qDebug() << period;
        control->SetPulses(QString::number(width), QString::number(period));

        QThread::msleep(250);

        Test(motorID);
        QThread::msleep(1500);
    }
}

