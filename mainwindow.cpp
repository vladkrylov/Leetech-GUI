#include <QtWidgets>
#include <QGraphicsProxyWidget>
#include <QThread>

#include "mainwindow.h"
#include "ui_mainwindow.h"

const float usPerTimerTick = 0.025;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  , sceneSize(500)
  , animation(NULL)
  , animationTimer(NULL)
{
    ui->setupUi(this);
    ui->collimatorScene->setMinimumSize(sceneSize+2, sceneSize+2);

    CreateTopMenu();

    ConstructScene();
    ConnectUIActions();
    ConnectCollimatorsRequests();
}

MainWindow::~MainWindow()
{
    delete ui;

    delete left;
    delete right;
    delete top;
    delete bottom;

    delete scene;

    delete titlesFont;
    delete textFont;

    if (animationTimer)
        delete animationTimer;
    if (animation)
        delete animation;
}

void MainWindow::ConstructScene()
{
    scene = new QGraphicsScene(this);
    ui->collimatorScene->setScene(scene);
    scene->setSceneRect(0, 0, sceneSize, sceneSize);

    /**
     * Add collimators to the scene
     */
    int collimLength = (int) (0.38*sceneSize);
    left = new CollimatorGraphicsItem(collimLength);
    left->setRotation(0);
    left->setPos(0, scene->sceneRect().height()/2 - left->boundingRect().height()/2);

    right = new CollimatorGraphicsItem(collimLength);
    right->setRotation(180);
    right->setPos(scene->sceneRect().width()-1, scene->sceneRect().height()/2 + right->boundingRect().height()/2);

    top = new CollimatorGraphicsItem(collimLength);
    top->setRotation(90);
    top->setPos(scene->sceneRect().width()/2 + top->boundingRect().height()/2, 0);
//    UpdateCoordinate(COLL_TOP, 0);

    bottom = new CollimatorGraphicsItem(collimLength);
    bottom->setRotation(270);
    bottom->setPos(scene->sceneRect().width()/2 - bottom->boundingRect().height()/2, scene->sceneRect().height()-1);

    scene->addItem(top);
    scene->addItem(bottom);
    scene->addItem(left);
    scene->addItem(right);

    /**
     * Add text
     */
    int lineSpacing = 0;
    int leftSpacing = 0;
    int topSpacing = 0;
    titlesFont = new QFont("Helvetica", 14);
    textFont = new QFont("Helvetica", 12);
    lineSpacing = 8;

    /**
     * Hole size information
     */
    leftSpacing = 10;
    topSpacing = 5;
    holeSizeTitle = new QGraphicsTextItem();
    holeSizeTitle->setFont(*titlesFont);
    holeSizeTitle->setPos(leftSpacing, topSpacing);
    holeSizeTitle->setHtml("Hole Size:");

    holeDXText = new QGraphicsTextItem();
    holeDXText->setFont(*textFont);
    holeDXText->setPos(leftSpacing, holeSizeTitle->pos().y() + holeSizeTitle->font().pointSize() + lineSpacing + 5);
    holeDXText->setHtml(HoleDXString(29.999));

    holeDYText = new QGraphicsTextItem();
    holeDYText->setFont(*textFont);
    holeDYText->setPos(leftSpacing, holeDXText->pos().y() + holeSizeTitle->font().pointSize() + lineSpacing);
    holeDYText->setHtml(HoleDYString(29.999));

    scene->addItem(holeSizeTitle);
    scene->addItem(holeDXText);
    scene->addItem(holeDYText);

    /**
     * Busy state indicator
     */
    waitingGif = new QMovie(":/31.gif");
    waitingIndicator = new QLabel;
//    proxyGif = new QGraphicsProxyWidget;
    waitingGif->start();
    waitingIndicator->setAttribute(Qt::WA_NoSystemBackground);
    waitingIndicator->setMovie(waitingGif);

    proxyGif = scene->addWidget(waitingIndicator);
    proxyGif->setPos(30, 100);
    scene->removeItem(proxyGif);

//    IndicateWaitingState();

    /**
     * Hole offset information
     */
    leftSpacing = scene->sceneRect().width() - 140;
    topSpacing = 5;
    holeOffsetTitle = new QGraphicsTextItem();
    holeOffsetTitle->setFont(*titlesFont);
    holeOffsetTitle->setPos(leftSpacing, topSpacing);
    holeOffsetTitle->setHtml("Hole Offset:");

    holeOffsetXText = new QGraphicsTextItem();
    holeOffsetXText->setFont(*textFont);
    holeOffsetXText->setPos(leftSpacing, holeOffsetTitle->pos().y() + holeSizeTitle->font().pointSize() + lineSpacing + 5);
    holeOffsetXText->setHtml(HoleOffsetXString(0));

    holeOffsetYText = new QGraphicsTextItem();
    holeOffsetYText->setFont(*textFont);
    holeOffsetYText->setPos(leftSpacing, holeOffsetXText->pos().y() + holeSizeTitle->font().pointSize() + lineSpacing);
    holeOffsetYText->setHtml(HoleOffsetYString(0));

    scene->addItem(holeOffsetTitle);
    scene->addItem(holeOffsetXText);
    scene->addItem(holeOffsetYText);

    /**
     * Maximum collimator opening information
     */
    leftSpacing = 10;
    topSpacing = scene->sceneRect().height() - 100;
    maxOpeningTitle1 = new QGraphicsTextItem();
    maxOpeningTitle1->setFont(*titlesFont);
    maxOpeningTitle1->setPos(leftSpacing, topSpacing);
    maxOpeningTitle1->setHtml("Maximum collimator");

    maxOpeningTitle2 = new QGraphicsTextItem();
    maxOpeningTitle2->setFont(*titlesFont);
    maxOpeningTitle2->setPos(leftSpacing, maxOpeningTitle1->pos().y() + holeSizeTitle->font().pointSize() + lineSpacing);
    maxOpeningTitle2->setHtml("opening:");

    maxOpeningXText = new QGraphicsTextItem();
    maxOpeningXText->setFont(*textFont);
    maxOpeningXText->setPos(leftSpacing, maxOpeningTitle2->pos().y() + holeSizeTitle->font().pointSize() + lineSpacing + 5);
    maxOpeningXText->setHtml(MaxOpeningXString(20.0));

    maxOpeningYText = new QGraphicsTextItem();
    maxOpeningYText->setFont(*textFont);
    maxOpeningYText->setPos(leftSpacing, maxOpeningXText->pos().y() + holeSizeTitle->font().pointSize() + lineSpacing);
    maxOpeningYText->setHtml(MaxOpeningYString(20.0));

    scene->addItem(maxOpeningTitle1);
    scene->addItem(maxOpeningTitle2);
    scene->addItem(maxOpeningXText);
    scene->addItem(maxOpeningYText);

    /**
     * Collimator positions
     */
    leftSpacing = scene->sceneRect().width() - 150;
    topSpacing = scene->sceneRect().height() - 150;
    positionsTitle1 = new QGraphicsTextItem();
    positionsTitle1->setFont(*titlesFont);
    positionsTitle1->setPos(leftSpacing, topSpacing);
    positionsTitle1->setHtml("Collimator");

    positionsTitle2 = new QGraphicsTextItem();
    positionsTitle2->setFont(*titlesFont);
    positionsTitle2->setPos(leftSpacing, positionsTitle1->pos().y() + positionsTitle1->font().pointSize() + lineSpacing);
    positionsTitle2->setHtml("positions:");

    positionRightText = new QGraphicsTextItem();
    positionRightText->setFont(*textFont);
    positionRightText->setPos(leftSpacing, positionsTitle2->pos().y() + positionsTitle2->font().pointSize() + lineSpacing + 5);
    positionRightText->setPlainText(PositionRightString(10.0));

    positionLeftText = new QGraphicsTextItem();
    positionLeftText->setFont(*textFont);
    positionLeftText->setPos(leftSpacing, positionRightText->pos().y() + positionsTitle2->font().pointSize() + lineSpacing);
    positionLeftText->setPlainText(PositionLeftString(10.0));

    positionTopText = new QGraphicsTextItem();
    positionTopText->setFont(*textFont);
    positionTopText->setPos(leftSpacing, positionLeftText->pos().y() + positionsTitle2->font().pointSize() + lineSpacing);
    positionTopText->setPlainText(PositionTopString(10.0));

    positionBottomText = new QGraphicsTextItem();
    positionBottomText->setFont(*textFont);
    positionBottomText->setPos(leftSpacing, positionTopText->pos().y() + positionsTitle2->font().pointSize() + lineSpacing);
    positionBottomText->setPlainText(PositionBottomString(10.0));

    scene->addItem(positionsTitle1);
    scene->addItem(positionsTitle2);
    scene->addItem(positionRightText);
    scene->addItem(positionLeftText);
    scene->addItem(positionTopText);
    scene->addItem(positionBottomText);

    /**
     * connect internal (view) signals and slots
     */
    connect(right, SIGNAL(MoveMe()), this, SLOT(MoveRightRequested()));
    connect(left, SIGNAL(MoveMe()), this, SLOT(MoveLeftRequested()));
    connect(top, SIGNAL(MoveMe()), this, SLOT(MoveTopRequested()));
    connect(bottom, SIGNAL(MoveMe()), this, SLOT(MoveBottomRequested()));

    connect(right, SIGNAL(ResetMe()), this, SLOT(ResetRightRequested()));
    connect(left, SIGNAL(ResetMe()), this, SLOT(ResetLeftRequested()));
    connect(top, SIGNAL(ResetMe()), this, SLOT(ResetTopRequested()));
    connect(bottom, SIGNAL(ResetMe()), this, SLOT(ResetBottomRequested()));

    connect(right, SIGNAL(UpdateMe()), this, SLOT(UpdateRightRequested()));
    connect(left, SIGNAL(UpdateMe()), this, SLOT(UpdateLeftRequested()));
    connect(top, SIGNAL(UpdateMe()), this, SLOT(UpdateTopRequested()));
    connect(bottom, SIGNAL(UpdateMe()), this, SLOT(UpdateBottomRequested()));

    connect(right, SIGNAL(CloseMe()), this, SLOT(CloseHorizontal()));
    connect(left, SIGNAL(CloseMe()), this, SLOT(CloseHorizontal()));
    connect(top, SIGNAL(CloseMe()), this, SLOT(CloseVertical()));
    connect(bottom, SIGNAL(CloseMe()), this, SLOT(CloseVertical()));
}

