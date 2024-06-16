#include "fabriquequestions.h"

#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <map>

#include "questionliste.h"
#include "scrutinuninominal2tours.h"

using namespace std;

const map<const QString, int> FabriqueQuestions::mapTypeToIds = {{"Uninominal 2 tours", 1}, {"Condorcet", 2}};

FabriqueQuestions::FabriqueQuestions() {}

bool FabriqueQuestions::lireJson(const QJsonArray &questions, list<shared_ptr<Question> > &_questions) const
{
    for (const QJsonValue &q : questions)
    {
        QJsonObject jobject = q.toObject();
        shared_ptr<Question> nouvelleQuestion;
        if (jobject["Type"].isNull())
        {
            nouvelleQuestion = make_shared<QuestionListe>();
        }
        else
        {
            nouvelleQuestion = creeQuestionFromType(jobject["Type"].toString());
        }
        nouvelleQuestion->setId(jobject["id"].toInteger());
        nouvelleQuestion->setQuestion(jobject["Question"].toString());
        nouvelleQuestion->setChoix(jobject["Choix"].toVariant());
        _questions.push_back(nouvelleQuestion);
    }
    return true;
}

QJsonArray FabriqueQuestions::ecrireJson(const std::list<std::shared_ptr<Question> > &_questions) const
{
    QJsonArray questions;
    for (const auto &v : _questions)
    {
        QJsonObject jobject;
        jobject["id"] = QJsonValue::fromVariant(v->id());
        jobject["Question"] = v->question();
        jobject["Choix"] = QJsonValue::fromVariant(v->choix());
        jobject["Ouvert"] = v->voteOuvert();
        // If a is true, then we have not key-value mapped to K
        bool absent = true;
        // Traverse the map
        for (const auto& it : mapTypeToIds)
        {
            // If mapped value is v->type(),
            // then print the key value
            if (it.second == v->type())
            {
                jobject["Type"] = it.first;
                absent = false;
                break;
            }
        }
        // If there is not key mapped with v->type() then print -1
        if (absent)
        {
            qWarning() << "Type de scrutin non défini dans la fabrique.";
        }
        questions.append(jobject);
    }
    return questions;
}

std::shared_ptr<Question> FabriqueQuestions::creeQuestionFromType(const QString &type) const
{
    return creeScrutin (mapTypeToIds.at(type));
}

const map<const QString, int> &FabriqueQuestions::getMapTypeToIds()
{
    return mapTypeToIds;
}

std::shared_ptr<Question> FabriqueQuestions::creeScrutin(const int type) const
{
    switch (type)
    {
    case 1 : return make_shared<ScrutinUninominal2Tours>();
    case 2 : return make_shared<QuestionListe>();
    default : qWarning() << "Type de scrutin non reconnu : " << type;
        return nullptr;
    }

}
