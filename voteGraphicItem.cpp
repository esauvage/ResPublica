#include "voteGraphicItem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QPainter>

#include <QDialog>

#include "question.h"
#include "personne.h"
#include "dlgeditvote.h"
#include "dlglistevote.h"

using namespace std;
//! [0]
QuestionGraphicItem::QuestionGraphicItem(DiagramType diagramType, std::shared_ptr<Question> vote,
                                         const Personne & personne,
                                         QGraphicsItem *parent, QObject *objParent)
    : QObject(objParent), QGraphicsPolygonItem(parent), myDiagramType(diagramType), _vote(vote), _personne(personne)
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
    _button = make_shared<QGraphicsRectItem>(20, 20, 20, 20, this);
    _button->setBrush(Qt::gray);
    auto textItem = new QGraphicsTextItem(this);
    textItem->setZValue(1000.0);
    textItem->setPlainText(vote->question());
}
//! [0]

//! [4]
QPixmap QuestionGraphicItem::image() const
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
void QuestionGraphicItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
}
//! [5]

//! [6]
QVariant QuestionGraphicItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return value;
}
//! [6]

void QuestionGraphicItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    DlgEditQuestion edtQuestion(_vote);
    if (!edtQuestion.exec())
    {
        return;
    }
    _vote->setQuestion(_textItem->toPlainText());
}

void QuestionGraphicItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (_button->rect().contains(event->pos()))
    {
        DlgListeQuestion dlg;
        dlg.setPossibilites(_vote->choix().toStringList());
        dlg.setSelection(_personne.votes()[_vote].choix().toStringList());
        if (dlg.exec())
        {
            setBrush(Qt::white);
            emit AVote(_vote, dlg.selection());
        }
    }
    QGraphicsPolygonItem::mouseReleaseEvent(event);
}