void MainWindow::ConnectUIActions()
{
    connect(ui->MoveButton, SIGNAL(clicked()), this, SLOT(MoveCollimatorHandler()));
    connect(ui->coordLineEdit, SIGNAL(returnPressed()), this, SLOT(MoveCollimatorHandler()));
    connect(ui->ResetButton, SIGNAL(clicked()), this, SLOT(ResetCollimatorHandler()));
}

void MainWindow::ConnectCollimatorsRequests()
{
    connect(this, SIGNAL(ResetCollimator(int,int)), this, SLOT(SetWaitingState()));
    connect(this, SIGNAL(MoveCollimator(int,int,QString)), this, SLOT(SetWaitingState()));
    connect(this, SIGNAL(UpdateCollimator(int,int)), this, SLOT(SetWaitingState()));
    connect(this, SIGNAL(CloseCollimators(int,int)), this, SLOT(SetWaitingState()));
}

int MainWindow::GetActiveCollimatorBox()
{
    int boxID = BOX_ENTRANCE;
    if (ui->entranceRadio->isChecked()) {
        boxID = BOX_ENTRANCE;
    } else if (ui->exit1Radio->isChecked()) {
        boxID = BOX_EXIT1;
    } else if (ui->exit2Radio->isChecked()) {
        boxID = BOX_EXIT2;
    }
    return boxID;
}

