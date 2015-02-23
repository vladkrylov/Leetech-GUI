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

void Controller::TalkToBoard(const QString &sendPhrase)
{
    qDebug() << endl << sendPhrase;
    PCB->PCB_SendData(sendPhrase);
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
//            + "_getTrajectory=1"
            + "_______________"
            ;
    TalkToBoard(data_to_send);
}

void Controller::GetMotorCoordinate(int setID, int motorID)
{
    QString data_to_send = "getc_motor"
            + QString::number(motorID+1)
            + "_steps2mm="
            + QString::number(colSets[setID]->GetSteps2mm(motorID))
            + "_setID="
            + QString::number(setID)
            + "_______________"
            ;

    TalkToBoard(data_to_send);
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
            + "_______________"
            ;
    TalkToBoard(data_to_send);
}

void Controller::ResetAll(int setID)
{
    QString data_to_send = QString("reset_all")
                            + "_setID="
                            + QString::number(setID)
                            ;
    TalkToBoard(data_to_send);
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
            + "_______________"
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
//    for testing purposes
//    response = InitResponse();
    response = PCB->readAll();

    // check for sigle coordinate received
    if (response.contains("response_")) {
        int setID;
        QString setIdIndicator = "set_id=";

        int p1 = response.indexOf(setIdIndicator);
        if (p1 != -1) {
            setID = uint8_t(response.at(p1 + setIdIndicator.length()));
            if ((setID < 0) && (setID > N_sets)) return;
        } else return;

        colSets[setID]->Update(response);
    } else {
        // check for trajectory data
        foreach (const QString &str1, traj->indicators) {
            int p1 = response.indexOf(str1);
            int p2 = response.size();
            if (p1 != -1) {
                foreach (const QString &str2, traj->indicators) if (str2 != str1) {
                    int newp2 = response.indexOf(str2);
                    if ((newp2 != -1) && (newp2 < p2) && (newp2 > p1)) {
                        p2 = newp2;
                    }
                }
                traj->AddData(str1, response.mid(p1+str1.length()+1, p2-p1-str1.length()-1));
            }
        }

        if (traj->AllDataReceived()) {
            traj->WriteToFile();
        }
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

