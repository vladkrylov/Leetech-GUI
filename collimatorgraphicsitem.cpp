#include <QDebug>

#include "collimatorgraphicsitem.h"

CollimatorGraphicsItem::CollimatorGraphicsItem(int myLength, QGraphicsObject *parent) :
    QGraphicsObject(parent)
    , length(myLength)
{
//    setFlag(ItemIsMovable);
    setAcceptHoverEvents(true);

    collimatorMenu = new QMenu();
    moveAction = collimatorMenu->addAction("Move");
    resetAction = collimatorMenu->addAction("Reset");
    updateAction = collimatorMenu->addAction("Update");
    collimatorMenu->addSeparator();
    closeAction = collimatorMenu->addAction("Close");

    connect(moveAction, SIGNAL(triggered()), this, SIGNAL(MoveMe()));
    connect(resetAction, SIGNAL(triggered()), this, SIGNAL(ResetMe()));
    connect(updateAction, SIGNAL(triggered()), this, SIGNAL(UpdateMe()));
    connect(closeAction, SIGNAL(triggered()), this, SIGNAL(CloseMe()));
}

CollimatorGraphicsItem::~CollimatorGraphicsItem()
{
    delete collimatorMenu;
}

QRectF CollimatorGraphicsItem::boundingRect() const
{
    return QRectF(0, 0, length, length*3/4);
}

void CollimatorGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rec = boundingRect();
    int w = (int)rec.width();
    int h = (int)rec.height();

    QBrush brush1(Qt::darkGray);
    QBrush brush2(Qt::lightGray);

    QRect r1(w-h/1.7, 0, h/1.7, h);
    QRect r2(0, h/3, w-h/1.7, h/3);

    painter->fillRect(r1, brush1);
    painter->fillRect(r2, brush2);
    painter->drawRect(r1);
    painter->drawRect(r2);
}

void CollimatorGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QAction *selectedAction = collimatorMenu->exec(event->screenPos());
}

void CollimatorGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(10);
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setColor(Qt::black);

    setGraphicsEffect(shadow);
}

void CollimatorGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setGraphicsEffect(0);
}
















































