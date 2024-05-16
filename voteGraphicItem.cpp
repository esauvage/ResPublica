#include "voteGraphicItem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QPainter>

#include "vote.h"
#include "dlgeditvote.h"

//! [0]
VoteGraphicItem::VoteGraphicItem(DiagramType diagramType, std::shared_ptr<Vote> vote, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent), myDiagramType(diagramType), _vote(vote)
{
    QPainterPath path;
    switch (myDiagramType) {
        case StartEnd:
            path.moveTo(200, 50);
            path.arcTo(150, 0, 50, 50, 0, 90);
            path.arcTo(50, 0, 50, 50, 90, 90);
            path.arcTo(50, 50, 50, 50, 180, 90);
            path.arcTo(150, 50, 50, 50, 270, 90);
            path.lineTo(200, 25);
            myPolygon = path.toFillPolygon();
            break;
        case Conditional:
            myPolygon << QPointF(-100, 0) << QPointF(0, 100)
                      << QPointF(100, 0) << QPointF(0, -100)
                      << QPointF(-100, 0);
            break;
        case Step:
            myPolygon << QPointF(-100, -100) << QPointF(100, -100)
                      << QPointF(100, 100) << QPointF(-100, 100)
                      << QPointF(-100, -100);
            break;
        default:
            myPolygon << QPointF(-120, -80) << QPointF(-70, 80)
                      << QPointF(120, 80) << QPointF(70, -80)
                      << QPointF(-120, -80);
            break;
    }
    setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    auto textItem = new QGraphicsTextItem(this);
    textItem->setZValue(1000.0);
    textItem->setPlainText(vote->question());
//    connect(textItem, &VoteTextItem::lostFocus,
//            this, &VoteScene::editorLostFocus);
//    connect(textItem, &VoteTextItem::selectedChange,
//            this, &VoteScene::itemSelected);
}
//! [0]

//! [4]
QPixmap VoteGraphicItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

    return pixmap;
}
//! [4]

//! [5]
void VoteGraphicItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
}
//! [5]

//! [6]
QVariant VoteGraphicItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return value;
}
//! [6]

void VoteGraphicItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    DlgEditVote edtVote(_vote);
    if (!edtVote.exec())
    {
        return;
    }
    _textItem->setPlainText(_vote->question());
}
