#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include <QObject>
#include "mainwindow.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    ~Controller();

    int IsCollimatorsConnected();
    int ConnectCollimators();
    void DisconnectCollimators();
    void SetCollimatorsIPAddress(const QString &ipaddress);

    void TalkToBoard(const QString &sendPhrase);
    void ResetCollimatorsData(int setID);

    void GetCollimatorCoordinate(int setID, int motorID);


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
    MainWindow* view;

    QString GenerateCoordinate(const QString &coord_mm, int setID, int motorID);
    int ValidateResponse(const QByteArray &response);
    QByteArray InitResponse();

private slots:
    void SetCollimatorCoordinate(int setID, int motorID, const QString &coord_mm);
    void ResetCollimator(int setID, int motorID);

signals:
    void CollimatorCoordinateChanged(int setID, int motorID, uint16_t newCoordinate);
    void Connected();
    void Disconnected();

    void MagnetConnected();
    void MagnetDataReceived(float u, float i);

public slots:
};

#endif /* CONTROLLER_H */
