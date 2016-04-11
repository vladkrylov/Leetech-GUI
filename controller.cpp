#include <QDebug>

#include "types.h"
#include "controller.h"

Controller::Controller(QObject *parent) : QObject(parent)
{
    view = new MainWindow();
    CollMaster = new IP_Connection();

    collSets = new CollimatorsSet*[N_SETS];
    for (int i=BOX_ENTRANCE; i<N_SETS; i++) {
        collSets[i] = new CollimatorsSet(i);
        connect(collSets[i], SIGNAL(MotorCoordinateChanged(int,int,float)), view, SLOT(UpdateCoordinate(int,int,float)));
    }

    connect(view, SIGNAL(ConnectCollimatiors(QString)), CollMaster, SLOT(Connect(QString)));
    connect(view, SIGNAL(DisconnectCollimatiors()), CollMaster, SLOT(Disconnect()));
    connect(view, SIGNAL(ResetCollimator(int,int)), this, SLOT(ResetCollimator(int,int)));
    connect(view, SIGNAL(MoveCollimator(int,int,QString)), this, SLOT(SetCollimatorCoordinate(int,int,QString)));

    connect(CollMaster, SIGNAL(Connected()), view, SLOT(CollimatorsConnected()));
    connect(CollMaster, SIGNAL(Disconnected()), view, SLOT(CollimatorsDisconnected()));
    connect(CollMaster, SIGNAL(dataReceived()), this, SLOT(DataReceived()));

    view->SetMaxOpeningX(20.);
    view->SetMaxOpeningY(22.);

    view->show();
}

Controller::~Controller()
{
    delete view;
    delete CollMaster;

    for (int i=BOX_ENTRANCE; i<N_SETS; ++i) {
        delete collSets[i];
    }
    delete [] collSets;
}

void Controller::TalkToBoard(const QString &sendPhrase)
{
    qDebug() << endl << sendPhrase;
    CollMaster->PCB_SendData(sendPhrase);
}

void Controller::DataReceived()
{
    QByteArray response;
//    for testing purposes
//    response = InitResponse();
    response = CollMaster->readAll();

    // check for sigle coordinate received
    if (response.contains("response_")) {
        int setID;
        QString setIdIndicator = "set_id=";

        int p1 = response.indexOf(setIdIndicator);
        if (p1 != -1) {
            setID = uint8_t(response.at(p1 + setIdIndicator.length()));
            if ((setID < 0) && (setID > N_SETS)) return;
        } else return;

        collSets[setID]->Update(response);
    }
//    qDebug() << "here";
//    qDebug() << CollMaster->readAll();
}

void Controller::GetCollimatorCoordinate(int boxID, int collimatorID)
{
    QString data_to_send = "getc_motor"
            + QString::number(collimatorID+1)
            + "_steps2mm="
            + QString::number(collSets[boxID]->GetSteps2mm(collimatorID))
            + "_setID="
            + QString::number(boxID)
            + "_______________"
            ;

    TalkToBoard(data_to_send);
}

void Controller::SetCollimatorCoordinate(int boxID, int collimatorID, const QString &coord_mm)
{
    float coord = coord_mm.toFloat();
//    qDebug() << "Set" << setID << ": move motor" << motorID << "to" << coord << "mm.";
//    view->UpdateCoordinate(collimatorID, coord);
}

void Controller::ResetCollimator(int boxID, int collimatorID)
{
    QString data_to_send = "reset_motor" +
            QString::number(collimatorID+1)
            + "_steps2mm="
            + QString::number(collSets[boxID]->GetSteps2mm(collimatorID))
            + "_setID="
            + QString::number(boxID)
            + "_______________"
            ;
    TalkToBoard(data_to_send);

//    view->UpdateCoordinate(collimatorID, 0);
}
