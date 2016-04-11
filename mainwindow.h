#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QFont>
#include <QPointF>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QTimeLine>
#include <QGraphicsItemAnimation>

#include "collimatorgraphicsitem.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void Update();
    void SetMaxOpeningX(float opening);
    void SetMaxOpeningY(float opening);

public slots:
    void CollimatorsConnected();
    void CollimatorsDisconnected();
    void UpdateCoordinate(int collimatorBox, int collimatorID, float position);

signals:
    void ConnectCollimatiors(QString IPAddress);
    void DisconnectCollimatiors();
    void MoveCollimator(int collimatorBox, int collimatorID, QString coordinate);
    void ResetCollimator(int collimatorBox, int collimatorID);

private:
    Ui::MainWindow* ui;
    int sceneSize;
    QGraphicsScene* scene;

    QMenu* collimatorMenu;
    QAction* moveAction;
    QAction* resetAction;

    CollimatorGraphicsItem* left;
    CollimatorGraphicsItem* right;
    CollimatorGraphicsItem* top;
    CollimatorGraphicsItem* bottom;

    QFont* titlesFont;
    QFont* textFont;
    QGraphicsTextItem* holeSizeTitle;
    QGraphicsTextItem* holeDXText;
    QGraphicsTextItem* holeDYText;
    QGraphicsTextItem* holeOffsetTitle;
    QGraphicsTextItem* holeOffsetXText;
    QGraphicsTextItem* holeOffsetYText;
    QGraphicsTextItem* maxOpeningTitle1;
    QGraphicsTextItem* maxOpeningTitle2;
    QGraphicsTextItem* maxOpeningXText;
    QGraphicsTextItem* maxOpeningYText;
    QGraphicsTextItem* positionsTitle1;
    QGraphicsTextItem* positionsTitle2;
    QGraphicsTextItem* positionRightText;
    QGraphicsTextItem* positionLeftText;
    QGraphicsTextItem* positionTopText;
    QGraphicsTextItem* positionBottomText;

    float maxOpeningX;
    float maxOpeningY;
    float posRight;
    float posLeft;
    float posTop;
    float posBottom;
    float CalculateHoleDX();
    float CalculateHoleDY();
    float CalculateHoleOffsetX();
    float CalculateHoleOffsetY();
    QString Coord2String(float x);
    QString HoleDXString(float dx);
    QString HoleDYString(float dy);
    QString HoleOffsetXString(float x);
    QString HoleOffsetYString(float y);
    QString MaxOpeningXString(float dx);
    QString MaxOpeningYString(float dy);
    QString PositionRightString(float pos);
    QString PositionLeftString(float pos);
    QString PositionTopString(float pos);
    QString PositionBottomString(float pos);

    void ConstructScene();
    void ConnectUIActions();

    int GetActiveCollimatorBox();
    int GetActiveCollimator();

    QTimer* stepTimer;
    QTimer* animTimer;
    QTimeLine* animationTimer;
    QGraphicsItemAnimation* animation;
    void NiceMove(CollimatorGraphicsItem* collimator, QPointF to);

    QMovie* waitingGif;
    QLabel* waitingIndicator;
    QGraphicsProxyWidget* proxyGif;
    void IndicateWaitingState();
    void FinishWaitingState();

private slots:
    void MoveRightRequested();
    void MoveLeftRequested();
    void MoveTopRequested();
    void MoveBottomRequested();
    void ResetRightRequested();
    void ResetLeftRequested();
    void ResetTopRequested();
    void ResetBottomRequested();

    void MoveCollimatorHandler();
    void ResetCollimatorHandler();
    void on_ConnectButton_clicked();
};

#endif // MAINWINDOW_H
