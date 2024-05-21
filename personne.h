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

private:
    std::map<std::shared_ptr<Question>, Vote> _votes;
};

#endif // PERSONNE_H
