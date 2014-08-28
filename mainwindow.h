#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QString>
#include <QtWidgets>

#include "controller.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_goButton_1_clicked();

    void on_ConnectButton_clicked();

    void on_pushButton_clicked();

    void on_goButton_2_clicked();

    void on_goButton_3_clicked();

    void on_goButton_4_clicked();

    void on_MotorReset_1_clicked();

    void on_MotorReset_2_clicked();

    void on_MotorReset_3_clicked();

    void on_MotorReset_4_clicked();

    void on_MotorReset_All_clicked();

    void on_TestButton_clicked();

    void on_TestButton_2_clicked();

    void on_PulsesButton_clicked();

    void on_PeriodSpinBox_valueChanged(const QString &arg1);

    void on_WidthSpinBox_valueChanged(const QString &arg1);

    void on_TestForceButton_clicked();

    void on_StopForceTestButton_clicked();

    void on_GetCoordinateButton_clicked();

    void on_LWIP_bug_clicked();

private:
    Ui::MainWindow *ui;
    Controller *hardware;

    void SetValidators();
    int ValidatePulsesWidth(float width_us);
    int ValidatePulsesPeriod(float period_us);
};

#endif // MAINWINDOW_H
