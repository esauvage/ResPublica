#ifndef FABRIQUEQUESTIONS_H
#define FABRIQUEQUESTIONS_H

#include <list>
#include <map>
#include <memory>

class Question;
class QJsonArray;
class QString;

class FabriqueQuestions
{
public:
    FabriqueQuestions();
    bool lireJson(const QJsonArray &questions, std::list<std::shared_ptr<Question> > &_questions) const;
    QJsonArray ecrireJson(const std::list<std::shared_ptr<Question> > &_questions) const;
    static const std::map<const QString, int> &getMapTypeToIds();
    std::shared_ptr<Question> creeScrutin(const int type) const;

private:
    std::shared_ptr<Question> creeQuestionFromType(const QString &type) const;

    static const std::map<const QString, int> mapTypeToIds;
};

#endif // FABRIQUEQUESTIONS_H
