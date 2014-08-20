#ifndef TESTS_H
#define TESTS_H

#include <QObject>
#include "controller.h"

class Tests : public QObject
{
    Q_OBJECT
public:
    IP_Connection *PCB;

    explicit Tests(IP_Connection *testPCB, QObject *parent = 0);

    void Test(int motorID);
    void TestPulsesForOscilloscope(int motorID);

private:


signals:

public slots:

};

#endif // TESTS_H
