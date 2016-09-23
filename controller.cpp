#include <QDebug>

#include "types.h"
#include "controller.h"

Controller::Controller(QObject *parent) : QObject(parent)
  ,waitForResponse(false)
{
    view = new MainWindow();
    CollMaster = new IP_Connection();

    collSets = new CollimatorsSet*[N_SETS];
    for (int i=0; i<N_SETS; i++) {
        collSets[i] = new CollimatorsSet(i);
        connect(collSets[i], SIGNAL(MotorCoordinateChanged(int,int,float)), this, SLOT(UpdateCoordinate(int,int,float)));
        connect(collSets[i], SIGNAL(HorizontalMaxOpeningChanged(int,float)), view, SLOT(SetMaxOpeningX(int,float)));
        connect(collSets[i], SIGNAL(VerticalMaxOpeningChanged(int,float)), view, SLOT(SetMaxOpeningY(int,float)));
    }

    connect(view, SIGNAL(ConnectCollimatiors(QString)), CollMaster, SLOT(Connect(QString)));
    connect(view, SIGNAL(DisconnectCollimatiors()), CollMaster, SLOT(Disconnect()));
    connect(view, SIGNAL(ResetCollimator(int,int)), this, SLOT(ResetCollimator(int,int)));
    connect(view, SIGNAL(ResetAll(int)), this, SLOT(ResetAll(int)));
    connect(view, SIGNAL(CloseCollimators(int,int)), this, SLOT(CloseCollimators(int,int)));
    connect(view, SIGNAL(UpdateCollimator(int,int)), this, SLOT(GetCollimatorCoordinate(int,int)));
    connect(view, SIGNAL(MoveCollimator(int,int,QString)), this, SLOT(SetCollimatorCoordinate(int,int,QString)));
    connect(view, SIGNAL(SetPWMPeriod(int,int,QString)), this, SLOT(SetPWM(int,int,QString)));
    connect(view, SIGNAL(SetXHoleSize(int,float)), this, SLOT(SetXHoleSize(int,float)));
    connect(view, SIGNAL(SetYHoleSize(int,float)), this, SLOT(SetYHoleSize(int,float)));
    connect(view, SIGNAL(SetXHoleOffset(int,float)), this, SLOT(SetXHoleOffset(int,float)));
    connect(view, SIGNAL(SetYHoleOffset(int,float)), this, SLOT(SetYHoleOffset(int,float)));
    connect(view, SIGNAL(UpdateScene(int)), this, SLOT(UpdateView(int)));

    connect(CollMaster, SIGNAL(Connected()), view, SLOT(CollimatorsConnected()));
    connect(CollMaster, SIGNAL(Disconnected()), view, SLOT(CollimatorsDisconnected()));
    connect(CollMaster, SIGNAL(dataReceived()), this, SLOT(DataReceived()));

    collSets[BOX_ENTRANCE]->SetHorizontalMaxOpening(21.063);
    collSets[BOX_ENTRANCE]->SetVerticalMaxOpening(19.795);
    collSets[BOX_EXIT1]->SetHorizontalMaxOpening(19.877);
    collSets[BOX_EXIT1]->SetVerticalMaxOpening(20.633);

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

int Controller::OppositeCollimator(int collimatorID)
{
    switch (collimatorID) {
    case COLL_LEFT:
        return COLL_RIGHT;
        break;
    case COLL_RIGHT:
        return COLL_LEFT;
        break;
    case COLL_TOP:
        return COLL_BOTTOM;
        break;
    case COLL_BOTTOM:
        return COLL_TOP;
        break;
    }
}

int Controller::GetStrongCollimator(int boxID, int collimatorID)
{
    switch (boxID) {
    case BOX_ENTRANCE:
        switch (collimatorID) {
        case COLL_LEFT:
        case COLL_RIGHT:
            return COLL_RIGHT;
            break;
        case COLL_TOP:
        case COLL_BOTTOM:
            return COLL_TOP;
            break;
        }
        break;

    case BOX_EXIT1:
        switch (collimatorID) {
        case COLL_LEFT:
        case COLL_RIGHT:
            return COLL_LEFT;
            break;
        case COLL_TOP:
        case COLL_BOTTOM:
            return COLL_TOP;
            break;
        }
        break;

    case BOX_EXIT2:
        switch (collimatorID) {
        case COLL_LEFT:
        case COLL_RIGHT:
            return COLL_RIGHT;
            break;
        case COLL_TOP:
        case COLL_BOTTOM:
            return COLL_TOP;
            break;
        }
        break;
    }
}

int Controller::GetWeakCollimator(int boxID, int collimatorID)
{
    return OppositeCollimator(GetStrongCollimator(boxID, collimatorID));
}

void Controller::UpdateCoordinate(int boxID, int collimatorID, float coord)
{
    if (!waitForResponse) {
        view->UpdateCoordinate(boxID, collimatorID, coord);
    } else {
        waitForResponse = false;
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

}

void Controller::CloseCollimators(int boxID, int collimatorID)
{
    int strongCollimator = GetStrongCollimator(boxID, collimatorID);
    int weakCollimator = GetWeakCollimator(boxID, collimatorID);

    // move the strong collimator
    waitForResponse = true;
    SetCollimatorCoordinate(boxID, strongCollimator, "11.000");
    // wait for collimators response
    while(waitForResponse) {
        qApp->processEvents();
    }

    // move the weak collimator
    waitForResponse = true;
    SetCollimatorCoordinate(boxID, weakCollimator, "11.000");
    // wait for collimators response
    while(waitForResponse) {
        qApp->processEvents();
    }

    // update the strong collimator's position
    waitForResponse = true;
    GetCollimatorCoordinate(boxID, strongCollimator);
    // wait for collimators response
    while(waitForResponse) {
        qApp->processEvents();
    }

    float strongCollimatorPosition = collSets[boxID]->GetPosition(strongCollimator); // mm
    float weakCollimatorPosition = collSets[boxID]->GetPosition(weakCollimator); // mm

    // calculate maximum opening in collimators pair
    float maxOpening = strongCollimatorPosition + weakCollimatorPosition;
    if (collimatorID == COLL_RIGHT || collimatorID == COLL_LEFT) {
        if (maxOpening > collSets[boxID]->GetHorizontalMaxOpening())
            collSets[boxID]->SetHorizontalMaxOpening(maxOpening);
    } else {
        if (maxOpening > collSets[boxID]->GetVerticalMaxOpening())
            collSets[boxID]->SetVerticalMaxOpening(maxOpening);
    }

    view->UpdateCoordinate(boxID, strongCollimator, strongCollimatorPosition);
    view->UpdateCoordinate(boxID, weakCollimator, weakCollimatorPosition);
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

void Controller::UpdateView(int collimatorBox)
{
    // 1) display maximum openings
    view->SetMaxOpeningX(collimatorBox, collSets[collimatorBox]->GetHorizontalMaxOpening());
    view->SetMaxOpeningY(collimatorBox, collSets[collimatorBox]->GetVerticalMaxOpening());
    // 2) display current collimators position
    for (int i=0; i<N_COLLIMATORS; i++) {
        float position = collSets[collimatorBox]->GetPosition(i);
        view->UpdateCoordinate(collimatorBox, i, position);
    }
}

void Controller::SetXHoleSize(int collimatorBox, float holeSize)
{
    if (holeSize < 0) return;

    float xLeft = collSets[collimatorBox]->GetPosition(COLL_LEFT);
    float xRight = collSets[collimatorBox]->GetPosition(COLL_RIGHT);
    float centerXOffset = (xLeft - xRight) / 2.;
    float maxOpening = collSets[collimatorBox]->GetHorizontalMaxOpening();

    float xNewLeft = (maxOpening - holeSize)/2. + centerXOffset;
    float xNewRight = (maxOpening - holeSize)/2. - centerXOffset;

    if (xNewLeft>0 && xNewRight>0
            && xNewLeft<15. && xNewRight<15.)  {

        waitForResponse = true;
        SetCollimatorCoordinate(collimatorBox, COLL_LEFT, QString::number(xNewLeft));
        // wait for collimators response
        while(waitForResponse) {
            qApp->processEvents();
        }

        waitForResponse = true;
        SetCollimatorCoordinate(collimatorBox, COLL_RIGHT, QString::number(xNewRight));
        // wait for collimators response
        while(waitForResponse) {
            qApp->processEvents();
        }

        view->UpdateCoordinate(collimatorBox, COLL_LEFT, collSets[collimatorBox]->GetPosition(COLL_LEFT));
        view->UpdateCoordinate(collimatorBox, COLL_RIGHT, collSets[collimatorBox]->GetPosition(COLL_RIGHT));
    }
}

void Controller::SetYHoleSize(int collimatorBox, float holeSize)
{
    if (holeSize < 0) return;

    float yBottom = collSets[collimatorBox]->GetPosition(COLL_BOTTOM);
    float yTop = collSets[collimatorBox]->GetPosition(COLL_TOP);
    float centerYOffset = (yBottom - yTop) / 2.;
    float maxOpening = collSets[collimatorBox]->GetVerticalMaxOpening();

    float yNewBottom = (maxOpening - holeSize)/2. + centerYOffset;
    float yNewTop = (maxOpening - holeSize)/2. - centerYOffset;

    if (yNewBottom>0 && yNewTop>0
            && yNewBottom<15. && yNewTop<15.)  {

        waitForResponse = true;
        SetCollimatorCoordinate(collimatorBox, COLL_BOTTOM, QString::number(yNewBottom));
        // wait for collimators response
        while(waitForResponse) {
            qApp->processEvents();
        }

        waitForResponse = true;
        SetCollimatorCoordinate(collimatorBox, COLL_TOP, QString::number(yNewTop));
        // wait for collimators response
        while(waitForResponse) {
            qApp->processEvents();
        }

        view->UpdateCoordinate(collimatorBox, COLL_BOTTOM, collSets[collimatorBox]->GetPosition(COLL_BOTTOM));
        view->UpdateCoordinate(collimatorBox, COLL_TOP, collSets[collimatorBox]->GetPosition(COLL_TOP));
    }
}

void Controller::SetXHoleOffset(int collimatorBox, float offset)
{
    if (offset < -15. || offset > 15.) return;

    float xLeft = collSets[collimatorBox]->GetPosition(COLL_LEFT);
    float xRight = collSets[collimatorBox]->GetPosition(COLL_RIGHT);
    float maxOpening = collSets[collimatorBox]->GetHorizontalMaxOpening();
    float holeSizeX = maxOpening - xLeft - xRight;

    float xNewLeft = (maxOpening - holeSizeX)/2. + offset;
    float xNewRight = (maxOpening - holeSizeX)/2. - offset;

    qDebug() << xNewLeft << xNewRight;
    if (xNewLeft>0 && xNewRight>0
            && xNewLeft<15. && xNewRight<15.)  {

        waitForResponse = true;
        SetCollimatorCoordinate(collimatorBox, COLL_LEFT, QString::number(xNewLeft));
        // wait for collimators response
        while(waitForResponse) {
            qApp->processEvents();
        }

        waitForResponse = true;
        SetCollimatorCoordinate(collimatorBox, COLL_RIGHT, QString::number(xNewRight));
        // wait for collimators response
        while(waitForResponse) {
            qApp->processEvents();
        }

        view->UpdateCoordinate(collimatorBox, COLL_LEFT, collSets[collimatorBox]->GetPosition(COLL_LEFT));
        view->UpdateCoordinate(collimatorBox, COLL_RIGHT, collSets[collimatorBox]->GetPosition(COLL_RIGHT));
    }
}

void Controller::SetYHoleOffset(int collimatorBox, float offset)
{
    if (offset < -15. || offset > 15.) return;

    float yBottom = collSets[collimatorBox]->GetPosition(COLL_BOTTOM);
    float yTop = collSets[collimatorBox]->GetPosition(COLL_TOP);
    float maxOpening = collSets[collimatorBox]->GetVerticalMaxOpening();
    float holeSizeY = maxOpening - yBottom - yTop;

    float yNewBottom = (maxOpening - holeSizeY)/2. + offset;
    float yNewTop = (maxOpening - holeSizeY)/2. - offset;

    if (yNewBottom>0 && yNewTop>0
            && yNewBottom<15. && yNewTop<15.)  {

        waitForResponse = true;
        SetCollimatorCoordinate(collimatorBox, COLL_BOTTOM, QString::number(yNewBottom));
        // wait for collimators response
        while(waitForResponse) {
            qApp->processEvents();
        }

        waitForResponse = true;
        SetCollimatorCoordinate(collimatorBox, COLL_TOP, QString::number(yNewTop));
        // wait for collimators response
        while(waitForResponse) {
            qApp->processEvents();
        }

        view->UpdateCoordinate(collimatorBox, COLL_BOTTOM, collSets[collimatorBox]->GetPosition(COLL_BOTTOM));
        view->UpdateCoordinate(collimatorBox, COLL_TOP, collSets[collimatorBox]->GetPosition(COLL_TOP));
    }
}
