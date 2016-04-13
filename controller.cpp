#include <QDebug>

#include "types.h"
#include "controller.h"

Controller::Controller(QObject *parent) : QObject(parent)
  ,applicationState(READY)
  ,responsesRequested(0)
{
    view = new MainWindow();
    CollMaster = new IP_Connection();

    collSets = new CollimatorsSet*[N_SETS];
    for (int i=BOX_ENTRANCE; i<N_SETS; i++) {
        collSets[i] = new CollimatorsSet(i);
        connect(collSets[i], SIGNAL(MotorCoordinateChanged(int,int,float)), this, SLOT(UpdateCoordinate(int,int,float)));
    }

    connect(view, SIGNAL(ConnectCollimatiors(QString)), CollMaster, SLOT(Connect(QString)));
    connect(view, SIGNAL(DisconnectCollimatiors()), CollMaster, SLOT(Disconnect()));
    connect(view, SIGNAL(ResetCollimator(int,int)), this, SLOT(ResetCollimator(int,int)));
    connect(view, SIGNAL(ResetAll(int)), this, SLOT(ResetAll(int)));
    connect(view, SIGNAL(CloseCollimators(int,int)), this, SLOT(CloseCollimators(int,int)));
    connect(view, SIGNAL(UpdateCollimator(int,int)), this, SLOT(GetCollimatorCoordinate(int,int)));
    connect(view, SIGNAL(MoveCollimator(int,int,QString)), this, SLOT(SetCollimatorCoordinate(int,int,QString)));
    connect(view, SIGNAL(SetPWMPeriod(int,int,QString)), this, SLOT(SetPWM(int,int,QString)));

    connect(CollMaster, SIGNAL(Connected()), view, SLOT(CollimatorsConnected()));
    connect(CollMaster, SIGNAL(Disconnected()), view, SLOT(CollimatorsDisconnected()));
    connect(CollMaster, SIGNAL(dataReceived()), this, SLOT(DataReceived()));

    view->SetMaxOpeningX(19.941);
    view->SetMaxOpeningY(20.992);

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

QString Controller::GenerateCoordinate(const QString &coord_mm, int boxID, int collimatorID)
{
    QString copyCoord = coord_mm;
    int coord = copyCoord.replace(QString(","), QString(".")).toFloat() * 1000 + collSets[boxID]->GetMotorOrigin(collimatorID);
    QString res = QString::number(coord);
    while (res.length() < 5) {
        res.prepend('0');
    }
    return res;
}

int Controller::GetBoxSpecificCollimatorID(int boxID, int collimatorID)
{
    switch (boxID) {
    case BOX_ENTRANCE:
        switch (collimatorID){
        case COLL_RIGHT:
            return 3;
            break;
        case COLL_LEFT:
            return 4;
            break;
        case COLL_TOP:
            return 2;
            break;
        case COLL_BOTTOM:
            return 1;
            break;
        }
    case BOX_EXIT1:
        switch (collimatorID){
        case COLL_RIGHT:
            return 4;
            break;
        case COLL_LEFT:
            return 3;
            break;
        case COLL_TOP:
            return 1;
            break;
        case COLL_BOTTOM:
            return 2;
            break;
        }
    case BOX_EXIT2:
        switch (collimatorID){
        case COLL_RIGHT:
            return 3;
            break;
        case COLL_LEFT:
            return 4;
            break;
        case COLL_TOP:
            return 2;
            break;
        case COLL_BOTTOM:
            return 1;
            break;
        }
    }
}

void Controller::UpdateCoordinate(int boxID, int collimatorID, float coord)
{
    if (responsesRequested == 0) {
        view->UpdateCoordinate(boxID, collimatorID, coord);
    } else {
        responsesRequested--;
    }
}

void Controller::SetCollimatorCoordinate(int boxID, int collimatorID, const QString &coord_mm)
{
    QString data_to_send = "move_motor"
            + QString::number(collimatorID+1)
            + "_tocoord="
            + GenerateCoordinate(coord_mm, boxID, collimatorID)
            + "m"
            + "_steps2mm="
            + QString::number(collSets[boxID]->GetSteps2mm(collimatorID))
            + "_setID="
            + QString::number(boxID)
//            + "_getTrajectory=1"
            + "_______________"
            ;
    TalkToBoard(data_to_send);
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
}

void Controller::ResetAll(int collimatorBox)
{
    responsesRequested = N_COLLIMATORS;

}

void Controller::CloseCollimators(int boxID, int collimatorID)
{
    switch (collimatorID) {
    case COLL_RIGHT:
    case COLL_LEFT:
        break;
    case COLL_TOP:
    case COLL_BOTTOM:
        responsesRequested = 2;
        // move the top collimator
        SetCollimatorCoordinate(boxID, COLL_TOP, "12.000");
        // wait for collimators response
        while(responsesRequested != 1) {
            qApp->processEvents();
        }
        // move the bottom collimator
        SetCollimatorCoordinate(boxID, COLL_BOTTOM, "12.000");
        // wait for collimators response
        while(responsesRequested != 1) {
            qApp->processEvents();
        }
        view->UpdateCoordinate(boxID, COLL_TOP, collSets[boxID]->GetPosition(COLL_TOP));
        view->UpdateCoordinate(boxID, COLL_BOTTOM, collSets[boxID]->GetPosition(COLL_BOTTOM));
        break;
    }
}

void Controller::SetPWM(int collimatorBox, int collimatorID, QString T)
{
    QString data_to_send = QString("set_pulses:")
            + "w=120"
            + ";T=" + T
            + "_setID="
            + QString::number(collimatorBox)
            + "_motorID="
            + QString::number(collimatorID)
            + "_______________"
            ;
    TalkToBoard(data_to_send);
}
