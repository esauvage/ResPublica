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

class Question;
class Personne;

//! [0]
class QuestionGraphicItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 15 };
    enum DiagramType { Step, Conditional, StartEnd, Io };

    QuestionGraphicItem(DiagramType diagramType, std::shared_ptr<Question> vote,
                        const Personne & personne,
                        QGraphicsItem *parent = nullptr, QObject *objParent = nullptr);

    DiagramType diagramType() const { return myDiagramType; }
    QPolygonF polygon() const { return myPolygon; }
    QPixmap image() const;
    int type() const override { return Type; }
signals:
    void AVote(std::shared_ptr<Question>, QVariant);
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:
    DiagramType myDiagramType;
    QPolygonF myPolygon;
    QGraphicsTextItem * _textItem;
    std::shared_ptr<QGraphicsRectItem> _button;
    std::shared_ptr<Question> _vote;
    const Personne &_personne;
};
//! [0]

#endif // VOTEGRAPHICITEM_H
