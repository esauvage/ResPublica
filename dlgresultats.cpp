#include "dlgresultats.h"
#include "ui_dlgresultats.h"

using namespace std;

DlgResultats::DlgResultats(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgResultats)
{
    ui->setupUi(this);
}

DlgResultats::~DlgResultats()
{
    delete ui;
}

void DlgResultats::setVotes(const std::list<Vote> &newVotes)
{
    _votes = newVotes;
    QStringList candidats;
    for (const auto & v : newVotes)
    {
       candidats += v.choix().toStringList();
    }
    candidats.removeDuplicates();
    //On simule les duels sur chaque bulletin
    map<QPair<QString, QString>, int> duels;
    for (const auto & v : newVotes)
    {
        QStringList liste = v.choix().toStringList();
        for (int i = 0; i < liste.length(); ++i)
        {
            for (int j = i+1; j < liste.length(); ++j)
            {
                //Le premier des deux a +1 à son duel, le second -1.
                duels[QPair<QString, QString>(liste.at(i), liste.at(j))]++;
                duels[QPair<QString, QString>(liste.at(j), liste.at(i))]--;
            }
        }
    }
    //On compte le nombre de duels remportés par chaque candidat
    map<QString, int> resultats;
    for (const auto & v : duels)
    {
        QString candidat = v.first.first;
        if (v.second > 0)
        {
            resultats[candidat]++;
        }
    }
    multimap<int, QString> vainqueurs;
    for (const auto & v : resultats)
    {
        vainqueurs.insert(pair<int, QString>(v.second, v.first));
    }
    ui->lstResultats->setColumnCount(2);
    for (const auto & v : vainqueurs)
    {
        auto item = new QTreeWidgetItem();
        item->setForeground(1, QBrush(QColor(255, 0, 0)));
        item->setText(0, v.second);
        item->setText(1, QString("%1").arg(v.first));
        ui->lstResultats->addTopLevelItem(item);
    }
}
