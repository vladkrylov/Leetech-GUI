#include "ip_connection.h"

IP_Connection::IP_Connection(QObject *parent) :
    QObject(parent)
{
    IP_Init();

    connect(socket, SIGNAL(connected()), this, SIGNAL(Connected()));
    connect(socket, SIGNAL(disconnected()), this, SIGNAL(Disconnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sdisconnected()));
    connect(socket, SIGNAL(readyRead()), this, SIGNAL(dataReceived()));
}

IP_Connection::~IP_Connection()
{
    delete socket;
}

void IP_Connection::IP_Init()
{
    socket = new QTcpSocket(this);
    port = 80;
    Delay_waitForConnected = 1000;
    Delay_waitForBytesWritten = 1000;
    Delay_waitForReadyRead = 150;
    _Connected = false;
}

bool IP_Connection::PCB_Connect()
{
    socket->connectToHost(IP_Address, 80);
    _Connected = socket->waitForConnected(Delay_waitForConnected);
    return _Connected;
}

void IP_Connection::PCB_Disconnect()
{
    if (_Connected) {
        socket->disconnectFromHost();
        _Connected = false;
    }
}

void IP_Connection::PCB_SendData(QString message)
{
    if (_Connected) {
        socket->write(message.toUtf8().constData());
        socket->waitForBytesWritten(Delay_waitForBytesWritten);
    } else {
        qDebug() << "Connection is not established! Cannot send the data.";
    }
}

QByteArray IP_Connection::PCB_ReceiveData()
{
    QByteArray socketResponse;
    if (_Connected) {
        socket->waitForReadyRead(Delay_waitForReadyRead);
        socketResponse = socket->readAll();
    } else {
        qDebug() << "Connection is not established! Cannot receive the data.";
    }
    return socketResponse;
}

void IP_Connection::SetIPAddress(QString adr)
{
    IP_Address = adr;
}

bool IP_Connection::IsConnected()
{
    return _Connected;
}

//void IP_Connection::readyRead()
//{
////    qDebug() << "Some data has been received.";
//    QByteArray response;
//    response = socket->readAll();
//    qDebug() << response << endl;
//}

void IP_Connection::sdisconnected()
{
    _Connected = false;
}

QByteArray IP_Connection::readAll()
{
    return socket->readAll();
}
