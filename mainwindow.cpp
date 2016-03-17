#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "m_errors.h"

const float usPerTimerTick = 0.025;
const float MAX_PULSE_WIDTH_US = 4.5;
const float MAX_PULSE_PERIOD_US = 24.5;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isCollimatorsConnected(false)
{
    ui->setupUi(this);
//    hardware = new Controller();
    SetValidators();

    emit ui->PeriodSpinBox->valueChanged(ui->PeriodSpinBox->text());
    emit ui->WidthSpinBox->valueChanged(ui->WidthSpinBox->text());

    on_SelectEntranceRadio_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete coordValidator;
}

void MainWindow::SetValidators()
{
    coordValidator = new QDoubleValidator( 0, 14.999, 2, this );
    ui->CoordinateLineEdit->setValidator(coordValidator);

//    QDoubleValidator* magnetVoltageValidator = new QDoubleValidator(0, 18., 2, this);
//    ui->SetMagnetVoltageLine->setValidator(magnetVoltageValidator);

//    QDoubleValidator* magnetCurrentValidator = new QDoubleValidator(0, 50., 2, this);
//    ui->SetMagnetCurrentLine->setValidator(magnetCurrentValidator);
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
    emit SetMotorCoordinate(set, motor, crd);
//    hardware->SetMotorCoordinate(set, motor, crd);
}

void MainWindow::on_ConnectButton_clicked()
{
    if (isCollimatorsConnected) {
        emit CollimatorsDisconnect();
    } else {
        emit CollimatorsConnect();
    }
}

QString MainWindow::GetCollimatorsIPAddress()
{
    return ui->IP_lineEdit->text();
}

void MainWindow::CollimatorsConnected()
{
    ui->ConnectLabel->setText("   Connected   ");
    ui->ConnectButton->setText("Disconnect");
    isCollimatorsConnected = true;
}

void MainWindow::CollimatorsDisconnected()
{
    ui->ConnectLabel->setText("   Disconnected   ");
    ui->ConnectButton->setText("Connect");
    isCollimatorsConnected = false;
}

void MainWindow::on_TestButton_clicked()
{
//    hardware->CollimatorsDataReceived();
}

void MainWindow::on_PulsesButton_clicked()
{
//    hardware->SetPulses(ChooseCollimatorSet(),
//                        ChooseMotor(),
//                        ui->WidthSpinBox->text(),
//                        ui->PeriodSpinBox->text()
//                        );
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
    emit ResetMotor(set, motor);
}

void MainWindow::on_ResetAllPushButton_clicked()
{
//    hardware->ResetAll(ChooseCollimatorSet());
}

void MainWindow::on_UpdateCoordinatesButton_clicked()
{
    int set = ChooseCollimatorSet();
    int motor = ChooseMotor();

    UnknownCoordinate(set, motor);
    emit GetMotorCoordinate(set, motor);
}

void MainWindow::MotorCoordinateChanged(int setID, int motorID, uint16_t newCoordinate)
{
    QString coord;
    if (newCoordinate != COORD_ERROR) {
        coord = CoordToShow(newCoordinate);
    } else {
        coord = "No coordinate... Try resetting";
    }
    if (setID == ChooseCollimatorSet()) {
        if (motorID == 0) {
            ui->DisplayCoordinate1->setText(coord);
        } else if (motorID == 1) {
            ui->DisplayCoordinate2->setText(coord);
        } else if (motorID == 2) {
            ui->DisplayCoordinate3->setText(coord);
        } else if (motorID == 3) {
            ui->DisplayCoordinate4->setText(coord);
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
//        MotorCoordinateChanged(setID, motorID, hardware->ShowMotorCoordinate(setID, motorID));
    }
    qDebug() << "Entrance has been chosen";
}

void MainWindow::on_SelectExit1Radio_clicked()
{
    int setID = ChooseCollimatorSet();
    for (int motorID = 0; motorID < 4; ++motorID) {
//        MotorCoordinateChanged(setID, motorID, hardware->ShowMotorCoordinate(setID, motorID));
    }
    qDebug() << "Exit 1 has been chosen";
}

void MainWindow::on_MagnetConnectButton_clicked()
{
//    if (!(hardware->IsMagnetConnected())) {
//        hardware->SetMagnetIPAddress(ui->MagnetIPLine->text());
//        hardware->ConnectMagnet();
//    }
}

void MainWindow::MagnetConnected()
{
    ui->MagnetConnectLabel->setText("   Connected   ");
    ui->MagnetConnectButton->setEnabled(false);
}

void MainWindow::UpdateMagnetData(float u, float i)
{
    ui->DisplayMagnetVoltageLine->setText(QString::number(u));
    ui->DisplayMagnetCurrentLine->setText(QString::number(i));
}

void MainWindow::on_SetMagnetVoltageButton_clicked()
{
//    hardware->SetMagnetVoltage(ui->SetMagnetVoltageLine->text().toFloat());
}

void MainWindow::on_SetMagnetCurrentButton_clicked()
{
//    hardware->SetMagnetCurrent(ui->SetMagnetCurrentLine->text().toFloat());
}

void MainWindow::on_SetMagnetVoltageLine_textChanged(const QString &arg1)
{
    if (arg1.toFloat() < 0.) ui->SetMagnetVoltageLine->setText(QString::number(0.));
    if (arg1.toFloat() > 18.) ui->SetMagnetVoltageLine->setText(QString::number(18.));
}

void MainWindow::on_SetMagnetCurrentLine_textChanged(const QString &arg1)
{
    if (arg1.toFloat() < 0.) ui->SetMagnetCurrentLine->setText(QString::number(0.));
    if (arg1.toFloat() > 50.) ui->SetMagnetCurrentLine->setText(QString::number(50.));
}

void MainWindow::on_MagnetOnOffButton_clicked()
{
//    if (hardware->MagnetOutputStatus() == true) {
//        hardware->MagnetOutputOff();
//        ui->MagnetOnOffButton->setText("Output On");
//    } else {
//        hardware->MagnetOutputOn();
//        ui->MagnetOnOffButton->setText("Output Off");
//    }
}

