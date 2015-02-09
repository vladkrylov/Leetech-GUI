#include "controller.h"
#include <QDebug>

const int N_sets = 2;

Controller::Controller(QObject *parent) :
    QObject(parent)
{
    PCB = new IP_Connection();
    TestObject = new Tests(this, PCB);

    traj = new Trajectory();

    colSets = new CollimatorsSet*[N_sets];
    for (int i = 0; i < N_sets; ++i) {
        colSets[i] = new CollimatorsSet(i);
        connect(colSets[i], SIGNAL(MotorCoordinateChanged(int,int,uint16_t)), this, SIGNAL(MotorCoordinateChanged(int,int,uint16_t)));
    }
    connect(PCB, SIGNAL(Connected()), this, SIGNAL(Connected()));
    connect(PCB, SIGNAL(Disconnected()), this, SIGNAL(Disconnected()));
    connect(PCB, SIGNAL(dataReceived()), this, SLOT(dataReceived()));
}

Controller::~Controller()
{
    delete PCB;
    delete TestObject;

    delete traj;

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

//    PCB->PCB_SendData("Get_coordinate");
//    response = PCB->PCB_ReceiveData();

    while (!ValidateResponse(response)) {
        if (counter >= 0) break;
        PCB->PCB_SendData("Get_coordinate");
        response = PCB->PCB_ReceiveData();
        counter++;
    }

    return response;
}

void Controller::SetMotorCoordinate(int setID, int motorID, const QString &coord_mm)
{
    QString data_to_send = "move_motor"
            + QString::number(motorID+1)
            + "_tocoord="
            + GenerateCoordinate(coord_mm, setID, motorID)
            + "m"
            + "_steps2mm="
            + QString::number(colSets[setID]->GetSteps2mm(motorID))
            + "_setID="
            + QString::number(setID)
            + "_getTrajectory=1"
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

void Controller::dataReceived()
{
    QByteArray response;
//    response = InitResponse();
    response = PCB->readAll();

    foreach (const QString &str1, traj->indicators) {
        int p1 = response.indexOf(str1);
        int p2 = response.size();
        if (p1 != -1) {
            foreach (const QString &str2, traj->indicators) if (str2 != str1) {
                int newp2 = response.indexOf(str2);
                if ((newp2 != -1) && (newp2 < p2) && (newp2 > p1)) {
                    p2 = newp2;
                    break;
                }
            }
            traj->AddData(str1, response.mid(p1+str1.length(), p2-p1-str1.length()));
        }
    }

    if (traj->AllDataReceived()) {
        traj->WriteToFile();
    }
}

QByteArray Controller::InitResponse()
{
    QByteArray res = "times";
    for (int i=0; i<1000; i++) {
        res.append((char)i);
    }
    res.append("usignal");
    for (int i=0; i<1000; i++) {
        res.append((char)2*i);
    }
    res.append("coords");
    for (int i=0; i<1000; i++) {
        res.append((char)3*i);
    }
    return res;
}

