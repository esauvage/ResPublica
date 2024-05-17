#include "personne.h"

Personne::Personne() {}

using namespace std;
void Personne::addVote(std::shared_ptr<Question> question, QVariant choix)
{
    _votes.insert(pair<std::shared_ptr<Question>, QVariant>(question, choix));
}

map<shared_ptr<Question>, QVariant> Personne::votes() const
{
    return _votes;
}
