#ifndef VOTESCENE_H
#define VOTESCENE_H

#include <QGraphicsScene>

#include "voteGraphicItem.h"
#include "voteTextItem.h"

class VoteScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertItem, InsertText, MoveItem };

    explicit VoteScene(QObject *parent = nullptr);
    QFont font() const { return myFont; }
    QColor textColor() const { return myTextColor; }
    QColor itemColor() const { return myItemColor; }
    QColor lineColor() const { return myLineColor; }
    void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setItemColor(const QColor &color);
    void setFont(const QFont &font);

public slots:
    void setMode(Mode mode);
    void setItemType(VoteGraphicItem::DiagramType type);
    void editorLostFocus(VoteTextItem *item);

signals:
    void itemInserted(QPointF pos);
    void textInserted(QGraphicsTextItem *item);
    void itemSelected(QGraphicsItem *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private:
    bool isItemChange(int type) const;

    VoteGraphicItem::DiagramType myItemType;
    Mode myMode;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;
    QFont myFont;
    VoteTextItem *textItem;
    QColor myTextColor;
    QColor myItemColor;
    QColor myLineColor;
};

#endif // VOTESCENE_H
