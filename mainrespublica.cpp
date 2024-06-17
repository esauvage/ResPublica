#include "mainrespublica.h"
#include "ui_mainrespublica.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>

#include <QMessageBox>

#include <QProcess>
#include <QSettings>

#include "dlgeditvote.h"
#include "VoteScene.h"
#include "fabriquequestions.h"
#include "questionliste.h"
#include "dlgconnexion.h"
#include "dlgresultats.h"

#include <QSqlQuery>

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
    setDatabase();
//    connect(scene, &VoteScene::textInserted,
//            this, &MainWindow::textInserted);
//    connect(scene, &VoteScene::itemSelected,
//            this, &MainWindow::itemSelected);
}

//Initialise la base de données et la crée au besoin.
void MainResPublica::setDatabase()
{
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    QSettings settings;
    database.setDatabaseName(settings.value("DatabaseName", "respublica.db3").toString());
    database.open();
    QSqlQuery sql("PRAGMA TABLE_INFO([UTILISATEURS])");
    if (!sql.next())
    {
        //La table des utilisateurs
        QSqlQuery("CREATE TABLE `UTILISATEURS` (ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, NOM VARCHAR(50) NOT NULL, "
                  "SEL VARCHAR(50) NOT NULL, HASH VARCHAR(50) NOT NULL)");
        //La table des positions des questions
        QSqlQuery("CREATE TABLE `POSITIONS_QUESTIONS` (ID INTEGER NOT NULL PRIMARY KEY, QUESTION VARCHAR(50) NOT NULL, "
                  "X INTEGER NOT NULL, Y INTEGER NOT NULL)");
    }
}

shared_ptr<Personne> MainResPublica::electeurCourant() const
{
    return _electeurCour;
}

list<shared_ptr<Question> > MainResPublica::questions() const
{
    return _questions;
}

MainResPublica::~MainResPublica()
{
    delete ui;
    delete _scene;
}

void MainResPublica::on_actionCr_er_triggered()
{
    itemInserted(QPointF());
}

void MainResPublica::itemInserted(QPointF pos)
{
    DlgEditQuestion dlg(nullptr, this);
    dlg.setTypes(FabriqueQuestions::getMapTypeToIds());
    if (!dlg.exec())
    {
        return;
    }
    // FabriqueQuestions fabrique;
    // auto nouveauQuestion = fabrique.creeScrutin(dlg.type());
    _questions.push_back(dlg.question());
    QuestionGraphicItem * voteItem = new QuestionGraphicItem(QuestionGraphicItem::Step, dlg.question(), _electeurCour);
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
        if ((p == _electeurCour) && (_desinscriptions.end() != std::find(_desinscriptions.begin(), _desinscriptions.end(), _electeurCour)))
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
                if (std::find(_desinscriptions.begin(), _desinscriptions.end(), e) != _desinscriptions.end()) continue;
                pseudos << e->pseudonyme();
            }
            p->setElecteursConnus(pseudos);
            personne["ElecteursConnus"] = QJsonArray::fromStringList(pseudos);
            p->setElecteursChecksum(p->calculElecteursCheckSum());
            personne["ElecteursCheckSum"] = p->electeursChecksum();
            for (const auto & q : _questions)
            {
                if (q->voteOuvert()) continue;
                list<Vote> votes;
                for(const auto &p : _personnes)
                {
                    votes.push_back(p->votes()[q]);
                }
                for(const auto &p : _votesSecrets[q])
                {
                    votes.push_back(p);
                }
                QStringList s;
                for (const auto & v : votes)
                {
                    s << v.choix().toStringList();
                }
                QCryptographicHash hasher(QCryptographicHash::Blake2s_256);
                auto hash = hasher.hash(s.join(";").toLocal8Bit(), QCryptographicHash::Blake2s_256);
                personne["VotesClos"] = p->chiffreClefPrivee(hash);
            }
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
        QJsonObject personne = v->ecrireJson();
        personne["Signature"] = v->chiffreClefPrivee(v->pseudonyme());
        desinscriptions.append(personne);
    }
    corpus["Desinscrits"] = desinscriptions;
    QJsonDocument doc( corpus );
    out << doc.toJson() << "\n";
    sortie.close();
    //Enregistrement de la scène en BDD
    _scene->sauvegarde();
}

