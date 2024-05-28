#ifndef FABRIQUEQUESTIONS_H
#define FABRIQUEQUESTIONS_H

#include <list>
#include <memory>

class Question;
class QJsonArray;

class FabriqueQuestions
{
public:
    FabriqueQuestions();
    bool lireJson(const QJsonArray &questions, std::list<std::shared_ptr<Question> > &_questions) const;
    QJsonArray ecrireJson(const std::list<std::shared_ptr<Question> > &_questions) const;
};

#endif // FABRIQUEQUESTIONS_H
