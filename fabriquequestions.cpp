#include "fabriquequestions.h"

#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

#include "questionliste.h"

using namespace std;

FabriqueQuestions::FabriqueQuestions() {}

bool FabriqueQuestions::lireJson(const QJsonArray &questions, list<shared_ptr<Question> > &_questions)
{
    for (const QJsonValue &q : questions)
    {
        auto nouveauQuestion = make_shared<QuestionListe>();
        QJsonObject jobject = q.toObject();
        nouveauQuestion->setId(jobject["id"].toInteger());
        nouveauQuestion->setQuestion(jobject["Question"].toString());
        nouveauQuestion->setChoix(jobject["Choix"].toVariant());
        _questions.push_back(nouveauQuestion);
    }
}
