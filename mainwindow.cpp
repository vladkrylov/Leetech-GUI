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

    connect(hardware, SIGNAL(Motor1CoordinateChanged(uint16_t)), this, SLOT(UpdateMotor1(uint16_t)));
    connect(hardware, SIGNAL(Motor2CoordinateChanged(uint16_t)), this, SLOT(UpdateMotor2(uint16_t)));
    connect(hardware, SIGNAL(Motor3CoordinateChanged(uint16_t)), this, SLOT(UpdateMotor3(uint16_t)));
    connect(hardware, SIGNAL(Motor4CoordinateChanged(uint16_t)), this, SLOT(UpdateMotor4(uint16_t)));
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

void MainWindow::on_GoButton_clicked()
{
    int motorID = ChooseMotor();
    QString crd = ui->CoordinateLineEdit->text();
    hardware->SetMotorCoordinate(motorID, crd);
}

void MainWindow::on_ConnectButton_clicked()
{
    ui->ConnectLabel->setText("   Connecting...   ");
    ui->ConnectButton->setEnabled(false);

    if (!(hardware->IsConnected())) {
        hardware->SetIPAddress(ui->IP_lineEdit->text());
        if (hardware->Connect()) {
            ui->ConnectLabel->setText("   Connected   ");
            ui->ConnectButton->setText("Disconnect");
        } else {
            ui->ConnectLabel->setText("   Disconnected   ");
        }

    } else {
        hardware->Disconnect();
        ui->ConnectLabel->setText("   Disconnected   ");
        ui->ConnectButton->setText("Connect");
    }
    ui->ConnectButton->setEnabled(true);
}

void MainWindow::on_MotorReset_1_clicked()
{
    hardware->Reset(0);
}

void MainWindow::on_MotorReset_2_clicked()
{
    hardware->Reset(1);
}

void MainWindow::on_MotorReset_3_clicked()
{
    hardware->Reset(2);
}

void MainWindow::on_MotorReset_4_clicked()
{
    hardware->Reset(3);
}

void MainWindow::on_MotorReset_All_clicked()
{
    hardware->ResetAll();
}

void MainWindow::on_TestButton_clicked()
{
    int motorID = 0;
    if (ui->Motor1_radioButton->isChecked()) {
        motorID = 0;
    } else if (ui->Motor2_radioButton->isChecked()) {
        motorID = 1;
    } else if (ui->Motor3_radioButton->isChecked()) {
        motorID = 2;
    } else if (ui->Motor4_radioButton->isChecked()) {
        motorID = 3;
    }

    hardware->TestObject->Test(motorID);
}


void MainWindow::on_TestButton_2_clicked()
{
    hardware->TestObject->TestPulsesForOscilloscope();
}

void MainWindow::on_PulsesButton_clicked()
{
    hardware->SetPulses(ui->WidthSpinBox->text(),
              ui->PeriodSpinBox->text());
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

void MainWindow::on_TestForceButton_clicked()
{
    int motorID = 0;
    if (ui->Motor1_radioButton->isChecked()) {
        motorID = 0;
    } else if (ui->Motor2_radioButton->isChecked()) {
        motorID = 1;
    } else if (ui->Motor3_radioButton->isChecked()) {
        motorID = 2;
    } else if (ui->Motor4_radioButton->isChecked()) {
        motorID = 3;
    }

    int width = ui->WidthSpinBox->text().toInt();
    int begin = ui->TestBeginField->text().toInt();
    int end = ui->TestEndField->text().toInt();

    hardware->TestObject->TestForce(width, begin, end, motorID);
}

void MainWindow::on_StopForceTestButton_clicked()
{
    hardware->TestObject->StopForseTest();
}

void MainWindow::on_GetCoordinateButton_clicked()
{
    int motorID = 0;
    if (ui->Motor1_radioButton->isChecked()) {
        motorID = 0;
    } else if (ui->Motor2_radioButton->isChecked()) {
        motorID = 1;
    } else if (ui->Motor3_radioButton->isChecked()) {
        motorID = 2;
    } else if (ui->Motor4_radioButton->isChecked()) {
        motorID = 3;
    }

    hardware->GetMotorCoordinate(motorID);
}

void MainWindow::on_LWIP_bug_clicked()
{
    hardware->TestObject->TestLWIP(100);
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
    int end;
    if (textCoord.length() == 1) {
        end = 1;
    } else {
        end = textCoord.length() - 1;
    }
    return textCoord.mid( 0, end );
}

void MainWindow::UpdateMotor1(uint16_t c)
{
    ui->DisplayCoordinate1->setText(CoordToShow(c));
}

void MainWindow::UpdateMotor2(uint16_t c)
{
    ui->DisplayCoordinate2->setText(CoordToShow(c));
}

void MainWindow::UpdateMotor3(uint16_t c)
{
    ui->DisplayCoordinate3->setText(CoordToShow(c));
}

void MainWindow::UpdateMotor4(uint16_t c)
{
    ui->DisplayCoordinate4->setText(CoordToShow(c));
}

void MainWindow::on_ResetOnePushButton_clicked()
{
    int motorID = ChooseMotor();
    hardware->Reset(motorID);
}

void MainWindow::on_ResetAllPushButton_clicked()
{
    hardware->ResetAll();
}
