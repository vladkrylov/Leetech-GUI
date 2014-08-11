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
    hardware = new Controller();
    SetValidators();
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

void MainWindow::on_goButton_1_clicked()
{
    ui->goButton_1->setEnabled(false);
    hardware->SetMotorCoordinate(2, ui->dist_1m->text(),
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
    hardware->SetMotorCoordinate(0, ui->dist_3m->text(),
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

