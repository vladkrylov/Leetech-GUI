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
    void on_ConnectButton_clicked();

    void on_TestButton_clicked();

    void on_TestButton_2_clicked();

    void on_PulsesButton_clicked();

    void on_PeriodSpinBox_valueChanged(const QString &arg1);

    void on_WidthSpinBox_valueChanged(const QString &arg1);

    void on_TestForceButton_clicked();

    void on_StopForceTestButton_clicked();

    void on_GetCoordinateButton_clicked();

    void on_LWIP_bug_clicked();

    void on_GoButton_clicked();

    void on_CoordinateLineEdit_textChanged(const QString &arg1);

    void UpdateMotor1(uint16_t);
    void UpdateMotor2(uint16_t);
    void UpdateMotor3(uint16_t);
    void UpdateMotor4(uint16_t);

    void on_ResetOnePushButton_clicked();

    void on_ResetAllPushButton_clicked();

    void on_UpdateCoordinatesButton_clicked();

private:
    Ui::MainWindow *ui;
    Controller *hardware;
    QDoubleValidator *coordValidator;

    void SetValidators();
    int ValidatePulsesWidth(float width_us);
    int ValidatePulsesPeriod(float period_us);
    int ChooseMotor();
    QString CoordToShow(uint16_t coordinate);
};

#endif // MAINWINDOW_H