QString MainWindow::Coord2String(float x)
{
    int digitsAfterPoint = 3;
    int zerosToAppend = 0;
    QString res = QString::number(x);

    int pointIndex = res.indexOf(".");
    if (pointIndex == -1) {
        res += ".";
        zerosToAppend = digitsAfterPoint;
    } else if (res.length() - pointIndex - 1 > digitsAfterPoint) {
        // number is too long, cut last digits
        res = res.mid(0, pointIndex+1+digitsAfterPoint);
        zerosToAppend = 0;
    } else {
        zerosToAppend = digitsAfterPoint+1 - (res.length() - pointIndex);
    }

    for(int i=0; i<zerosToAppend; i++)
        res += "0";

    return res;
}

QString MainWindow::HoleDXString(float dx)
{
    return QString(QChar(0x94, 0x03)) + "X = " + Coord2String(dx) + " mm";
}

QString MainWindow::HoleDYString(float dy)
{
    return QString(QChar(0x94, 0x03)) + "Y = " + Coord2String(dy) + " mm";
}

QString MainWindow::HoleOffsetXString(float x)
{
    return QString(QChar(0x94, 0x03)) + "X<sub>0</sub> = " + Coord2String(x) + " mm";
}

QString MainWindow::HoleOffsetYString(float y)
{
    return QString(QChar(0x94, 0x03)) + "Y<sub>0</sub> = " + Coord2String(y) + " mm";
}

