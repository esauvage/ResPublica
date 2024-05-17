#ifndef PERSONNE_H
#define PERSONNE_H

#include <list>

#include "vote.h"

class Personne
{
public:
    Personne();
    void addVote(const Vote &vote);
private:
    std::list<Vote> _votes;
};

#endif // PERSONNE_H
