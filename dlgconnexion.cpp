#include "dlgconnexion.h"
#include "ui_dlgconnexion.h"

DlgConnexion::DlgConnexion(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgConnexion)
{
    ui->setupUi(this);
}

DlgConnexion::~DlgConnexion()
{
    delete ui;
}

const QString DlgConnexion::pseudonyme() const
{
    return ui->edtPseudo->text();
}
