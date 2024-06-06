#include "dlgconnexion.h"
#include "ui_dlgconnexion.h"

#include <QCryptographicHash>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

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
    return hasher.hash(QString(ui->edtPasse->text() + _sel).toLocal8Bit(), QCryptographicHash::Blake2s_256);
}

void DlgConnexion::accept()
{
        QSqlQuery resultat;
        resultat.prepare("SELECT SEL, HASH FROM UTILISATEURS WHERE NOM = ?");
        resultat.bindValue(0, ui->edtPseudo->text());
        resultat.exec();
        if (resultat.next())
        {
            _sel = resultat.value(0).toString();
            if (hash() == resultat.value(1).toString())
            {
                QDialog::accept();
                return;
            }
            ui->edtPasse->clear();
        }
        else
        {
            _sel = QDateTime::currentDateTime().toString("ddMMyyyyTHHmmss");
            resultat.prepare("INSERT INTO UTILISATEURS (NOM, SEL, HASH) VALUES (?, ?, ?)");
            resultat.bindValue(0, ui->edtPseudo->text());
            resultat.bindValue(1, _sel);
            resultat.bindValue(2, hash());
            resultat.exec();
            if (resultat.lastError().isValid())
            {
                qDebug() << resultat.lastQuery();
                qDebug() << resultat.lastError().text();
            }
            else
            {
                QDialog::accept();
            }
        }
}

