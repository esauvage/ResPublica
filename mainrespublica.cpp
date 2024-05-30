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
#include "fabriquequestions.h"
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
    QJsonObject corpus;
    FabriqueQuestions fabriqueQuestions;
    corpus["questions"] = fabriqueQuestions.ecrireJson(_questions);
    QJsonArray electeurs;
    for (const auto & p : _personnes)
    {
        if ((p == _electeurCour) && (_desinscriptions.contains(p->pseudonyme())))
        {
            continue;
        }
        for (const auto &v : p->votes())
        {
            //Test d'absence de problème sur les votes
            if (p == _electeurCour)
            {
                if (v.second.aConfirmer())
                {
                    QMessageBox::information(this, "Attention", "Il vous reste des votes à confirmer");
                }
                p->setVotesChecksum(p->signatureVotes());
            }
        }
        QJsonObject personne = p->ecrireJson();
        if (p == _electeurCour)
        {
            QStringList pseudos;
            for (const auto & e : _personnes)
            {
                if (e == p) continue;
                if (_desinscriptions.contains(e->pseudonyme())) continue;
                pseudos << e->pseudonyme();
            }
            p->setElecteursConnus(pseudos);
            personne["ElecteursConnus"] = QJsonArray::fromStringList(pseudos);
            p->setElecteursChecksum(p->calculElecteursCheckSum().toBase64());
            personne["ElecteursCheckSum"] = p->electeursChecksum();
        }
        electeurs.append(personne);
    }
    corpus["electeurs"] = electeurs;
    QJsonArray votes;
    for (auto &v : _votesSecrets)
    {
        QJsonObject jobject;
        jobject["Question"] = v.first->question();
        QJsonArray liste;
        for (auto & elem : v.second)
        {
            QJsonObject voteValide;
            voteValide["Vote"] = QJsonValue::fromVariant(elem.choix());
            if (elem.checksum().isEmpty())
            {
                elem.setChecksum(elem.signe(v.first->question()));
            }
            voteValide["VoteChecksum"] = QJsonValue::fromVariant(elem.checksum());
            voteValide["VoteClefPublique"] = QJsonValue::fromVariant(elem.clefPublique());
            liste.append(voteValide);
        }
        jobject["Votes"] = liste;
        jobject["QuestionChecksum"] = QString(v.first->checksum().toBase64());
        votes.append(jobject);
    }
    corpus["VotesSecrets"] = votes;
    QJsonArray desinscriptions;
    for (auto &v : _desinscriptions)
    {
        desinscriptions.append(v);
    }
    corpus["Desinscrits"] = desinscriptions;
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
    _votesSecrets.clear();

    QJsonDocument loadDoc(QJsonDocument::fromJson(entree.readAll()));
    const QJsonObject corpus = loadDoc.object();
    FabriqueQuestions fabriqueQuestions;
    fabriqueQuestions.lireJson(corpus["questions"].toArray(), _questions);

    const QJsonArray desinscriptions = corpus["Desinscrits"].toArray();
    for (const QJsonValue &d : desinscriptions)
    {
        _desinscriptions << d.toString();
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
            nouvelElecteur->deleteVotes();
        }
        else
        {
            nouvelElecteur = make_shared<Personne>();
            nouvelElecteur->setPseudonyme(personne["Pseudo"].toString());
            nouvelElecteur->setClefPublique(personne["ClefPublique"].toString().toUtf8());
        }
        nouvelElecteur->lireJson(personne, _questions);
        _personnes.push_back(nouvelElecteur);
    }
    if (!verifierPresenceConnus())
    {
        QMessageBox::information(this, "Corruption du fichier", "Des électeurs référencés ne sont pas présents dans le fichier.");
    }
    const QJsonArray votesSecrets = corpus["VotesSecrets"].toArray();
    for (const auto &v : votesSecrets)
    {
        QJsonObject jobject = v.toObject();
        for (const shared_ptr<Question> &q : _questions)
        {
            if (q->question() != jobject["Question"].toString())
            {
                continue;
            }
            for (const auto & vote : jobject["Votes"].toArray())
            {
                Vote v(vote.toObject()["Vote"].toVariant());
                v.setChecksum(vote.toObject()["VoteChecksum"].toString());
                v.setClefPublique(vote.toObject()["VoteClefPublique"].toString().toUtf8());
                if (!v.verifie(q->question()))
                {
                    QMessageBox::information(this, "Corruption du fichier", QString("Les votes à bulletins secrets à la question %1 ont été corrompus").arg(q->question()));
                }
                _votesSecrets[q].push_back(v);
            }
        }
    }
    creerScene();
}

bool MainResPublica::verifierPresenceConnus()
{
    bool trouve = true;
    QStringList pseudos;
    for (const auto &p : _personnes)
    {
        pseudos << p->pseudonyme();
    }
    for (const auto &p : _personnes)
    {
        const auto connus = p->electeursConnus();
        for (const auto &c : connus)
        {
            trouve &= pseudos.contains(c) || _desinscriptions.contains(c);
            if (!trouve) break;
        }
        if (!trouve) break;
    }
    return trouve;
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
        connect(item, &QuestionGraphicItem::AVoteSecret, this, &MainResPublica::on_AVoteSecret);
        connect(item, &QuestionGraphicItem::montrerResultats, this, &MainResPublica::on_MontrerResultats);
    }
}

void MainResPublica::on_AVote(std::shared_ptr<Question> question, QVariant choix)
{
    _electeurCour->addVote(question, choix, false);
}

void MainResPublica::on_AVoteSecret(std::shared_ptr<Question> question, QVariant choix)
{
    Vote vote(choix, false);
    if (vote.checksum().isEmpty())
    {
        vote.setChecksum(vote.signe(question->question()));
    }
    _votesSecrets[question].push_back(vote);
    auto clefChiffree = _electeurCour->chiffreClefPublique(vote.checksum());
    _electeurCour->addVote(question, clefChiffree, false);
}

void MainResPublica::on_MontrerResultats(std::shared_ptr<Question> question)
{
    list<Vote> votes;
    for(const auto &p : _personnes)
    {
        votes.push_back(p->votes()[question]);
    }
    for(const auto &p : _votesSecrets[question])
    {
        votes.push_back(p);
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
        ui->actionSe_d_sinscrire->setEnabled(true);
    }
    creerScene();
}


void MainResPublica::on_actionSe_d_sinscrire_triggered()
{
    if (!_electeurCour)
        return;
    _electeurCour->supprimeVotesSecrets(_votesSecrets);
    _desinscriptions << _electeurCour->pseudonyme();
}

