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
    TestObject = new Tests(this, PCB);
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

QString Controller::GenerateCoordinate(const QString &mm, const QString &um, int motorID)
{
    int coord = 1000*mm.toInt() + um.toInt() + motors[motorID]->GetOrigin();
    QString res = QString::number(coord);
    while (res.length() < 5) {
        res.prepend('0');
    }
    return res;
}

int Controller::ValidateResponse(const QByteArray &response)
{
    QString validator = "response_";
    return !(QString::compare(response.mid(0,9), validator));
}

QByteArray Controller::TalkToBoard(const QString &sendPhrase)
{
    QByteArray response;
    int counter = 0;

    qDebug() << endl << sendPhrase;
    PCB->PCB_SendData(sendPhrase);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;

    PCB->PCB_SendData("Get_coordinate");
    response = PCB->PCB_ReceiveData();
    qDebug() << response;

    while (!ValidateResponse(response)) {
        if (counter > 2) break;
        PCB->PCB_SendData("Get_coordinate");
        response = PCB->PCB_ReceiveData();
        qDebug() << response;
        counter++;
    }

    return response;
}

void Controller::SetMotorCoordinate(int motorID, const QString &mm, const QString &um)
{
    QString data_to_send = "move_motor" +
            QString::number(motorID+1)
            + "_"
            + GenerateCoordinate(mm, um, motorID)
            + "m"
            + "_steps2mm="
            + QString::number(motors[motorID]->GetSteps2mm());
    QByteArray response = TalkToBoard(data_to_send);
    motors[motorID]->UpdateCoordinate(response);
    qDebug() << endl << motors[motorID]->GetPosition() <<"\t"<< motors[motorID]->GetSteps2mm();
}

int Controller::GetMotorCoordinate(int motorID)
{
    QString data_to_send = "getc_motor"
            + QString::number(motorID+1)
            + "_steps2mm="
            + QString::number(motors[motorID]->GetSteps2mm());

    QByteArray response = TalkToBoard(data_to_send);
    motors[motorID]->UpdateCoordinate(response);
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
    QString data_to_send = "reset_motor" +
            QString::number(motorID+1)
            + "_steps2mm="
            + QString::number(motors[motorID]->GetSteps2mm());
    QByteArray response = TalkToBoard(data_to_send);
    motors[motorID]->UpdateOrigin(response);
    motors[motorID]->ResetSteps2mm();
    qDebug() << motors[motorID]->GetOrigin();
}

void Controller::ResetAll()
{
    QString data_to_send = QString("reset_all");
    QByteArray response = TalkToBoard(data_to_send);
    for (int i = 0; i < N_motors; ++i) {
        motors[i]->UpdateOrigin(response.mid(2*i, 2));
        qDebug() << motors[i]->GetOrigin();
    }
}

void Controller::SetPulses(const QString &width, const QString &period)
{
    QString data_to_send = QString("set_pulses:")
            + "w=" + width
            + ";T=" + period;
    PCB->PCB_SendData(data_to_send);
}


