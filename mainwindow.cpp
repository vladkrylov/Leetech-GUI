#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>

const float usPerTimerTick = 0.025;
const float MAX_PULSE_WIDTH_US = 4.5;
const float MAX_PULSE_PERIOD_US = 24.5;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    hardware = new Controller();
    SetValidators();

    emit ui->PeriodSpinBox->valueChanged(ui->PeriodSpinBox->text());
    emit ui->WidthSpinBox->valueChanged(ui->WidthSpinBox->text());

    connect(hardware, SIGNAL(MotorCoordinateChanged(int,int,uint16_t)), this, SLOT(MotorCoordinateChanged(int,int,uint16_t)));
    connect(hardware, SIGNAL(Connected()), this, SLOT(Connected()));
    connect(hardware, SIGNAL(Disconnected()), this, SLOT(Disconnected()));
    on_SelectEntranceRadio_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete hardware;
    delete coordValidator;
}

void MainWindow::SetValidators()
{
    coordValidator = new QDoubleValidator( 0, 14.999, 2, this );
    ui->CoordinateLineEdit->setValidator(coordValidator);
}

int MainWindow::ValidatePulsesWidth(float width_us)
{
    if (width_us > MAX_PULSE_WIDTH_US) {
        return 0;
    }
    return 1;
}

int MainWindow::ValidatePulsesPeriod(float period_us)
{
    if (period_us > MAX_PULSE_PERIOD_US) {
        return 0;
    }
    return 1;
}

int MainWindow::ChooseMotor()
{
    int motorID = 0;
    if (ui->radioButton_1->isChecked()) {
        motorID = 0;
    } else if (ui->radioButton_2->isChecked()) {
        motorID = 1;
    } else if (ui->radioButton_3->isChecked()) {
        motorID = 2;
    } else if (ui->radioButton_4->isChecked()) {
        motorID = 3;
    }
    return motorID;
}

int MainWindow::ChooseCollimatorSet()
{
    int setID = 0;
    if (ui->SelectEntranceRadio->isChecked()) {
        setID = 0;
    } else if (ui->SelectExit1Radio->isChecked()) {
        setID = 1;
    } else if (ui->SelectExit2Radio->isChecked()) {
        setID = 2;
    }
    return setID;
}

void MainWindow::on_GoButton_clicked()
{
    int set = ChooseCollimatorSet();
    int motor = ChooseMotor();

    UnknownCoordinate(set, motor);
    QString crd = ui->CoordinateLineEdit->text();
    hardware->SetMotorCoordinate(set, motor, crd);
}

void MainWindow::on_ConnectButton_clicked()
{
//    ui->ConnectLabel->setText("   Connecting...   ");
//    ui->ConnectButton->setEnabled(false);

    if (!(hardware->IsConnected())) {
        hardware->SetIPAddress(ui->IP_lineEdit->text());
        hardware->Connect();
    } else {
        hardware->Disconnect();
    }
    ui->ConnectButton->setEnabled(true);
}

void MainWindow::Connected()
{
    ui->ConnectLabel->setText("   Connected   ");
    ui->ConnectButton->setText("Disconnect");
}

void MainWindow::Disconnected()
{
    ui->ConnectLabel->setText("   Disconnected   ");
    ui->ConnectButton->setText("Connect");
}

void MainWindow::on_TestButton_clicked()
{
//    hardware->TestObject->Test(ChooseCollimatorSet(), ChooseMotor());
//    hardware->TestObject->CollectData(ChooseCollimatorSet(), ChooseMotor(), ui->PeriodSpinBox->text());
    hardware->dataReceived();
}

void MainWindow::on_PulsesButton_clicked()
{
    hardware->SetPulses(ChooseCollimatorSet(),
                        ChooseMotor(),
                        ui->WidthSpinBox->text(),
                        ui->PeriodSpinBox->text()
                        );
}

void MainWindow::on_PeriodSpinBox_valueChanged(const QString &arg1)
{
    float t = usPerTimerTick * arg1.toInt();

    if (ValidatePulsesPeriod(t)) {
        ui->PulsesPeriodUS->setText("= " + QString::number(t) + " us");
    } else {
        ui->PeriodSpinBox->setValue(int(MAX_PULSE_PERIOD_US/usPerTimerTick));
        ui->PulsesPeriodUS->setText("= " + QString::number(MAX_PULSE_PERIOD_US) + " us");
    }
}

