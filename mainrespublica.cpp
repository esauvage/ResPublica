#include "mainrespublica.h"
#include "ui_mainrespublica.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "dlgeditvote.h"
#include "VoteScene.h"
#include "voteliste.h"

#include <QDebug>

using namespace std;

MainResPublica::MainResPublica(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainResPublica)
{
    ui->setupUi(this);
    scene = new VoteScene(this);
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));
    ui->mainView->setScene(scene);
    connect(scene, &VoteScene::itemInserted, this, &MainResPublica::itemInserted);
//    connect(scene, &VoteScene::textInserted,
//            this, &MainWindow::textInserted);
//    connect(scene, &VoteScene::itemSelected,
//            this, &MainWindow::itemSelected);
}

MainResPublica::~MainResPublica()
{
    delete ui;
    delete scene;
}


void MainResPublica::on_actionCr_er_triggered()
{
    auto nouveauVote = make_shared<VoteListe>();
    _votes.push_back(nouveauVote);
    DlgEditVote dlg(nouveauVote, this);
    dlg.exec();
}

void MainResPublica::itemInserted(QPointF pos)
{
    auto nouveauVote = make_shared<VoteListe>();
    DlgEditVote dlg(nouveauVote, this);
    if (!dlg.exec())
    {
        return;
    }
    _votes.push_back(nouveauVote);
    auto voteItem = new VoteGraphicItem(VoteGraphicItem::Step, nouveauVote);
    voteItem->setBrush(Qt::white);
    scene->addItem(voteItem);
    voteItem->setPos(pos);
}


void MainResPublica::on_actionEnregistrer_triggered()
{
    QFile sortie("Votes.txt");
    if (!sortie.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&sortie);
    QJsonArray votes;
    for (const auto &v : _votes)
    {
        QJsonObject jobject;
        jobject["id"] = QJsonValue::fromVariant(v->id());
        jobject["Question"] = v->question();
        jobject["Choix"] = QJsonValue::fromVariant(v->choix());
        votes.append(jobject);
    }
    QJsonDocument doc( votes );
    out << doc.toJson() << "\n";
    sortie.close();
}


void MainResPublica::on_actionOuvrir_triggered()
{
    QFile entree("Votes.txt");
    if (!entree.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    _votes.clear();
    scene->clear();
    QByteArray saveData = entree.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    const QJsonArray votes = loadDoc.array();
    for (const QJsonValue &vote : votes)
    {
        auto nouveauVote = make_shared<VoteListe>();
        QJsonObject jobject = vote.toObject();
        nouveauVote->setId(jobject["id"].toInteger());
        nouveauVote->setQuestion(jobject["Question"].toString());
        nouveauVote->setChoix(jobject["Choix"].toVariant());
        _votes.push_back(nouveauVote);
        auto item = new VoteGraphicItem(VoteGraphicItem::Step, nouveauVote);
        item->setBrush(Qt::white);
        scene->addItem(item);
        item->setSelected(true);
        item->setPos(100 * nouveauVote->id(), 100 * nouveauVote->id());
    }

    // QTextStream in(&entree);
    // while (!in.atEnd())
    // {
    //     auto nouveauVote = make_shared<VoteListe>();
    //     in >> nouveauVote;
    //     if (nouveauVote->question().isEmpty())
    //         break;
    //     _votes.push_back(nouveauVote);
    //     auto item = new VoteGraphicItem(VoteGraphicItem::Step, nouveauVote);
    //     item->setBrush(Qt::white);
    //     scene->addItem(item);
    //     item->setSelected(true);
    //     item->setPos(100 * nouveauVote->id(), 100 * nouveauVote->id());
    // }
}

