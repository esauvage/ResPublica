#include "dlglistevote.h"
#include "ui_dlglistevote.h"

DlgListeVote::DlgListeVote(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgListeVote)
{
    ui->setupUi(this);
}

DlgListeVote::~DlgListeVote()
{
    delete ui;
}

void DlgListeVote::setPossibilites(const QStringList &v)
{
    ui->lstPossibles->addItems(v);
}

void DlgListeVote::on_btnSelect_clicked()
{
    for (auto s : ui->lstPossibles->selectedItems())
    {
        ui->lstSelectionnes->addItem(s->text());
        ui->lstPossibles->removeItemWidget(s);
        delete s;
    }
}


void DlgListeVote::on_btnDeselect_clicked()
{
    for (auto s : ui->lstSelectionnes->selectedItems())
    {
        ui->lstPossibles->addItem(s->text());
        ui->lstSelectionnes->removeItemWidget(s);
        delete s;
    }
}

