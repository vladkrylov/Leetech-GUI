#ifndef COLLIMATORGRAPHICSITEM_H
#define COLLIMATORGRAPHICSITEM_H

#include <QGraphicsObject>
#include <QPainter>
#include <QtCore>
#include <QtGui>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsDropShadowEffect>

#include "types.h"

class CollimatorGraphicsItem : public QGraphicsObject
{
    Q_OBJECT
public:
    CollimatorGraphicsItem(int myLength, QGraphicsObject* parent = 0);
    ~CollimatorGraphicsItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void MoveMe();
    void ResetMe();
    void UpdateMe();
    void CloseMe();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
    int length;
    QMenu* collimatorMenu;
    QAction* moveAction;
    QAction* resetAction;
    QAction* updateAction;
    QAction* closeAction;
};

#endif // COLLIMATORGRAPHICSITEM_H
