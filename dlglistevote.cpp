#include "dlglistevote.h"
#include "ui_dlglistevote.h"

DlgListeQuestion::DlgListeQuestion(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgListeQuestion)
{
    ui->setupUi(this);
}

DlgListeQuestion::~DlgListeQuestion()
{
    delete ui;
}

void DlgListeQuestion::setPossibilites(const QStringList &v)
{
    ui->lstPossibles->addItems(v);
}

QStringList DlgListeQuestion::selection() const
{
    QStringList r;
    for (auto s : ui->lstSelectionnes->findItems("", Qt::MatchContains))
    {
        r << s->text();
    }
    return r;
}

void DlgListeQuestion::on_btnSelect_clicked()
{
    for (auto s : ui->lstPossibles->selectedItems())
    {
        ui->lstSelectionnes->addItem(s->text());
        ui->lstPossibles->removeItemWidget(s);
        delete s;
    }
}


void DlgListeQuestion::on_btnDeselect_clicked()
{
    for (auto s : ui->lstSelectionnes->selectedItems())
    {
        ui->lstPossibles->addItem(s->text());
        ui->lstSelectionnes->removeItemWidget(s);
        delete s;
    }
}

