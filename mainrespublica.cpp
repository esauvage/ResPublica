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
    _votes.push_back(nouveauQuestion);
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
    _votes.push_back(nouveauQuestion);
    QuestionGraphicItem * voteItem = new QuestionGraphicItem(QuestionGraphicItem::Step, nouveauQuestion);
    voteItem->setBrush(Qt::white);
    _scene->addItem(voteItem);
    voteItem->setPos(pos);
    connect(voteItem, &QuestionGraphicItem::AVote, this, &MainResPublica::on_AVote);
}


void MainResPublica::on_actionEnregistrer_triggered()
{
    QFile sortie("Questions.txt");
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
    QFile entree("Questions.txt");
    if (!entree.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    _votes.clear();
    _scene->clear();
    QByteArray saveData = entree.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    const QJsonArray votes = loadDoc.array();
    for (const QJsonValue &vote : votes)
    {
        auto nouveauQuestion = make_shared<QuestionListe>();
        QJsonObject jobject = vote.toObject();
        nouveauQuestion->setId(jobject["id"].toInteger());
        nouveauQuestion->setQuestion(jobject["Question"].toString());
        nouveauQuestion->setChoix(jobject["Choix"].toVariant());
        _votes.push_back(nouveauQuestion);
        auto item = new QuestionGraphicItem(QuestionGraphicItem::Step, nouveauQuestion);
        item->setBrush(Qt::white);
        _scene->addItem(item);
        item->setSelected(true);
        item->setPos(100 * nouveauQuestion->id(), 100 * nouveauQuestion->id());
    }

    // QTextStream in(&entree);
    // while (!in.atEnd())
    // {
    //     auto nouveauQuestion = make_shared<QuestionListe>();
    //     in >> nouveauQuestion;
    //     if (nouveauQuestion->question().isEmpty())
    //         break;
    //     _votes.push_back(nouveauQuestion);
    //     auto item = new QuestionGraphicItem(QuestionGraphicItem::Step, nouveauQuestion);
    //     item->setBrush(Qt::white);
    //     scene->addItem(item);
    //     item->setSelected(true);
    //     item->setPos(100 * nouveauQuestion->id(), 100 * nouveauQuestion->id());
    // }
}

void MainResPublica::on_AVote(std::shared_ptr<Question> question, QVariant choix)
{

}

