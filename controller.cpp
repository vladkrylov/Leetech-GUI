#include "controller.h"
#include <QDebug>

const int N_motors = 4;

Controller::Controller(QObject *parent) :
    QObject(parent)
{
    PCB = new IP_Connection();
    motors = new Encoder*[N_motors];
    for (int i = 0; i < N_motors; ++i) {
        motors[i] = new Encoder();
    }
    TestObject = new Tests(PCB);
}

Controller::~Controller()
{
    delete PCB;
    for (int i = 0; i < N_motors; ++i) {
        delete motors[i];
    }
    delete [] motors;
    delete TestObject;
}

QString Controller::GenerateCoordinate(const QString &mm, const QString &um)
{
    QString res, mm_res, um_res;
    // mm_res processing
    if (mm.length() == 0) {
        mm_res = "00";
    } else if (mm.length() == 1) {
        mm_res = "0" + mm;
    } else {
        mm_res = mm;
    }

    // um_res processing
    if (um.length() == 0) {
        um_res = "000";
    } else if (um.length() == 1) {
        um_res = "00" + um;
    } else if (um.length() == 2) {
        um_res = "0" + um;
    } else {
        um_res = um;
    }

    res = mm_res + um_res;
    return res;
}

void Controller::SetMotorCoordinate(int motorID, const QString &mm, const QString &um)
{
    QByteArray response;
    QString data_to_send = "move_motor" +
            QString::number(motorID+1)
            + "_"
            + GenerateCoordinate(mm, um)
            + "m"
            + "_steps2mm="
            + QString::number(motors[motorID]->GetSteps2mm());

    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;

    data_to_send = "Get_coordinate";
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;
    motors[motorID]->Update(response);
    qDebug() << motors[motorID]->GetPosition();
}

int Controller::IsConnected()
{
    return PCB->IsConnected();
}

int Controller::Connect()
{
    return PCB->PCB_Connect();
}

void Controller::Disconnect()
{
    return PCB->PCB_Disconnect();
}

void Controller::SetIPAddress(const QString &ipaddress)
{
    PCB->SetIPAddress(ipaddress);
}

void Controller::ResetMotorsData()
{
    for (int i = 0; i < N_motors; ++i) {
        motors[i]->ResetSteps2mm();
    }
}

void Controller::Reset(int motorID)
{
    QByteArray response;
    QString data_to_send = "move_motor" +
            QString::number(motorID+1)
            + "_00000"
            + "r"
            + "_steps2mm="
            + QString::number(motors[motorID]->GetSteps2mm());

    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;

    data_to_send = "Get_coordinate";
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;
    motors[motorID]->UpdateOrigin(response);
    qDebug() << motors[motorID]->GetOrigin();
}

void Controller::ResetAll()
{
    QByteArray response;
    QString data_to_send = QString("move_motors")
            + "_00000"
            + "R"
            + "_steps2mm=0";

    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;

    data_to_send = "Get_coordinate";
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;
    for (int i = 0; i < N_motors; ++i) {
        motors[i]->UpdateOrigin(response.mid(2*i, 2));
        qDebug() << motors[i]->GetOrigin();
    }
}

//void Controller::Test(int motorID)
//{
//    QString data_to_send = QString("move_motor")
//            + QString::number(motorID+1)
//            + "_00000"
//            + "t"
//            + "_steps2mm=0";

//    qDebug() << data_to_send;
//    PCB->PCB_SendData(data_to_send);
//    QString data_to_send = QString("test_motor")
//            + QString::number(motorID+1)
//            + "_00000"
//            + "t"
//            + "_steps2mm=0";

//    qDebug() << data_to_send;
//    PCB->PCB_SendData(data_to_send);
//}

//void Controller::TestPulsesForOscilloscope(int motorID)
//{
//    QString data_to_send = QString("move_motor")
//            + QString::number(motorID+1)
//            + "_00000"
//            + "z"
//            + "_steps2mm=0";

//    qDebug() << data_to_send;
//    PCB->PCB_SendData(data_to_send);
//}


