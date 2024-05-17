#ifndef MAINRESPUBLICA_H
#define MAINRESPUBLICA_H

#include <QMainWindow>

#include "question.h"
#include "personne.h"

class VoteScene;
class QuestionGraphicItem;

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
    void on_AVote(std::shared_ptr<Question> question, QVariant choix);

private:
    Ui::MainResPublica *ui;
    std::list<std::shared_ptr<Question> > _votes;
    VoteScene * _scene;
    Personne _personne;
};
#endif // MAINRESPUBLICA_H
