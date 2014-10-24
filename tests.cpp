#include "tests.h"
#include "controller.h"
#include "ip_connection.h"

Tests::Tests(Controller *c, IP_Connection *testPCB, QObject *parent):
    QObject(parent)
{
    PCB = testPCB;
    control = c;

//    connect(PCB->socket, SIGNAL(readyRead()), this, SLOT(PrintBoardResponse()));
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

void Tests::TestForce(int width, int begin, int end, int setID, int motorID)
{
    stopForseTest = 0;
    for (int period = begin; period <= end; period++) {
        if (stopForseTest) break;

        qDebug() << period;
        control->SetPulses(setID, motorID, QString::number(width), QString::number(period));

        QTest::qWait(250);

        Test(motorID);
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

void Tests::TestLWIP(int numerOfRetries)
{
    int success = 0;
    QString response;
    for (int i = 0; i < numerOfRetries; ++i) {
        response = control->TalkToBoard("move_motor1_00000m_steps2mm=110");
        if (response.mid(0, 9) == "response_") {
            success++;
        }
    }
    qDebug() << endl <<"Bug ratio ="<< 100.*(numerOfRetries - success)/numerOfRetries <<"%";
}