void MainResPublica::on_actionOuvrir_triggered()
{
    QFile entree("Votes.txt");
    if (!entree.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    _questions.clear();
    _personnes.clear();
    _votesSecrets.clear();
    _desinscriptions.clear();

    QJsonDocument loadDoc(QJsonDocument::fromJson(entree.readAll()));
    const QJsonObject corpus = loadDoc.object();
    FabriqueQuestions fabriqueQuestions;
    fabriqueQuestions.lireJson(corpus["questions"].toArray(), _questions);

    const QJsonArray desinscriptions = corpus["Desinscrits"].toArray();
    for (const QJsonValue &d : desinscriptions)
    {
        const QJsonObject personne = d.toObject();
        shared_ptr<Personne> nouvelElecteur = make_shared<Personne>();
        nouvelElecteur->setPseudonyme(personne["Pseudo"].toString());
        nouvelElecteur->setClefPublique(personne["ClefPublique"].toString().toUtf8());
        auto signature = nouvelElecteur->dechiffreClefPublique(personne["Signature"].toString());
        if (nouvelElecteur->dechiffreClefPublique(personne["Signature"].toString()) != nouvelElecteur->pseudonyme())
        {
            QMessageBox::information(this, "Corruption du fichier", QString("%1 n'a pas signé lui-même sa révocation").arg(nouvelElecteur->pseudonyme()));
        }
        else
        {
            _desinscriptions.push_back(nouvelElecteur);
        }
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
        auto voteClos = nouvelElecteur->dechiffreClefPublique(personne["VotesClos"].toString());
        if (!voteClos.isEmpty())
        {
            for (const auto & q : _questions)
            {
                if (q->voteOuvert()) continue;
                list<Vote> votes;
                for(const auto &p : _personnes)
                {
                    votes.push_back(p->votes()[q]);
                }
                for(const auto &p : _votesSecrets[q])
                {
                    votes.push_back(p);
                }
                QStringList s;
                for (const auto & v : votes)
                {
                    s << v.choix().toStringList();
                }
                QCryptographicHash hasher(QCryptographicHash::Blake2s_256);
                auto hash = hasher.hash(s.join(";").toLocal8Bit(), QCryptographicHash::Blake2s_256);
                if (hash != voteClos)
                {
                    QMessageBox::warning(this, "Corruption du fichier", "Un scrutin clos a été modifié après sa clôture.");
                }
            }
        }
        _personnes.push_back(nouvelElecteur);
    }
    if (!verifierPresenceConnus())
    {
        QMessageBox::information(this, "Corruption du fichier", "Des électeurs référencés ne sont pas présents dans le fichier.");
    }
    //Suppression des désinscrits non référencés
    list <shared_ptr<Personne>> aSuppr;
    for (const auto & d : _desinscriptions)
    {
        bool reference = false;
        for (const auto & v : _personnes)
        {
            reference |= v->electeursConnus().contains(d->pseudonyme());
        }
        if (!reference)
        {
            aSuppr.push_back(d);
        }
    }
    for (const auto & d : aSuppr)
    {
        _desinscriptions.remove(d);
    }
    //Vote clos à vérifier après première vérification des gens
    for (const auto & personne : _personnes)
    {
    }
    _scene->creer(this);
}

bool MainResPublica::verifierPresenceConnus()
{
    bool trouve = true;
    QStringList pseudos;
    for (const auto &p : _personnes)
    {
        pseudos << p->pseudonyme();
    }
    for (const auto &p : _desinscriptions)
    {
        pseudos << p->pseudonyme();
    }
    for (const auto &p : _personnes)
    {
        const auto connus = p->electeursConnus();
        for (const auto &c : connus)
        {
            trouve &= pseudos.contains(c);
            if (!trouve) break;
        }
        if (!trouve) break;
    }
    return trouve;
}

// void MainResPublica::creerScene()
// {
//     //Création de l'IHM.
//     _scene->clear();
//     map<shared_ptr<Question>, QuestionGraphicItem *>itemsQuestions;
//     for (const auto &q : _questions)
//     {
//         auto item = new QuestionGraphicItem(QuestionGraphicItem::Step, q, _electeurCour);
//         itemsQuestions.insert(QPair<shared_ptr<Question>, QuestionGraphicItem *>(q, item));
//         if (_electeurCour)
//         {
//             itemsQuestions[q]->setBrush(_electeurCour->votes()[q].aConfirmer() ? Qt::red : Qt::white);
//         }
//         else
//         {
//             itemsQuestions[q]->setBrush(Qt::white);
//         }
//         _scene->addItem(item);
//         item->setSelected(true);
//         QPoint pos(100 * q->id(), 100 * q->id());
//         QSqlQuery sqlPos(QString("SELECT X, Y FROM POSITIONS_QUESTIONS WHERE ID = %1").arg(q->id()));
//         if (sqlPos.next())
//         {
//             pos.setX(sqlPos.value(0).toInt());
//             pos.setY(sqlPos.value(1).toInt());
//         }

//         item->setPos(pos);
//         connect(item, &QuestionGraphicItem::AVote, this, &MainResPublica::on_AVote);
//         connect(item, &QuestionGraphicItem::AVoteSecret, this, &MainResPublica::on_AVoteSecret);
//         connect(item, &QuestionGraphicItem::montrerResultats, this, &MainResPublica::on_MontrerResultats);
//     }
// }

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
    _scene->creer(this);
}

void MainResPublica::on_actionSe_d_sinscrire_triggered()
{
    if (!_electeurCour)
        return;
    _electeurCour->supprimeVotesSecrets(_votesSecrets);
    _electeurCour->deleteVotes();
    _desinscriptions.push_back(_electeurCour);
}
