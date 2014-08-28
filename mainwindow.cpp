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
}

MainWindow::~MainWindow()
{
    delete ui;
    delete hardware;
}

void MainWindow::SetValidators()
{
    int min_um = 0;
    int max_um = 999;
    ui->dist_1->setValidator( new QIntValidator(min_um, max_um, this) );
    ui->dist_2->setValidator( new QIntValidator(min_um, max_um, this) );
    ui->dist_3->setValidator( new QIntValidator(min_um, max_um, this) );
    ui->dist_4->setValidator( new QIntValidator(min_um, max_um, this) );

    int min_mm = 0;
    int max_mm = 14;
    ui->dist_1m->setValidator( new QIntValidator(min_mm, max_mm, this) );
    ui->dist_2m->setValidator( new QIntValidator(min_mm, max_mm, this) );
    ui->dist_3m->setValidator( new QIntValidator(min_mm, max_mm, this) );
    ui->dist_4m->setValidator( new QIntValidator(min_mm, max_mm, this) );
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

void MainWindow::on_goButton_1_clicked()
{
    ui->goButton_1->setEnabled(false);
    hardware->SetMotorCoordinate(0, ui->dist_1m->text(),
                                    ui->dist_1->text());
    ui->goButton_1->setEnabled(true);
}

void MainWindow::on_goButton_2_clicked()
{
    ui->goButton_2->setEnabled(false);
    hardware->SetMotorCoordinate(1, ui->dist_2m->text(),
                                    ui->dist_2->text());
    ui->goButton_2->setEnabled(true);
}

void MainWindow::on_goButton_3_clicked()
{
    ui->goButton_3->setEnabled(false);
    hardware->SetMotorCoordinate(2, ui->dist_3m->text(),
                                    ui->dist_3->text());
    ui->goButton_3->setEnabled(true);
}

void MainWindow::on_goButton_4_clicked()
{
    ui->goButton_4->setEnabled(false);
    hardware->SetMotorCoordinate(3, ui->dist_4m->text(),
                                    ui->dist_4->text());
    ui->goButton_4->setEnabled(true);
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


void MainWindow::on_pushButton_clicked()
{
    hardware->ResetMotorsData();
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