void MainWindow::on_WidthSpinBox_valueChanged(const QString &arg1)
{
    float w = usPerTimerTick * arg1.toInt();

    if (ValidatePulsesWidth(w)) {
        ui->PulsesWidthUS->setText("= " + QString::number(w) + " us");
    } else {
        ui->WidthSpinBox->setValue(int(MAX_PULSE_WIDTH_US/usPerTimerTick));
        ui->PulsesWidthUS->setText("= " + QString::number(MAX_PULSE_WIDTH_US) + " us");
    }
}

//void MainWindow::on_TestForceButton_clicked()
//{
//    int width = ui->WidthSpinBox->text().toInt();
//    int begin = ui->TestBeginField->text().toInt();
//    int end = ui->TestEndField->text().toInt();

//    hardware->TestObject->TestForce(width, begin, end, ChooseCollimatorSet(), ChooseMotor());
//}

//void MainWindow::on_StopForceTestButton_clicked()
//{
//    hardware->TestObject->StopForseTest();
//}

//void MainWindow::on_GetCoordinateButton_clicked()
//{
//    hardware->GetMotorCoordinate(ChooseCollimatorSet(), ChooseMotor());
//}

//void MainWindow::on_LWIP_bug_clicked()
//{
//    hardware->TestObject->TestLWIP(100);
//}

void MainWindow::on_CoordinateLineEdit_textChanged(const QString &arg1)
{
    if (arg1.toFloat() > 14.99) {
        ui->CoordinateLineEdit->setText("14,99");
    }
}

QString MainWindow::CoordToShow(uint16_t coordinate)
{
    QString textCoord = QString::number( coordinate/1000. );
    return textCoord/*.mid( 0, end )*/;
}

void MainWindow::on_ResetOnePushButton_clicked()
{
    int set = ChooseCollimatorSet();
    int motor = ChooseMotor();

    UnknownCoordinate(set, motor);
    hardware->Reset(set, motor);
}

void MainWindow::on_ResetAllPushButton_clicked()
{
    hardware->ResetAll(ChooseCollimatorSet());
}

void MainWindow::on_UpdateCoordinatesButton_clicked()
{
    int set = ChooseCollimatorSet();
    int motor = ChooseMotor();

    UnknownCoordinate(set, motor);
    hardware->GetMotorCoordinate(set, motor);
}

void MainWindow::MotorCoordinateChanged(int setID, int motorID, uint16_t newCoordinate)
{
    if (setID == ChooseCollimatorSet()) {
        if (motorID == 0) {
            ui->DisplayCoordinate1->setText(CoordToShow(newCoordinate));
        } else if (motorID == 1) {
            ui->DisplayCoordinate2->setText(CoordToShow(newCoordinate));
        } else if (motorID == 2) {
            ui->DisplayCoordinate3->setText(CoordToShow(newCoordinate));
        } else if (motorID == 3) {
            ui->DisplayCoordinate4->setText(CoordToShow(newCoordinate));
        }
    }
}

void MainWindow::UnknownCoordinate(int setID, int motorID)
{
    QString unkn = "-----";
    if (setID == ChooseCollimatorSet()) {
        if (motorID == 0) {
            ui->DisplayCoordinate1->setText(unkn);
        } else if (motorID == 1) {
            ui->DisplayCoordinate2->setText(unkn);
        } else if (motorID == 2) {
            ui->DisplayCoordinate3->setText(unkn);
        } else if (motorID == 3) {
            ui->DisplayCoordinate4->setText(unkn);
        }
    }
}

void MainWindow::on_SelectEntranceRadio_clicked()
{
    int setID = ChooseCollimatorSet();
    for (int motorID = 0; motorID < 4; ++motorID) {
        MotorCoordinateChanged(setID, motorID, hardware->ShowMotorCoordinate(setID, motorID));
    }
    qDebug() << "Entrance has been chosen";
}

void MainWindow::on_SelectExit1Radio_clicked()
{
    int setID = ChooseCollimatorSet();
    for (int motorID = 0; motorID < 4; ++motorID) {
        MotorCoordinateChanged(setID, motorID, hardware->ShowMotorCoordinate(setID, motorID));
    }
    qDebug() << "Exit 1 has been chosen";
}
