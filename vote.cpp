#include "vote.h"

#include <QCryptographicHash>
#include <QStringList>
#include <QProcess>

#include "cipher.h"

Vote::Vote(QVariant choix, bool aVerifier)
    :_choix(choix), _aConfirmer(aVerifier)
{

}

QVariant Vote::choix() const
{
    return _choix;
}

void Vote::setChoix(const QVariant &newChoix)
{
    _choix = newChoix;
}

bool Vote::aConfirmer() const
{
    return _aConfirmer;
}

void Vote::setAConfirmer(bool newAConfirmer)
{
    _aConfirmer = newAConfirmer;
}

QString Vote::checksum() const
{
    return _checksum;
}

void Vote::setChecksum(const QString &newChecksum)
{
    _checksum = newChecksum;
}

QString Vote::signe(const QString &question)
{
    Cipher cipher;
    //Creation de la paire de clefs
    QString program = "openssl";
    QStringList arguments;
    QString nom = cipher.randomBytes(8).toBase64();
    arguments << "genrsa" << "-out" << QString("%1.pem").arg(nom) << "2048";

    QProcess *myProcess = new QProcess(nullptr);
    myProcess->start(program, arguments);
    if (!myProcess->waitForFinished())
    {
        //TODO : gestion erreur
    }
    qDebug() << myProcess->error() << myProcess->readAllStandardOutput();
    delete myProcess;

    arguments.clear();
    arguments << "rsa" << "-in" << QString("%1.pem").arg(nom) << "-pubout" << "-out" << QString("%1.crt").arg(nom);
    myProcess = new QProcess(nullptr);
    myProcess->start(program, arguments);
    if (!myProcess->waitForFinished())
    {
        //TODO : gestion erreur
    }
    delete myProcess;
    QFile clefPublique(QString("%1.crt").arg(nom));
    if (!clefPublique.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Fichier illisible";
    }
    setClefPublique(clefPublique.readAll());
    clefPublique.close();
    auto privateKey = cipher.getPrivateKey(QString("%1.pem").arg(nom));
    QString ret = QString::fromLocal8Bit(cipher.encryptPrivateRSA(privateKey, checksumVotes(question)));
    cipher.freeRSAKey(privateKey);
    QFile file (QString("%1.pem").arg(nom));
    file.remove();
    clefPublique.remove();
    return ret;
}

bool Vote::verifie(const QString &question)
{
    Cipher cipher;
    auto publicKey = cipher.getPublicKey(_clefPublique);
    auto code = _checksum.toLocal8Bit();
    auto decode = cipher.decryptPublicRSA(publicKey, code);
    return (decode == checksumVotes(question));
}

QByteArray Vote::checksumVotes(const QString &question) const
{
    QStringList votes;
    votes << question;
    for (const auto &c : _choix.toStringList()) {
        votes << c;
    }
    QCryptographicHash hasher(QCryptographicHash::Blake2s_256);
    return hasher.hash(votes.join(";").toLocal8Bit(), QCryptographicHash::Blake2s_256);
}

QByteArray Vote::clefPublique() const
{
    return _clefPublique;
}

void Vote::setClefPublique(const QByteArray &newClefPublique)
{
    _clefPublique = newClefPublique;
}
