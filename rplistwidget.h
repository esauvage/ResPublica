#ifndef RPLISTWIDGET_H
#define RPLISTWIDGET_H

#include <QListWidget>

class RPListWidget : public QListWidget
{
public:
    RPListWidget(QWidget *parent = nullptr);
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // RPLISTWIDGET_H
