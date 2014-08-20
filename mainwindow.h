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

private:
    Ui::MainWindow *ui;
    Controller *hardware;

    void SetValidators();
};

#endif // MAINWINDOW_H
