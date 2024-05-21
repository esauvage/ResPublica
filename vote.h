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

private:
    QVariant _choix;
    bool _aConfirmer;
};

#endif // VOTE_H
