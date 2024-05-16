#ifndef VOTEGRAPHICITEM_H
#define VOTEGRAPHICITEM_H

#include <QGraphicsPixmapItem>
#include <QList>

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPolygonF;
QT_END_NAMESPACE

class Vote;

//! [0]
class VoteGraphicItem : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 15 };
    enum DiagramType { Step, Conditional, StartEnd, Io };

    VoteGraphicItem(DiagramType diagramType, std::shared_ptr<Vote> vote, QGraphicsItem *parent = nullptr);

    DiagramType diagramType() const { return myDiagramType; }
    QPolygonF polygon() const { return myPolygon; }
    QPixmap image() const;
    int type() const override { return Type; }

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    DiagramType myDiagramType;
    QPolygonF myPolygon;
    QGraphicsTextItem * _textItem;
    std::shared_ptr<Vote> _vote;
};
//! [0]

#endif // VOTEGRAPHICITEM_H
