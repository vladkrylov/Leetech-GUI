#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include <QObject>
#include <QtSerialPort/QSerialPort>
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

    int IsConnected();
    int Connect();
    void Disconnect();
    void SetIPAddress(const QString &ipaddress);

    void TalkToBoard(const QString &sendPhrase);
    void ResetMotorsData(int setID);
    void SetMotorCoordinate(int setID, int motorID, const QString &coord_mm);
    void GetMotorCoordinate(int setID, int motorID);

    void Reset(int setID, int motorID);
    void ResetAll(int setID);

    void SetPulses(int setID, int motorID, const QString &width, const QString &period);

    uint16_t ShowMotorCoordinate(int setID, int motorID);

    QStringList GetSerialPorts();
    bool ConnectHV(const QString& name, int baud);
    bool HVConnented();
    void DisconnectHV();
    QByteArray GetHV();
    QByteArray GetHVCurrent();

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

signals:
    void MotorCoordinateChanged(int setID, int motorID, uint16_t newCoordinate);
    void CollimatorsConnected();
    void CollimatorsDisconnected();

    void MagnetConnected();
    void MagnetDataReceived(float u, float i);

public slots:
    void CollimatorsDataReceived();

    void SetMagnetVoltage(float u);
    void SetMagnetCurrent(float i);

};

#endif // CONTROLLER_H
