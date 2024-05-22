#include "mainrespublica.h"
#include "ui_mainrespublica.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QMessageBox>

#include <QProcess>

#include "dlgeditvote.h"
#include "VoteScene.h"
#include "questionliste.h"
#include "dlgconnexion.h"
#include "dlgresultats.h"

#include <QDebug>

using namespace std;

MainResPublica::MainResPublica(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainResPublica), _electeurCour(nullptr)
{
    ui->setupUi(this);
    _scene = new VoteScene(this);
    _scene->setSceneRect(QRectF(0, 0, 5000, 5000));
    ui->mainView->setScene(_scene);
    connect(_scene, &VoteScene::itemInserted, this, &MainResPublica::itemInserted);
//    connect(scene, &VoteScene::textInserted,
//            this, &MainWindow::textInserted);
//    connect(scene, &VoteScene::itemSelected,
//            this, &MainWindow::itemSelected);
}

MainResPublica::~MainResPublica()
{
    delete ui;
    delete _scene;
}


void MainResPublica::on_actionCr_er_triggered()
{
    auto nouveauQuestion = make_shared<QuestionListe>();
    _questions.push_back(nouveauQuestion);
    DlgEditQuestion dlg(nouveauQuestion, this);
    dlg.exec();
}

void MainResPublica::itemInserted(QPointF pos)
{
    auto nouveauQuestion = make_shared<QuestionListe>();
    DlgEditQuestion dlg(nouveauQuestion, this);
    if (!dlg.exec())
    {
        return;
    }
    _questions.push_back(nouveauQuestion);
    QuestionGraphicItem * voteItem = new QuestionGraphicItem(QuestionGraphicItem::Step, nouveauQuestion, _electeurCour);
    voteItem->setBrush(Qt::white);
    _scene->addItem(voteItem);
    voteItem->setPos(pos);
    connect(voteItem, &QuestionGraphicItem::AVote, this, &MainResPublica::on_AVote);
}


void MainResPublica::on_actionEnregistrer_triggered()
{
    QFile sortie("Votes.txt");
    if (!sortie.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&sortie);
    QJsonArray questions;
    for (const auto &v : _questions)
    {
        QJsonObject jobject;
        jobject["id"] = QJsonValue::fromVariant(v->id());
        jobject["Question"] = v->question();
        jobject["Choix"] = QJsonValue::fromVariant(v->choix());
        questions.append(jobject);
    }
    QJsonObject corpus;
    corpus["questions"] = questions;
    QJsonArray electeurs;
    for (const auto & p : _personnes)
    {
        QJsonArray votes;
        for (const auto &v : p->votes())
        {
            QJsonObject jobject;
            //Test d'absence de problème sur les votes
            if (v.second.aConfirmer())
            {
                QMessageBox::information(this, "Attention", "Il vous reste des votes à confirmer");
            }
            jobject["Question"] = v.first->question();
            jobject["QuestionChecksum"] = QString(v.first->checksum().toBase64());
            jobject["Choix"] = QJsonValue::fromVariant(v.second.choix());
            votes.append(jobject);
        }
        QJsonObject personne;
        personne["Votes"] = votes;
        personne["Pseudo"] = p->pseudonyme();
        personne["ClefPublique"] = QString::fromUtf8(p->clefPublique());
        electeurs.append(personne);
    }
    corpus["electeurs"] = electeurs;
    QJsonDocument doc( corpus );
    out << doc.toJson() << "\n";
    sortie.close();
}


