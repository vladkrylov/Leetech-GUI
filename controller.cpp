#include <QDebug>

#include "controller.h"

Controller::Controller(QObject *parent) : QObject(parent)
{
    view = new MainWindow();

    view->SetMaxOpeningX(20.);
    view->SetMaxOpeningY(22.);

    connect(view, SIGNAL(MoveCollimator(int,int,QString)), this, SLOT(SetCollimatorCoordinate(int,int,QString)));
    connect(view, SIGNAL(ResetCollimator(int,int)), this, SLOT(ResetCollimator(int,int)));

    view->show();
}

Controller::~Controller()
{
    delete view;
}

void Controller::SetCollimatorCoordinate(int setID, int motorID, const QString &coord_mm)
{
    float coord = coord_mm.toFloat();
//    qDebug() << "Set" << setID << ": move motor" << motorID << "to" << coord << "mm.";
    view->UpdateCoordinate(motorID, coord);
}

void Controller::ResetCollimator(int setID, int motorID)
{
    view->UpdateCoordinate(motorID, 0);
}
