#include "mainrespublica.h"
#include "ui_mainrespublica.h"

#include <QFile>

#include "dlgeditvote.h"
#include "VoteScene.h"

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
    auto nouveauVote = make_shared<Vote>();
    _votes.push_back(nouveauVote);
    DlgEditVote dlg(nouveauVote, this);
    dlg.exec();
}

void MainResPublica::itemInserted(QPointF pos)
{
    auto nouveauVote = make_shared<Vote>();
    _votes.push_back(nouveauVote);
    DlgEditVote dlg(nouveauVote, this);
    dlg.exec();
    auto voteItem = new VoteGraphicItem(VoteGraphicItem::Step, nouveauVote->question());
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
    for (const auto &v : _votes)
    {
        out << v << "\n";
    }
    sortie.close();
}


void MainResPublica::on_actionOuvrir_triggered()
{
    QFile entree("Votes.txt");
    if (!entree.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&entree);
    while (!in.atEnd())
    {
        auto nouveauVote = make_shared<Vote>();
        in >> nouveauVote;
        if (nouveauVote->question().isEmpty())
            break;
        _votes.push_back(nouveauVote);
        auto item = new VoteGraphicItem(VoteGraphicItem::Step);
        item->setBrush(Qt::white);
        scene->addItem(item);
        item->setSelected(true);
        item->setPos(100, 100);
    }
}

