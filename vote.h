#ifndef VOTE_H
#define VOTE_H

#include <QVariant>

class Vote
{
public:
    Vote(QVariant choix = QVariant(), bool aVerifier = false);

    QVariant choix() const;
    void setChoix(const QVariant &newChoix);

    bool aConfirmer() const;
    void setAConfirmer(bool newAConfirmer);

    QString checksum() const;
    void setChecksum(const QString &newChecksum);

    QString signe(const QString &question);
    bool verifie(const QString &question);

    QByteArray clefPublique() const;
    void setClefPublique(const QByteArray &newClefPublique);

private:
    QByteArray checksumVotes(const QString &question) const;

    QVariant _choix;
    QByteArray _clefPublique;
    QString _checksum;
    bool _aConfirmer;
};

#endif // VOTE_H