QString MainWindow::MaxOpeningXString(float dx)
{
    return QString(QChar(0x94, 0x03)) + "X<sub>max</sub> = " + Coord2String(dx) + " mm";
}

QString MainWindow::MaxOpeningYString(float dy)
{
    return QString(QChar(0x94, 0x03)) + "Y<sub>max</sub> = " + Coord2String(dy) + " mm";
}

QString MainWindow::PositionRightString(float pos)
{
    return QString("Right: ") + Coord2String(pos) + " mm";
}

QString MainWindow::PositionLeftString(float pos)
{
    return QString("Left: ") + Coord2String(pos) + " mm";
}

QString MainWindow::PositionTopString(float pos)
{
    return QString("Top: ") + Coord2String(pos) + " mm";
}

QString MainWindow::PositionBottomString(float pos)
{
    return QString("Bottom: ") + Coord2String(pos) + " mm";
}

int MainWindow::GetActiveCollimator()
{
    int collimatorID = COLL_TOP;
    if (ui->rightCollimator->isChecked()) {
        collimatorID = COLL_RIGHT;
    } else if (ui->leftCollimator->isChecked()) {
        collimatorID = COLL_LEFT;
    } else if (ui->topCollimator->isChecked()) {
        collimatorID = COLL_TOP;
    } else if (ui->bottomCollimator->isChecked()) {
        collimatorID = COLL_BOTTOM;
    }
    return collimatorID;
}

void MainWindow::NiceMove(CollimatorGraphicsItem *collimator, QPointF to)
{
    QPointF from = collimator->pos();

    if (animationTimer)
        delete animationTimer;
    animationTimer = new QTimeLine(300);
    animationTimer->setFrameRange(0, 300);
    animationTimer->setUpdateInterval(30);

    if (animation)
        delete animation;
    animation = new QGraphicsItemAnimation;
    animation->setItem(collimator);
    animation->setTimeLine(animationTimer);

    animation->setPosAt(0., from);
    animation->setPosAt(1., to);

    animationTimer->start();
    while (animationTimer->state() == QTimeLine::Running) {
        qApp->processEvents();
    }

    // update numbers on the scene
    holeDXText->setHtml(HoleDXString(CalculateHoleDX()));
    holeDYText->setHtml(HoleDYString(CalculateHoleDY()));
    holeOffsetXText->setHtml(HoleOffsetXString(CalculateHoleOffsetX()));
    holeOffsetYText->setHtml(HoleOffsetYString(CalculateHoleOffsetY()));
    positionRightText->setPlainText(PositionRightString(posRight));
    positionLeftText->setPlainText(PositionLeftString(posLeft));
    positionTopText->setPlainText(PositionTopString(posTop));
    positionBottomText->setPlainText(PositionBottomString(posBottom));
}

void MainWindow::FinishWaitingState()
{
    // remove waiting indicator from the scene
    if (scene->items().contains(proxyGif)) {
        scene->removeItem(proxyGif);
    }
    // change the state
    applicationState = READY;
    emit ImReady();
}

void MainWindow::WaitForReady()
{
    forever {
        if (applicationState == READY)
            break;
        qApp->processEvents();
    }
}

void MainWindow::CollimatorBoxChanged()
{
//    int boxID = ChooseCollimatorSet();
//    for (int collimatorID = 0; collimatorID < 4; ++collimatorID) {
//        emit UpdateCollimator(boxID, collimatorID);
//    }
}

void MainWindow::SetWaitingState()
{
    // indicate waiting on the scene
    scene->addItem(proxyGif);
    // set the state
    applicationState = WAITING_FOR_COLLIMATORS_RESPONSE;
}

void MainWindow::Update()
{

}

