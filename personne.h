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

    QString pseudonyme() const;
    void setPseudonyme(const QString &newPseudonyme);

private:
    std::map<std::shared_ptr<Question>, Vote> _votes;
    QString _pseudonyme;
};

#endif // PERSONNE_H
