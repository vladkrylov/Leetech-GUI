#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "ip_connection.h"
#include "encoder.h"
#include "tests.h"

class Tests;

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    ~Controller();

    Tests *TestObject;

    int IsConnected();
    int Connect();
    void Disconnect();
    void SetIPAddress(const QString &ipaddress);
    void ResetMotorsData();
    void SetMotorCoordinate(int motorID, const QString &mm, const QString &um);
    void Reset(int motorID);
    void ResetAll();

private:
    IP_Connection *PCB;
    Encoder **motors;


    QString GenerateCoordinate(const QString &mm, const QString &um);

signals:

public slots:

};

#endif // CONTROLLER_H
