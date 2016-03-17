#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include <QObject>
#include <QTcpSocket>

#include "ip_connection.h"
#include "tests.h"
#include "collimatorsset.h"
#include "trajectory.h"

class Tests;
class IP_Connection;
class MainWindow;

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    ~Controller();

    MainWindow *userInterface;

    Tests *TestObject;

//    int IsCollimatorsConnected();
    void TalkToBoard(const QString &sendPhrase);


    void Reset(int setID, int motorID);
    void ResetAll(int setID);

    void SetPulses(int setID, int motorID, const QString &width, const QString &period);

    uint16_t ShowMotorCoordinate(int setID, int motorID);

    bool IsMagnetConnected();
    bool ConnectMagnet();
    void SetMagnetIPAddress(const QString &ipaddress);
    bool MagnetOutputStatus();
    void MagnetOutputOn();
    void MagnetOutputOff();

private:
    IP_Connection* collMaster;
    CollimatorsSet** colSets;

    QString GenerateCoordinate(const QString &coord_mm, int setID, int motorID);
    int ValidateResponse(const QByteArray &response);
    QByteArray InitResponse();

    Trajectory* traj;

    QTcpSocket* magnet;
    QString magnetIP;
    int magnetPort;
    QTimer* magnetTimer;

private slots:
    void UpdateMagnetData();

public slots:
    int CollimatorsConnect();
    void CollimatorsDisconnect();
    void ResetMotorsData(int setID);
    void SetMotorCoordinate(int setID, int motorID, const QString &coord_mm);
    void GetMotorCoordinate(int setID, int motorID);
//    void SetCollIPAddress(const QString &ipaddress);

    void CollimatorsDataReceived();

    void SetMagnetVoltage(float u);
    void SetMagnetCurrent(float i);

};

#endif // CONTROLLER_H