void MainResPublica::on_actionOuvrir_triggered()
{
    QFile entree("Votes.txt");
    if (!entree.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    _questions.clear();
    _personnes.clear();
    QByteArray saveData = entree.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    const QJsonObject corpus = loadDoc.object();
    const QJsonArray questions = corpus["questions"].toArray();
    for (const QJsonValue &q : questions)
    {
        auto nouveauQuestion = make_shared<QuestionListe>();
        QJsonObject jobject = q.toObject();
        nouveauQuestion->setId(jobject["id"].toInteger());
        nouveauQuestion->setQuestion(jobject["Question"].toString());
        nouveauQuestion->setChoix(jobject["Choix"].toVariant());
        _questions.push_back(nouveauQuestion);
    }
    const QJsonArray electeurs = corpus["electeurs"].toArray();
    for (const QJsonValue &e : electeurs)
    {
        const QJsonObject personne = e.toObject();
        shared_ptr<Personne> nouvelElecteur = nullptr;
        //Vérifier que ce n'est pas l'électeur courant
        if (_electeurCour && _electeurCour->pseudonyme() == personne["Pseudo"].toString())
        {
            nouvelElecteur = _electeurCour;
        }
        else
        {
            nouvelElecteur = make_shared<Personne>();
            nouvelElecteur->setPseudonyme(personne["Pseudo"].toString());
            nouvelElecteur->setClefPublique(personne["ClefPublique"].toString().toUtf8());
            _personnes.push_back(nouvelElecteur);
        }
        const QJsonArray votes = personne["Votes"].toArray();
        for (const auto &v : votes)
        {
            QJsonObject jobject = v.toObject();
            for (const shared_ptr<Question> &q : _questions)
            {
                if (q->question() != jobject["Question"].toString())
                {
                    continue;
                }
                bool aVerifier = QString(q->checksum().toBase64()) != jobject["QuestionChecksum"].toString();
                nouvelElecteur->addVote(q, jobject["Choix"].toVariant(), aVerifier);
            }
        }
    }
    creerScene();
}


void MainResPublica::creerScene()
{
    //Création de l'IHM.
    _scene->clear();
    map<shared_ptr<Question>, QuestionGraphicItem *>itemsQuestions;
    for (const auto &q : _questions)
    {
        auto item = new QuestionGraphicItem(QuestionGraphicItem::Step, q, _electeurCour);
        itemsQuestions.insert(QPair<shared_ptr<Question>, QuestionGraphicItem *>(q, item));
        if (_electeurCour)
        {
            itemsQuestions[q]->setBrush(_electeurCour->votes()[q].aConfirmer() ? Qt::red : Qt::white);
        }
        else
        {
            itemsQuestions[q]->setBrush(Qt::white);
        }
        _scene->addItem(item);
        item->setSelected(true);
        item->setPos(100 * q->id(), 100 * q->id());
        connect(item, &QuestionGraphicItem::AVote, this, &MainResPublica::on_AVote);
        connect(item, &QuestionGraphicItem::montrerResultats, this, &MainResPublica::on_MontrerResultats);
    }
}

void MainResPublica::on_AVote(std::shared_ptr<Question> question, QVariant choix)
{
    _electeurCour->addVote(question, choix, false);
}

void MainResPublica::on_MontrerResultats(std::shared_ptr<Question> question)
{
    list<Vote> votes;
    for(const auto &p : _personnes)
    {
        votes.push_back(p->votes()[question]);
    }
    DlgResultats dlgResultats(this);
    dlgResultats.setVotes(votes);
    dlgResultats.exec();
}


void MainResPublica::on_actionSe_connecter_triggered()
{
    DlgConnexion dlgConnexion(this);
    if (dlgConnexion.exec())
    {
        _electeurCour = nullptr;
        for (const auto & p : _personnes)
        {
            if (p->pseudonyme() == dlgConnexion.pseudonyme())
            {
                _electeurCour = p;
                break;
            }
        }
        if (!_electeurCour)
        {
            _electeurCour = make_shared<Personne>();
            _electeurCour->setPseudonyme(dlgConnexion.pseudonyme());
            _personnes.push_back(_electeurCour);
            //Creation de la paire de clefs
            QString program = "openssl";
            QStringList arguments;
            arguments << "genrsa" << "-out" << QString("%1.pem").arg(_electeurCour->pseudonyme()) << "2048";

            QProcess *myProcess = new QProcess(this);
            myProcess->start(program, arguments);
            if (!myProcess->waitForFinished())
            {
                //TODO : gestion erreur
            }
            qDebug() << myProcess->error() << myProcess->readAllStandardOutput();
            delete myProcess;

            arguments.clear();
            arguments << "rsa" << "-in" << QString("%1.pem").arg(_electeurCour->pseudonyme()) << "-pubout" << "-out" << QString("%1.crt").arg(_electeurCour->pseudonyme());
            myProcess = new QProcess(this);
            myProcess->start(program, arguments);
            if (!myProcess->waitForFinished())
            {
                //TODO : gestion erreur
            }
            delete myProcess;
            QFile clefPublique(QString("%1.crt").arg(_electeurCour->pseudonyme()));
            if (!clefPublique.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                qDebug() << "Fichier illisible";
            }
            _electeurCour->setClefPublique(clefPublique.readAll());
            clefPublique.close();
            //run "openssl genrsa -out private.pem 2048" "openssl rsa -in keypair.pem -pubout -out publickey.crt"
        }
    }
    creerScene();
}

