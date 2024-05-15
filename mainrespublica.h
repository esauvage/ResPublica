#ifndef MAINRESPUBLICA_H
#define MAINRESPUBLICA_H

#include <QMainWindow>

#include "vote.h"

class VoteScene;
class VoteGraphicItem;

QT_BEGIN_NAMESPACE
namespace Ui { class MainResPublica; }
QT_END_NAMESPACE

class MainResPublica : public QMainWindow
{
    Q_OBJECT

public:
    MainResPublica(QWidget *parent = nullptr);
    ~MainResPublica();

private slots:
    void on_actionCr_er_triggered();
    void itemInserted(QPointF pos);

    void on_actionEnregistrer_triggered();

    void on_actionOuvrir_triggered();

private:
    Ui::MainResPublica *ui;
    std::list<std::shared_ptr<Vote> > _votes;
    VoteScene * scene;
};
#endif // MAINRESPUBLICA_H