void MainWindow::UpdateCoordinate(int collimatorBox, int collimatorID, float position)
{
    if (position < 0 || position > 15.)
        return;

    int originX = 0;
    int originY = 0;
    int destX = 0;
    int destY = 0;

    switch (collimatorID) {
    case COLL_RIGHT:
        posRight = position;

        originX = scene->sceneRect().width()-1;
        originY = scene->sceneRect().height()/2 + right->boundingRect().height()/2;

        destX = originX - int(position/(maxOpeningX/2) * (scene->sceneRect().width()/2 - right->boundingRect().width()));
        destY = originY;
        NiceMove(right, QPointF(destX, destY));
        break;

    case COLL_LEFT:
        posLeft = position;

        originX = 0;
        originY = scene->sceneRect().height()/2 - left->boundingRect().height()/2;

        destX = originX + int(position/(maxOpeningX/2) * (scene->sceneRect().width()/2 - left->boundingRect().width()));
        destY = originY;
        NiceMove(left, QPointF(destX, destY));
        break;

    case COLL_TOP:
        posTop = position;

        originX = scene->sceneRect().width()/2 + top->boundingRect().height()/2;
        originY = 0;

        destX = originX;
        destY = originY + int(position/(maxOpeningY/2) * (scene->sceneRect().height()/2 - top->boundingRect().width()));
        NiceMove(top, QPointF(destX, destY));
        break;

    case COLL_BOTTOM:
        posBottom = position;

        originX = scene->sceneRect().width()/2 - bottom->boundingRect().height()/2;
        originY = scene->sceneRect().height()-1;

        destX = originX;
        destY = originY - int(position/(maxOpeningY/2) * (scene->sceneRect().height()/2 - bottom->boundingRect().width()));
        NiceMove(bottom, QPointF(destX, destY));
        break;

    default:
        break;
    }
    FinishWaitingState();
}

void MainWindow::CreateTopMenuActions()
{
    setPulsesAction = new QAction(tr("&PWM pulses"), this);
    setPulsesAction->setStatusTip(tr("Set parameters of PWM pulses sent to piezomotors"));
    connect(setPulsesAction, SIGNAL(triggered()), this, SLOT(SetPWM()));
}

void MainWindow::CreateTopMenu()
{
    CreateTopMenuActions();
    settingsMenu = menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(setPulsesAction);
}

void MainWindow::SetMaxOpeningX(int collimatorBox, float opening)
{
    if (collimatorBox == GetActiveCollimatorBox()) {
        maxOpeningX = opening;
        maxOpeningXText->setHtml(MaxOpeningXString(maxOpeningX));
    }
}

void MainWindow::SetMaxOpeningY(int collimatorBox, float opening)
{
    if (collimatorBox == GetActiveCollimatorBox()) {
        maxOpeningY = opening;
        maxOpeningYText->setHtml(MaxOpeningYString(maxOpeningY));
    }
}

float MainWindow::CalculateHoleDX()
{
    return maxOpeningX - posLeft - posRight;
}

float MainWindow::CalculateHoleDY()
{
    return maxOpeningY - posTop - posBottom;
}

float MainWindow::CalculateHoleOffsetX()
{
    return (posLeft - posRight)/2.;
}

float MainWindow::CalculateHoleOffsetY()
{
    return (posBottom - posTop)/2.;
}

void MainWindow::MoveRightRequested()
{
    ui->rightCollimator->setChecked(true);
    ui->coordLineEdit->selectAll();
    ui->coordLineEdit->setFocus();
}

void MainWindow::MoveLeftRequested()
{
    ui->leftCollimator->setChecked(true);
    ui->coordLineEdit->selectAll();
    ui->coordLineEdit->setFocus();
}

void MainWindow::MoveTopRequested()
{
    ui->topCollimator->setChecked(true);
    ui->coordLineEdit->selectAll();
    ui->coordLineEdit->setFocus();
}

void MainWindow::MoveBottomRequested()
{
    ui->bottomCollimator->setChecked(true);
    ui->coordLineEdit->selectAll();
    ui->coordLineEdit->setFocus();
}

void MainWindow::ResetRightRequested()
{
    int collimatorBox = GetActiveCollimatorBox();
    emit ResetCollimator(collimatorBox, COLL_RIGHT);
}

void MainWindow::ResetLeftRequested()
{
    int collimatorBox = GetActiveCollimatorBox();
    emit ResetCollimator(collimatorBox, COLL_LEFT);
}

void MainWindow::ResetTopRequested()
{
    int collimatorBox = GetActiveCollimatorBox();
    emit ResetCollimator(collimatorBox, COLL_TOP);
}

void MainWindow::ResetBottomRequested()
{
    int collimatorBox = GetActiveCollimatorBox();
    emit ResetCollimator(collimatorBox, COLL_BOTTOM);
}

void MainWindow::UpdateRightRequested()
{
    int collimatorBox = GetActiveCollimatorBox();
    emit UpdateCollimator(collimatorBox, COLL_RIGHT);
}

void MainWindow::UpdateLeftRequested()
{
    int collimatorBox = GetActiveCollimatorBox();
    emit UpdateCollimator(collimatorBox, COLL_LEFT);
}

