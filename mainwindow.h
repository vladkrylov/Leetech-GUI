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

public slots:
    void CollimatorsConnected();
    void CollimatorsDisconnected();
    void UpdateCoordinate(int collimatorBox, int collimatorID, float position);
    void SetMaxOpeningX(int collimatorBox, float opening);
    void SetMaxOpeningY(int collimatorBox, float opening);

signals:
    void ConnectCollimatiors(QString IPAddress);
    void DisconnectCollimatiors();
    void MoveCollimator(int collimatorBox, int collimatorID, QString coordinate);
    void ResetCollimator(int collimatorBox, int collimatorID);
    void ResetAll(int collimatorBox);
    void CloseCollimators(int collimatorBox, int collimatorID);
    void UpdateCollimator(int collimatorBox, int collimatorID);
    void SetPWMPeriod(int collimatorBox, int collimatorID, QString T);
    void SetXHoleSize(int collimatorBox, float size);
    void SetYHoleSize(int collimatorBox, float size);
    void SetXHoleOffset(int collimatorBox, float offset);
    void SetYHoleOffset(int collimatorBox, float offset);

    void ImReady();
    void UpdateScene(int collimatorBox);

private:
    typedef enum {
        WAITING_FOR_COLLIMATORS_RESPONSE,
        READY
    } state_t;
    state_t applicationState;

    Ui::MainWindow* ui;
    int sceneSize;
    QGraphicsScene* scene;

    // right click menu items
    QMenu* collimatorMenu;
    QAction* moveAction;
    QAction* resetAction;

    // top menu items
    QMenu* settingsMenu;
    QAction* setPulsesAction;
    void CreateTopMenuActions();
    void CreateTopMenu();

    // scene elements
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
    void ConnectCollimatorsRequests();

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
    void FinishWaitingState();
    void WaitForReady();

private slots:
    void SetWaitingState();

    void MoveRightRequested();
    void MoveLeftRequested();
    void MoveTopRequested();
    void MoveBottomRequested();

    void ResetRightRequested();
    void ResetLeftRequested();
    void ResetTopRequested();
    void ResetBottomRequested();

    void UpdateCollimatorHandler();
    void UpdateRightRequested();
    void UpdateLeftRequested();
    void UpdateTopRequested();
    void UpdateBottomRequested();

    void CloseHorizontal();
    void CloseVertical();

    void MoveCollimatorHandler();
    void ResetCollimatorHandler();
    void ResetAllCollimatorHandler();
    void SetPWM();

    void CollimatorBoxChanged();
    void on_ConnectButton_clicked();
    void on_ResetAllButton_clicked();
    void on_holeSizeHorizontal_returnPressed();
    void on_holeSizeVertical_returnPressed();
    void on_centerHorizontalOffet_returnPressed();
    void on_centerVerticalOffet_returnPressed();
    void on_PulsePeriodBox_valueChanged(const QString &arg1);
    void on_pushButton_clicked();
    void on_SetXHoleSizeButton_clicked();
    void on_SetYHoleSizeButton_clicked();
    void on_SetXHoleOffsetButton_clicked();
    void on_SetYHoleOffsetButton_clicked();
};

#endif // MAINWINDOW_H
