#ifndef PERSONNE_H
#define PERSONNE_H

#include "vote.h"

class Question;

class Personne
{
public:
    Personne();
    void addVote(std::shared_ptr<Question> question, QVariant choix, bool aVerifier = true);
    std::map<std::shared_ptr<Question>, Vote> votes() const;
    void deleteVotes();

    QString pseudonyme() const;
    void setPseudonyme(const QString &newPseudonyme);

    QByteArray clefPublique() const;
    void setClefPublique(const QByteArray &newClefPublique);

    QStringList electeursConnus() const;
    void setElecteursConnus(const QStringList &newElecteursConnus);

    QByteArray calculElecteursCheckSum() const;
    bool verifierElecteurs(const QString v);

    void setElecteursChecksum(const QString &newElecteursChecksum);
    QString electeursChecksum() const;

    QString signatureVotes() const;
    bool verifierVotes();

    QString votesChecksum() const;
    void setVotesChecksum(const QString &newVotesChecksum);

private:
    std::map<std::shared_ptr<Question>, Vote> _votes;
    QString _pseudonyme;
    QByteArray _clefPublique;
    QStringList _electeursConnus;
    QString _electeursChecksum;
    QString _votesChecksum;
};

#endif // PERSONNE_H
