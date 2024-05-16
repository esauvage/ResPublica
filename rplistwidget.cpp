#include "rplistwidget.h"

RPListWidget::RPListWidget(QWidget * parent)
    :QListWidget(parent)
{}

void RPListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (findItems("", Qt::MatchExactly).isEmpty())
    {
        QListWidgetItem * item = new QListWidgetItem(this);
        item->setText("");
        item->setFlags (item->flags () | Qt::ItemIsEditable);
        addItem(item);
    }
    QListWidget::mouseDoubleClickEvent(event);
}
