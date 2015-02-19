#include "tests.h"
#include "controller.h"
#include "ip_connection.h"

#include <QFileInfo>
#include <QThread>

Tests::Tests(Controller *c, IP_Connection *testPCB, QObject *parent):
    QObject(parent)
{
    PCB = testPCB;
    control = c;

//    connect(PCB->socket, SIGNAL(readyRead()), this, SLOT(PrintBoardResponse()));
}

void Tests::Test(int setID, int motorID)
{
    QString data_to_send = QString("test_motor")
            + QString::number(motorID+1)
            + "_setID="
            + QString::number(setID);

//    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
}

void Tests::TestPulsesForOscilloscope()
{
    QString data_to_send = QString("test_pulses");

    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
}

void Tests::TestForce(int width, int begin, int end, int setID, int motorID)
{
    stopForseTest = 0;
    for (int period = begin; period <= end; period++) {
        if (stopForseTest) break;

        qDebug() << period;
        control->SetPulses(setID, motorID, QString::number(width), QString::number(period));

        QTest::qWait(250);

        Test(setID, motorID);
        QTest::qWait(5000);
    }
}

void Tests::StopForseTest()
{
    stopForseTest = 1;
}

void Tests::PrintBoardResponse()
{
    QString resp = PCB->socket->readAll();
    qDebug() << resp;
}


void Tests::CollectData(int setID, int motorID, QString period)
{
    QFileInfo *info = new QFileInfo("D:\\Leetech\\Slave\\DebugLogs\\MatlabSignalFile.log");
    QDateTime lastMod = info->lastModified();
    QDateTime modified;

    for(int pulseWidth = 110; pulseWidth >= 50; pulseWidth -= 10) {
        control->SetPulses(setID, motorID, QString::number(pulseWidth), period);
        QThread::msleep(200);
        for(double destination = 3.; destination <= 12.; destination += 1.) {
            control->Reset(setID, motorID);
            QThread::msleep(200);
            control->SetMotorCoordinate(setID, motorID, QString::number(destination));
            QThread::msleep(200);

            while(1) {
                info->refresh();
                QThread::msleep(500);
                modified = info->lastModified();
                if (modified > lastMod) {
                    lastMod = modified;
                    break;
                }
            }
        }
    }
}

void Tests::WaitForMotorData()
{
    QFileInfo *info = new QFileInfo("D:\\Leetech\\Slave\\DebugLogs\\MatlabSignalFile.log");
    QDateTime lastMod = info->lastModified();
    QDateTime modified;
    bool received = false;
    while(received == false) {
        info->refresh();
        QThread::msleep(100);
        modified = info->lastModified();
        if (modified > lastMod) {
            received = true;
        }
    }
    delete info;
}