void MainWindow::UpdateTopRequested()
{
    int collimatorBox = GetActiveCollimatorBox();
    emit UpdateCollimator(collimatorBox, COLL_TOP);
}

void MainWindow::UpdateBottomRequested()
{
    int collimatorBox = GetActiveCollimatorBox();
    emit UpdateCollimator(collimatorBox, COLL_BOTTOM);
}

void MainWindow::CloseHorizontal()
{
    int collimatorBox = GetActiveCollimatorBox();
//    switch (collimatorBox) {
//    case BOX_ENTRANCE:
//        emit MoveCollimator(collimatorBox, COLL_LEFT, "12.000");
//        WaitForReady();

//        emit MoveCollimator(collimatorBox, COLL_RIGHT, "12.000");
//        WaitForReady();

//        emit UpdateCollimator(collimatorBox, COLL_LEFT);
//        WaitForReady();
//        break;
//    case BOX_EXIT1:
//        break;
//    case BOX_EXIT2:
//        break;
//    }
    emit CloseCollimators(collimatorBox, COLL_RIGHT);
}

void MainWindow::CloseVertical()
{
    int collimatorBox = GetActiveCollimatorBox();
//    switch (collimatorBox) {
//    case BOX_ENTRANCE:
//        emit MoveCollimator(collimatorBox, COLL_TOP, "12.000");
//        WaitForReady();

//        emit MoveCollimator(collimatorBox, COLL_BOTTOM, "12.000");
//        WaitForReady();

//        emit UpdateCollimator(collimatorBox, COLL_TOP);
//        WaitForReady();
//        break;
//    case BOX_EXIT1:
//        break;
//    case BOX_EXIT2:
//        break;
//    }
    emit CloseCollimators(collimatorBox, COLL_TOP);
}

void MainWindow::MoveCollimatorHandler()
{
    int collimatorBox = GetActiveCollimatorBox();
    int collimatorID = GetActiveCollimator();
    QString coordinate = ui->coordLineEdit->text();
    emit MoveCollimator(collimatorBox, collimatorID, coordinate);
}

void MainWindow::ResetCollimatorHandler()
{
    int collimatorBox = GetActiveCollimatorBox();
    int collimatorID = GetActiveCollimator();
    emit ResetCollimator(collimatorBox, collimatorID);
}

void MainWindow::ResetAllCollimatorHandler()
{
    int collimatorBox = GetActiveCollimatorBox();
    for (int collimatorID = 0; collimatorID < N_COLLIMATORS; collimatorID++) {
        emit ResetCollimator(collimatorBox, collimatorID);
        WaitForReady();
    }
//    int collimatorBox = GetActiveCollimatorBox();
//    emit ResetAll();
//    WaitForReady();
}

void MainWindow::UpdateCollimatorHandler()
{
    int collimatorBox = GetActiveCollimatorBox();
    int collimatorID = GetActiveCollimator();
    emit UpdateCollimator(collimatorBox, collimatorID);
}

void MainWindow::SetPWM()
{

}

void MainWindow::CollimatorsConnected()
{
    ui->ConnectLabel->setText("Connected");
    ui->ConnectButton->setText("Disconnect");
}

void MainWindow::CollimatorsDisconnected()
{
    ui->ConnectLabel->setText("Disconnected");
    ui->ConnectButton->setText("Connect");
}

void MainWindow::on_ConnectButton_clicked()
{
    if (ui->ConnectButton->text() == "Connect") {
        emit ConnectCollimatiors(ui->IPLineEdit->text());
    } else if (ui->ConnectButton->text() == "Disconnect") {
        emit DisconnectCollimatiors();
    }
}

void MainWindow::on_ResetAllButton_clicked()
{
    ResetAllCollimatorHandler();
}

void MainWindow::on_holeSizeHorizontal_returnPressed()
{

}

void MainWindow::on_holeSizeVertical_returnPressed()
{

}

void MainWindow::on_centerHorizontalOffet_returnPressed()
{

}

void MainWindow::on_centerVerticalOffet_returnPressed()
{

}

void MainWindow::on_PulsePeriodBox_valueChanged(const QString &arg1)
{
    float T = usPerTimerTick * arg1.toInt();
    ui->PeriodLine_us->setText("= " + QString::number(T) + " us");
}

void MainWindow::on_pushButton_clicked()
{
    int collimatorBox = GetActiveCollimatorBox();
    int collimatorID = GetActiveCollimator();
    QString T = ui->PulsePeriodBox->text();
    emit SetPWMPeriod(collimatorBox, collimatorID, T);
}
