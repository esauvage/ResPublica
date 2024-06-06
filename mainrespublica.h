#ifndef MAINRESPUBLICA_H
#define MAINRESPUBLICA_H

#include <QMainWindow>

#include <QSqlDatabase>

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
    void on_AVoteSecret(std::shared_ptr<Question> question, QVariant choix);
    void on_MontrerResultats(std::shared_ptr<Question> question);
    void on_actionSe_connecter_triggered();
    void on_actionSe_d_sinscrire_triggered();

private:
    bool verifierPresenceConnus();
    void creerScene();
    void setDatabase();

    Ui::MainResPublica *ui;
    std::list<std::shared_ptr<Question> > _questions;
    std::list<std::shared_ptr<Personne> > _personnes;
    std::map<std::shared_ptr<Question>, std::list<Vote> > _votesSecrets;
    std::shared_ptr<Personne> _electeurCour;
    VoteScene * _scene;
    std::list<std::shared_ptr<Personne> > _desinscriptions;
};
#endif // MAINRESPUBLICA_H
