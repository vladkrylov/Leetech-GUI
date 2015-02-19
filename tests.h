#ifndef TESTS_H
#define TESTS_H

#include <QObject>
#include <QTest>
#include <QTimer>

#include "controller.h"

class Controller;
class IP_Connection;

class Tests : public QObject
{
    Q_OBJECT
public:
    IP_Connection *PCB;
    Controller *control;

    explicit Tests(Controller *c, IP_Connection *testPCB, QObject *parent = 0);

    void Test(int setID, int motorID);
    void TestPulsesForOscilloscope();
    void TestForce(int width, int begin, int end, int setID, int motorID);
    void StopForseTest();
    void CollectData(int setID, int motorID, QString period);

private:
    int stopForseTest;
    void Delay(int t);

    void WaitForMotorData();

signals:

public slots:
    void PrintBoardResponse();
};

#endif // TESTS_H
