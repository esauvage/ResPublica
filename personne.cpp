#include "personne.h"

Personne::Personne() {}

void Personne::addVote(const Vote &vote)
{
    _votes.push_back(vote);
}
