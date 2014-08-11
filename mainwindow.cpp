#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SetValidators();

    data_to_send.clear();
    PCB = new IP_Connection();
    motor1 = new Encoder();
    motor2 = new Encoder();
    motor3 = new Encoder();
    motor4 = new Encoder();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete PCB;
    delete motor1;
    delete motor2;
    delete motor3;
    delete motor4;
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

void MainWindow::on_goButton_1_clicked()
{
    QByteArray response;
    ui->goButton_1->setEnabled(false);

    data_to_send = "move_motor3_"
            + GenerateCoordinate(ui->dist_1m->text(),
                                 ui->dist_1->text()
                                 )
            + "f"
            + "_steps2mm=" + QString::number(motor1->GetSteps2mm());

    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;

    data_to_send = "Get_coordinate";
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;
    motor1->Update(response);
    qDebug() << motor1->GetPosition();

//    Calibrate(590,591,1000);
    ui->goButton_1->setEnabled(true);
}

void MainWindow::on_goButton_2_clicked()
{
    QByteArray response;
    ui->goButton_2->setEnabled(false);

    data_to_send = "move_motor2_"
            + GenerateCoordinate(ui->dist_2m->text(),
                                 ui->dist_2->text()
                                 )
            + "f"
            + "_steps2mm=" + QString::number(motor2->GetSteps2mm());

    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;

    data_to_send = "Get_coordinate";
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;
    motor2->Update(response);
    qDebug() << motor2->GetPosition();

//    Calibrate(590,591,1000);
    ui->goButton_2->setEnabled(true);
}

void MainWindow::on_goButton_3_clicked()
{
    QByteArray response;
    ui->goButton_3->setEnabled(false);

    data_to_send = "move_motor1_"
            + GenerateCoordinate(ui->dist_3m->text(),
                                 ui->dist_3->text()
                                 )
            + "f"
            + "_steps2mm=" + QString::number(motor3->GetSteps2mm());

    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;

    data_to_send = "Get_coordinate";
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;
    motor3->Update(response);
    qDebug() << motor3->GetPosition();

//    Calibrate(590,591,1000);
    ui->goButton_3->setEnabled(true);
}

void MainWindow::on_goButton_4_clicked()
{
    QByteArray response;
    ui->goButton_4->setEnabled(false);

    data_to_send = "move_motor4_"
            + GenerateCoordinate(ui->dist_4m->text(),
                                 ui->dist_4->text()
                                 )
            + "f"
            + "_steps2mm=" + QString::number(motor4->GetSteps2mm());

    qDebug() << data_to_send;
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;

    data_to_send = "Get_coordinate";
    PCB->PCB_SendData(data_to_send);
    response = PCB->PCB_ReceiveData();
//    qDebug() << response;
    motor4->Update(response);
    qDebug() << motor4->GetPosition();

//    Calibrate(590,591,1000);
    ui->goButton_4->setEnabled(true);
}

QString MainWindow::GenerateCoordinate(QString mm, QString um)
{
    QString res, mm_res, um_res;
    // mm_res processing
    if (mm.length() == 0) {
        mm_res = "00";
    } else if (mm.length() == 1) {
        mm_res = "0" + mm;
    } else {
        mm_res = mm;
    }

    // um_res processing
    if (um.length() == 0) {
        um_res = "000";
    } else if (um.length() == 1) {
        um_res = "00" + um;
    } else if (um.length() == 2) {
        um_res = "0" + um;
    } else {
        um_res = um;
    }

    res = mm_res + um_res;
    return res;
}

void MainWindow::on_ConnectButton_clicked()
{
    ui->ConnectLabel->setText("   Connecting...   ");
    ui->ConnectButton->setEnabled(false);

//    qDebug() << !(PCB->IsConnected());
    if (!(PCB->IsConnected())) {
        PCB->SetIPAddress(ui->IP_lineEdit->text());
        if (PCB->PCB_Connect()) {
            ui->ConnectLabel->setText("   Connected   ");
            ui->ConnectButton->setText("Disconnect");
        } else {
            ui->ConnectLabel->setText("   Disconnected   ");
        }

    } else {
        PCB->PCB_Disconnect();
        ui->ConnectLabel->setText("   Disconnected   ");
        ui->ConnectButton->setText("Connect");
    }
    ui->ConnectButton->setEnabled(true);
}

void MainWindow::Calibrate(int begin, int end, int gap)
{
    QByteArray response;
    QFile file("out.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    for (int i=begin; i<=end; i+=gap) {

        data_to_send = "move_motor1_" + GenerateCoordinate("", QString::number(i));
        PCB->PCB_SendData(data_to_send);
        response = PCB->PCB_ReceiveData();
        qDebug() << data_to_send;

        data_to_send = "Get_coordinate";
        PCB->PCB_SendData(data_to_send);
        response = PCB->PCB_ReceiveData();
        motor1->Update(response);
        qDebug() << motor1->GetPosition();

        out << i <<"    "<< motor1->GetPosition() << endl;
    }
    file.close();
}

void MainWindow::on_pushButton_clicked()
{
    motor1->ResetSteps2mm();
    motor2->ResetSteps2mm();
}

