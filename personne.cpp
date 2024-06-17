#include "personne.h"

#include "vote.h"
#include "question.h"
#include "cipher.h"

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

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

QString Personne::calculElecteursCheckSum() const
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

QByteArray Personne::checksumVotes() const
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
    return hasher.hash(votes.join(";").toLocal8Bit(), QCryptographicHash::Blake2s_256);
}

QString Personne::signatureVotes() const
{
    Cipher cipher;
    auto privateKey = cipher.getPrivateKey(QString("%1.pem").arg(pseudonyme()));
    return cipher.encryptPrivateRSA(privateKey, checksumVotes());
}

bool Personne::verifierVotes()
{
    Cipher cipher;
    auto publicKey = cipher.getPublicKey(_clefPublique);
    auto code = _votesChecksum.toLocal8Bit();
    auto decode = cipher.decryptPublicRSA(publicKey, code);
    auto checksum = checksumVotes();
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

QString Personne::chiffreClefPublique(const QString &clair)
{
    Cipher cipher;
    auto publicKey = cipher.getPublicKey(_clefPublique);
    QCryptographicHash hasher(QCryptographicHash::Blake2s_256);
    auto decode = hasher.hash(clair.toLocal8Bit(), QCryptographicHash::Blake2s_256);
    return QString::fromLocal8Bit(cipher.encryptRSA(publicKey, decode));
}

QString Personne::chiffreClefPrivee(const QString &clair)
{
    Cipher cipher;
    auto publicKey = cipher.getPrivateKey(QString("%1.pem").arg(pseudonyme()));
    auto decode = clair.toLocal8Bit();
    return cipher.encryptPrivateRSA(publicKey, decode);
}

QString Personne::dechiffreClefPublique(const QString &chiffre)
{
    Cipher cipher;
    auto publicKey = cipher.getPublicKey(_clefPublique);
    auto decode = chiffre.toLocal8Bit();
    return cipher.decryptPublicRSA(publicKey, decode);
}

QString Personne::dechiffreClefPrivee(const QString &clair) const
{
    Cipher cipher;
    auto privateKey = cipher.getPrivateKey(QString("%1.pem").arg(pseudonyme()));
    auto code = clair.toLocal8Bit();
    return QString::fromLocal8Bit(cipher.decryptRSA(privateKey, code));
}

std::list<Vote>::iterator Personne::trouverVoteSecret(const QString &vote, list<Vote> &votesSecrets) const
{
    auto checksum = dechiffreClefPrivee(vote).toLocal8Bit();
    list<Vote>::iterator i = votesSecrets.begin();
    while (i != votesSecrets.end())
    {
        QCryptographicHash hasher(QCryptographicHash::Blake2s_256);
        auto decode = hasher.hash((*i).checksum().toLocal8Bit(), QCryptographicHash::Blake2s_256);
        if (decode == checksum)
        {
            return i;
        }
        ++i;
    }
    return i;
}

void Personne::lireJson(const QJsonObject &json, const list<shared_ptr<Question> > &questions)
{
    const QJsonArray connus = json["ElecteursConnus"].toArray();
    QStringList c;
    for (const auto & i : connus)
    {
        c << i.toString();
    }
    setElecteursConnus(c);
    setElecteursChecksum(json["ElecteursCheckSum"].toString());
    if (!verifierElecteurs(json["ElecteursCheckSum"].toString()))
    {
        QMessageBox::information(nullptr, "Corruption du fichier", "La liste des électeurs a été corrompue");
    }
    const QJsonArray votes = json["Votes"].toArray();
    for (const auto &v : votes)
    {
        QJsonObject jobject = v.toObject();
        for (const shared_ptr<Question> &q : questions)
        {
            if (q->question() != jobject["Question"].toString())
            {
                continue;
            }
            bool aVerifier = QString(q->checksum().toBase64()) != jobject["QuestionChecksum"].toString();
            addVote(q, jobject["Choix"].toVariant(), aVerifier);
        }
    }
    setVotesChecksum(json["VotesChecksum"].toString());
    if (!verifierVotes())
    {
        QMessageBox::information(nullptr, "Corruption du fichier", QString("Les votes de %1 ont été corrompus").arg(pseudonyme()));
    }
}

QJsonObject Personne::ecrireJson() const
{
    QJsonObject personne;
    QJsonArray votes;
    for (const auto &v : _votes)
    {
        QJsonObject jobject;
        jobject["Question"] = v.first->question();
        jobject["QuestionChecksum"] = QString(v.first->checksum().toBase64());
        jobject["Choix"] = QJsonValue::fromVariant(v.second.choix());
        votes.append(jobject);
    }
    personne["Votes"] = votes;
    personne["VotesChecksum"] = votesChecksum();
    personne["Pseudo"] = pseudonyme();
    personne["ClefPublique"] = QString::fromUtf8(clefPublique());
    personne["ElecteursConnus"] = QJsonArray::fromStringList(electeursConnus());
    personne["ElecteursCheckSum"] = electeursChecksum();
    return personne;
}

void Personne::supprimeVotesSecrets(map<shared_ptr<Question>, list<Vote> > &votesSecrets) const
{
    for (const auto & v : votes())
    {
        if (!v.first->choix().toStringList().contains(v.second.choix().toString()))
        {
            auto toDelete = trouverVoteSecret(v.second.choix().toString(), votesSecrets[v.first]);
            if (toDelete != votesSecrets[v.first].end())
            {
                votesSecrets[v.first].erase(toDelete);
            }
            // auto checksum = dechiffreClefPrivee(v.second.choix().toString()).toLocal8Bit();
            // auto list = votesSecrets[v.first];
            // std::list<Vote>::iterator i = list.begin();
            // while (i != list.end())
            // {
            //     QCryptographicHash hasher(QCryptographicHash::Blake2s_256);
            //     auto decode = hasher.hash((*i).checksum().toLocal8Bit(), QCryptographicHash::Blake2s_256);

            //     bool isActive = (decode == checksum);
            //     if (isActive)
            //     {
            //         i = list.erase(i);
            //     }
            //     else
            //     {
            //         ++i;
            //     }
            // }
            // votesSecrets[v.first] = list;
        }
    }

}
