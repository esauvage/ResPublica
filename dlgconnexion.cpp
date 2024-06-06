#include "dlgconnexion.h"
#include "ui_dlgconnexion.h"

#include <QCryptographicHash>

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

void DlgConnexion::setSel(QString v)
{
    _sel = v;
}

const QByteArray DlgConnexion::hash() const
{
    QCryptographicHash hasher(QCryptographicHash::Blake2s_256);
    return hasher.hash(QString(ui->edtPseudo->text() + _sel).toLocal8Bit(), QCryptographicHash::Blake2s_256);
}

