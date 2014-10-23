#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include <QObject>

#include "ip_connection.h"
#include "tests.h"
#include "collimatorsset.h"

class Tests;
class IP_Connection;

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

    QByteArray TalkToBoard(const QString &sendPhrase);
    void ResetMotorsData(int setID);
    void SetMotorCoordinate(int setID, int motorID, const QString &coord_mm);
    void GetMotorCoordinate(int setID, int motorID);

    void Reset(int setID, int motorID);
    void ResetAll(int setID);

    void SetPulses(int setID, const QString &width, const QString &period);

    uint16_t ShowMotorCoordinate(int setID, int motorID);

private:
    IP_Connection *PCB;
    CollimatorsSet **colSets;
    QString GenerateCoordinate(const QString &coord_mm, int setID, int motorID);
    int ValidateResponse(const QByteArray &response);

signals:
    void MotorCoordinateChanged(int setID, int motorID, uint16_t newCoordinate);

public slots:

};

#endif // CONTROLLER_H
