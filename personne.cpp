#include "personne.h"

#include "vote.h"

Personne::Personne() {}

using namespace std;
void Personne::addVote(std::shared_ptr<Question> question, QVariant choix, bool aVerifier)
{
    _votes.insert(pair<std::shared_ptr<Question>, Vote>(question, Vote(choix, aVerifier)));
}

map<shared_ptr<Question>, Vote> Personne::votes() const
{
    return _votes;
}

QString Personne::pseudonyme() const
{
    return _pseudonyme;
}

void Personne::setPseudonyme(const QString &newPseudonyme)
{
    _pseudonyme = newPseudonyme;
}
