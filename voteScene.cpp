
#include "VoteScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>

//! [0]
VoteScene::VoteScene(QObject *parent)
    : QGraphicsScene(parent)
{
//    myMode = MoveItem;
    myMode = InsertItem;
    myItemType = VoteGraphicItem::Step;
    line = nullptr;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
}
//! [0]

//! [1]
void VoteScene::setLineColor(const QColor &color)
{
    myLineColor = color;
}
//! [1]

//! [2]
void VoteScene::setTextColor(const QColor &color)
{
    myTextColor = color;
}
//! [2]

//! [3]
void VoteScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(VoteGraphicItem::Type)) {
        VoteGraphicItem *item = qgraphicsitem_cast<VoteGraphicItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}
//! [3]

//! [4]
void VoteScene::setFont(const QFont &font)
{
    myFont = font;
}
//! [4]

void VoteScene::setMode(Mode mode)
{
    myMode = mode;
}

void VoteScene::setItemType(VoteGraphicItem::DiagramType type)
{
    myItemType = type;
}

//! [6]
void VoteScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;
    myMode = selectedItems().isEmpty() ? InsertItem : MoveItem;

    switch (myMode) {
        case InsertItem:
            emit itemInserted(mouseEvent->scenePos());
            break;
//! [6] //! [9]
    default:
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}
//! [9]

//! [10]
void VoteScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
//! [10]

//! [11]
void VoteScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
//! [13]

//! [14]
bool VoteScene::isItemChange(int type) const
{
    const QList<QGraphicsItem *> items = selectedItems();
    const auto cb = [type](const QGraphicsItem *item) { return item->type() == type; };
    return std::find_if(items.begin(), items.end(), cb) != items.end();
}
//! [14]
