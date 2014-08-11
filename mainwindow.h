#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QString>
#include <QMainWindow>
#include "ip_connection.h"
#include "encoder.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Encoder *motor1;
    Encoder *motor2;
    Encoder *motor3;
    Encoder *motor4;

private slots:
    void on_goButton_1_clicked();

    void on_ConnectButton_clicked();

    void on_pushButton_clicked();

    void on_goButton_2_clicked();

    void on_goButton_3_clicked();

    void on_goButton_4_clicked();

private:
    QString data_to_send;
    IP_Connection *PCB;
    Ui::MainWindow *ui;

    void Calibrate(int, int, int);
    void SetValidators();
    QString GenerateCoordinate(QString, QString);
};

#endif // MAINWINDOW_H
