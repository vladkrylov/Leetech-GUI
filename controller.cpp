#include "controller.h"
#include <QDebug>

const int N_sets = 2;

Controller::Controller(QObject *parent) :
    QObject(parent)
{
    PCB = new IP_Connection();
    TestObject = new Tests(this, PCB);

    colSets = new CollimatorsSet*[N_sets];
    for (int i = 0; i < N_sets; ++i) {
        colSets[i] = new CollimatorsSet(i);
        connect(colSets[i], SIGNAL(MotorCoordinateChanged(int,int,uint16_t)), this, SIGNAL(MotorCoordinateChanged(int,int,uint16_t)));
    }
}

Controller::~Controller()
{
    delete PCB;
    delete TestObject;

    for (int i = 0; i < N_sets; ++i) {
        delete colSets[i];
    }
    delete [] colSets;
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

int Controller::ValidateResponse(const QByteArray &response)
{
    QString validator = "response_";
    return !(QString::compare(response.mid(0,9), validator));
}

QString Controller::GenerateCoordinate(const QString &coord_mm, int setID, int motorID)
{
    QString copyCoord = coord_mm;
    int coord = copyCoord.replace(QString(","), QString(".")).toFloat() * 1000 + colSets[setID]->GetMotorOrigin(motorID);
//            motors[motorID]->GetOrigin();
    QString res = QString::number(coord);
    while (res.length() < 5) {
        res.prepend('0');
    }
    return res;
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
        if (counter >= 0) break;
        PCB->PCB_SendData("Get_coordinate");
        response = PCB->PCB_ReceiveData();
        qDebug() << response;
        counter++;
    }

    return response;
}

void Controller::SetMotorCoordinate(int setID, int motorID, const QString &coord_mm)
{
    QString data_to_send = "move_motor"
            + QString::number(motorID+1)
            + "_"
            + GenerateCoordinate(coord_mm, setID, motorID)
            + "m"
            + "_steps2mm="
            + QString::number(colSets[setID]->GetSteps2mm(motorID))
            + "_setID="
            + QString::number(setID)
            ;
    QByteArray response = TalkToBoard(data_to_send);
    colSets[setID]->UpdateCoordinate(motorID, response);
    qDebug() << endl << colSets[setID]->GetPosition(motorID) <<"\t"<< colSets[setID]->GetSteps2mm(motorID);
}

void Controller::GetMotorCoordinate(int setID, int motorID)
{
    QString data_to_send = "getc_motor"
            + QString::number(motorID+1)
            + "_steps2mm="
            + QString::number(colSets[setID]->GetSteps2mm(motorID))
            + "_setID="
            + QString::number(setID)
            ;

    QByteArray response = TalkToBoard(data_to_send);
    colSets[setID]->UpdateCoordinate(motorID, response);
    qDebug() << colSets[setID]->GetPosition(motorID);
}

void Controller::ResetMotorsData(int setID)
{
    colSets[setID]->ResetMotorsData();
}

void Controller::Reset(int setID, int motorID)
{
    QString data_to_send = "reset_motor" +
            QString::number(motorID+1)
            + "_steps2mm="
            + QString::number(colSets[setID]->GetSteps2mm(motorID))
            + "_setID="
            + QString::number(setID)
            ;
    QByteArray response = TalkToBoard(data_to_send);
    colSets[setID]->UpdateOrigin(motorID, response);
    colSets[setID]->ResetSteps2mm(motorID);
    qDebug() << colSets[setID]->GetMotorOrigin(motorID);
}

void Controller::ResetAll(int setID)
{
    QString data_to_send = QString("reset_all")
                            + "_setID="
                            + QString::number(setID)
                            ;
    QByteArray response = TalkToBoard(data_to_send);
    colSets[setID]->UpdateAllOrigins(response);
}

void Controller::SetPulses(int setID, int motorID, const QString &width, const QString &period)
{
    QString data_to_send = QString("set_pulses:")
            + "w=" + width
            + ";T=" + period
            + "_setID="
            + QString::number(setID)
            + "_motorID="
            + QString::number(motorID)
            ;
    PCB->PCB_SendData(data_to_send);
}

uint16_t Controller::ShowMotorCoordinate(int setID, int motorID)
{
    return colSets[setID]->GetPosition(motorID);
}
