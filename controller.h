#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include <QObject>

#include "mainwindow.h"
#include "ip_connection.h"
#include "collimatorsset.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    ~Controller();

    void TalkToBoard(const QString &sendPhrase);
    void ResetCollimatorsData(int setID);

    void ResetAllCollimators(int setID);

    void SetPulses(int setID, int motorID, const QString &width, const QString &period);

    uint16_t ShowCollimatorCoordinate(int setID, int motorID);

    bool IsMagnetConnected();
    bool ConnectMagnet();
    void SetMagnetIPAddress(const QString &ipaddress);
    bool MagnetOutputStatus();
    void MagnetOutputOn();
    void MagnetOutputOff();

private:
    IP_Connection* CollMaster;
    CollimatorsSet** collSets;
    MainWindow* view;

    QString GenerateCoordinate(const QString &coord_mm, int boxID, int collimatorID);
    int ValidateResponse(const QByteArray &response);
    QByteArray InitResponse();

private slots:
    void DataReceived();
    void GetCollimatorCoordinate(int boxID, int collimatorID);
    void SetCollimatorCoordinate(int boxID, int collimatorID, const QString &coord_mm);
    void ResetCollimator(int boxID, int collimatorID);

signals:
    void CollimatorCoordinateChanged(int setID, int motorID, uint16_t newCoordinate);
    void Connected();
    void Disconnected();

    void MagnetConnected();
    void MagnetDataReceived(float u, float i);

public slots:
};

#endif /* CONTROLLER_H */
