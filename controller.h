#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "ip_connection.h"
#include "encoder.h"
#include "tests.h"

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
    void ResetMotorsData();
    void SetMotorCoordinate(int motorID, const QString &mm, const QString &um);
    int GetMotorCoordinate(int motorID);
    void Reset(int motorID);
    void ResetAll();
    void SetPulses(const QString &width, const QString &period);
    QByteArray TalkToBoard(const QString &sendPhrase);

private:
    IP_Connection *PCB;
    Encoder **motors;
    int ValidateResponse(const QByteArray &response);

    QString GenerateCoordinate(const QString &mm, const QString &um, int motorID);

signals:

public slots:

};

#endif // CONTROLLER_H
