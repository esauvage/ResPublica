#include "mainrespublica.h"
#include "ui_mainrespublica.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "dlgeditvote.h"
#include "VoteScene.h"
#include "questionliste.h"

#include <QDebug>

using namespace std;

MainResPublica::MainResPublica(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainResPublica)
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
    QuestionGraphicItem * voteItem = new QuestionGraphicItem(QuestionGraphicItem::Step, nouveauQuestion, _personne);
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
    for (auto &v : _questions)
    {
        QJsonObject jobject;
        jobject["id"] = QJsonValue::fromVariant(v->id());
        jobject["Question"] = v->question();
        jobject["Choix"] = QJsonValue::fromVariant(v->choix());
        questions.append(jobject);
    }
    QJsonObject corpus;
    corpus["questions"] = questions;
    QJsonArray votes;
    for (const auto &v : _personne.votes())
    {
        QJsonObject jobject;
        jobject["Question"] = v.first->question();
        jobject["QuestionChecksum"] = QJsonValue::fromVariant(QString(v.first->checksum().toBase64()));
        jobject["Choix"] = QJsonValue::fromVariant(v.second.choix());
        votes.append(jobject);
    }
    QJsonObject personne;
    personne["Votes"] = votes;
    corpus["personne"] = personne;
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
    _scene->clear();
    QByteArray saveData = entree.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    const QJsonObject corpus = loadDoc.object();
    const QJsonArray questions = corpus["questions"].toArray();
    map<shared_ptr<Question>, QuestionGraphicItem *>itemsQuestions;
    for (const QJsonValue &q : questions)
    {
        auto nouveauQuestion = make_shared<QuestionListe>();
        QJsonObject jobject = q.toObject();
        nouveauQuestion->setId(jobject["id"].toInteger());
        nouveauQuestion->setQuestion(jobject["Question"].toString());
        nouveauQuestion->setChoix(jobject["Choix"].toVariant());
        _questions.push_back(nouveauQuestion);
        auto item = new QuestionGraphicItem(QuestionGraphicItem::Step, nouveauQuestion, _personne);
        itemsQuestions.insert(QPair<shared_ptr<Question>, QuestionGraphicItem *>(nouveauQuestion, item));
        item->setBrush(Qt::white);
        _scene->addItem(item);
        item->setSelected(true);
        item->setPos(100 * nouveauQuestion->id(), 100 * nouveauQuestion->id());
    }
    const QJsonObject personne = corpus["personne"].toObject();
    const QJsonArray votes = personne["Votes"].toArray();
    for (const auto &v : votes)
    {
        QJsonObject jobject = v.toObject();
        for (shared_ptr<Question> q : _questions)
        {
            if (q->question() != jobject["Question"].toString())
            {
                continue;
            }
            bool aVerifier = QString(q->checksum().toBase64()) != jobject["QuestionChecksum"].toString();
            _personne.addVote(q, jobject["Choix"].toVariant(), aVerifier);
            itemsQuestions[q]->setBrush(aVerifier ? Qt::red : Qt::white);
        }
    }
}

void MainResPublica::on_AVote(std::shared_ptr<Question> question, QVariant choix)
{
    _personne.addVote(question, choix, false);
}

