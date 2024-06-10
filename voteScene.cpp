
#include "VoteScene.h"

#include "mainrespublica.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QSqlQuery>

using namespace std;

//! [0]
VoteScene::VoteScene(QObject *parent)
    : QGraphicsScene(parent)
{
//    myMode = MoveItem;
    myMode = InsertItem;
    myItemType = QuestionGraphicItem::Step;
    line = nullptr;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
}

void VoteScene::sauvegarde()
{
    for (const auto & question : _itemsQuestions)
    {
        QSqlQuery sqlPos(QString("SELECT X, Y FROM POSITIONS_QUESTIONS WHERE ID = %1").arg(question.first->id()));
        if (sqlPos.next())
        {
            sqlPos.prepare("UPDATE POSITIONS_QUESTIONS SET X = ?, Y = ? WHERE ID = ?");
            sqlPos.bindValue(0, question.second->pos().x());
            sqlPos.bindValue(1, question.second->pos().y());
            sqlPos.bindValue(2, question.first->id());
            sqlPos.exec();
        }
    }
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
    if (isItemChange(QuestionGraphicItem::Type)) {
        QuestionGraphicItem *item = qgraphicsitem_cast<QuestionGraphicItem *>(selectedItems().first());
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

void VoteScene::setItemType(QuestionGraphicItem::DiagramType type)
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
//! [11]

//! [12]
bool VoteScene::isItemChange(int type) const
{
    const QList<QGraphicsItem *> items = selectedItems();
    const auto cb = [type](const QGraphicsItem *item) { return item->type() == type; };
    return std::find_if(items.begin(), items.end(), cb) != items.end();
}
//! [12]

//! [13]
void VoteScene::creer(MainResPublica * mainRespublica)
{
    //Création de l'IHM.
    clear();
    auto electeur = mainRespublica->electeurCourant();
    for (const auto &q : mainRespublica->questions())
    {
        auto item = new QuestionGraphicItem(QuestionGraphicItem::Step, q, electeur);
        _itemsQuestions.insert(QPair<shared_ptr<Question>, QuestionGraphicItem *>(q, item));
        if (electeur)
        {
            _itemsQuestions[q]->setBrush(electeur->votes()[q].aConfirmer() ? Qt::red : Qt::white);
        }
        else
        {
            _itemsQuestions[q]->setBrush(Qt::white);
        }
        addItem(item);
        item->setSelected(true);
        QPoint pos(100 * q->id(), 100 * q->id());
        QSqlQuery sqlPos(QString("SELECT X, Y FROM POSITIONS_QUESTIONS WHERE ID = %1").arg(q->id()));
        if (sqlPos.next())
        {
            pos.setX(sqlPos.value(0).toInt());
            pos.setY(sqlPos.value(1).toInt());
        }

        item->setPos(pos);
        connect(item, &QuestionGraphicItem::AVote, mainRespublica, &MainResPublica::on_AVote);
        connect(item, &QuestionGraphicItem::AVoteSecret, mainRespublica, &MainResPublica::on_AVoteSecret);
        connect(item, &QuestionGraphicItem::montrerResultats, mainRespublica, &MainResPublica::on_MontrerResultats);
    }
}
//! [13]
