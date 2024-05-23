#include "personne.h"

#include "vote.h"
#include "cipher.h"

Personne::Personne() {}

using namespace std;
void Personne::addVote(std::shared_ptr<Question> question, QVariant choix, bool aVerifier)
{
    _votes[question] = Vote(choix, aVerifier);
}

map<shared_ptr<Question>, Vote> Personne::votes() const
{
    return _votes;
}

void Personne::deleteVotes()
{
    _votes.clear();
}

QString Personne::pseudonyme() const
{
    return _pseudonyme;
}

void Personne::setPseudonyme(const QString &newPseudonyme)
{
    _pseudonyme = newPseudonyme;
}

QByteArray Personne::clefPublique() const
{
    return _clefPublique;
}

void Personne::setClefPublique(const QByteArray &newClefPublique)
{
    _clefPublique = newClefPublique;
}

QStringList Personne::electeursConnus() const
{
    return _electeursConnus;
}

void Personne::setElecteursConnus(const QStringList &newElecteursConnus)
{
    _electeursConnus = newElecteursConnus;
}

QByteArray Personne::calculElecteursCheckSum() const
{
    Cipher cipher;
    auto privateKey = cipher.getPrivateKey(QString("%1.pem").arg(pseudonyme()));
    QByteArray electeurs = _electeursConnus.join(";").toUtf8();
    return cipher.encryptPrivateRSA(privateKey, electeurs);
}

bool Personne::verifierElecteurs(const QString v)
{
    Cipher cipher;
    auto publicKey = cipher.getPublicKey(_clefPublique);
    auto code = QByteArray::fromBase64(v.toUtf8());
    auto decode = cipher.decryptPublicRSA(publicKey, code);
    return decode == _electeursConnus.join(";").toUtf8();
}

void Personne::setElecteursChecksum(const QString &newElecteursChecksum)
{
    _electeursChecksum = newElecteursChecksum;
}

QString Personne::electeursChecksum() const
{
    return _electeursChecksum;
}
