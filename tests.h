#ifndef TESTS_H
#define TESTS_H

#include <QObject>
#include "controller.h"

class Controller;

class Tests : public QObject
{
    Q_OBJECT
public:
    IP_Connection *PCB;
    Controller *control;

    explicit Tests(Controller *c, IP_Connection *testPCB, QObject *parent = 0);

    void Test(int motorID);
    void TestPulsesForOscilloscope();
    void TestForce(int numOfRepeats, int timePerMoving, int motorID);

private:
    void Delay(int t);

signals:

public slots:

};

#endif // TESTS_H
