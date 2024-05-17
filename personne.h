#ifndef PERSONNE_H
#define PERSONNE_H

#include <QVariant>

class Question;

class Personne
{
public:
    Personne();
    void addVote(std::shared_ptr<Question> question, QVariant choix);
    std::map<std::shared_ptr<Question>, QVariant> votes() const;

private:
    std::map<std::shared_ptr<Question>, QVariant> _votes;
};

#endif // PERSONNE_H
