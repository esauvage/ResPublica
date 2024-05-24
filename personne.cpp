#include "personne.h"

#include "vote.h"
#include "question.h"
#include "cipher.h"

#include <QCryptographicHash>

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

QString Personne::signatureVotes() const
{
    QStringList votes;
    for (const auto &v : _votes)
    {
        votes << v.first->question();
        for (const auto &c : v.second.choix().toStringList()) {
            votes << c;
        }
    }
    QCryptographicHash hasher(QCryptographicHash::Blake2s_256);
    QByteArray checksum = hasher.hash(votes.join(";").toUtf8(), QCryptographicHash::Blake2s_256);
    Cipher cipher;
    auto privateKey = cipher.getPrivateKey(QString("%1.pem").arg(pseudonyme()));
    return QString(cipher.encryptPrivateRSA(privateKey, checksum).toBase64());
}

bool Personne::verifierVotes()
{
    QStringList votes;
    for (const auto &v : _votes)
    {
        votes << v.first->question();
        for (const auto &c : v.second.choix().toStringList()) {
            votes << c;
        }
    }
    QCryptographicHash hasher(QCryptographicHash::Blake2s_256);
    QByteArray checksum = hasher.hash(votes.join(";").toUtf8(), QCryptographicHash::Blake2s_256);
    Cipher cipher;
    auto publicKey = cipher.getPublicKey(_clefPublique);
    auto code = QByteArray::fromBase64(_votesChecksum.toUtf8());
    auto decode = cipher.decryptPublicRSA(publicKey, code);
    return (decode == checksum);
}

QString Personne::votesChecksum() const
{
    return _votesChecksum;
}

void Personne::setVotesChecksum(const QString &newVotesChecksum)
{
    _votesChecksum = newVotesChecksum;
}
