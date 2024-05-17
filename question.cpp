#include "question.h"

#include <QJsonDocument>

using namespace std;

size_t Question::suivantId = 1;

Question::Question()
    :_id(0)
{
    _id = _id ? _id : Question::suivantId++;

}

const QString &Question::question() const
{
    return _question;
}

void Question::setQuestion(const QString &v)
{
    _question = v;
}

void Question::setOuvertureDate(const QDateTime &newOuvertureDate)
{
    _ouvertureDate = newOuvertureDate;
}

size_t Question::id() const
{
    return _id;
}

void Question::setId(size_t newId)
{
    _id = newId;
}


QTextStream &operator <<(QTextStream & out, const shared_ptr<Question> v)
{
    v->setId(v->id() ? v->id() : Question::suivantId++);
    out << v->id();
    out << v->question();
    out << v->choix().toJsonDocument().toJson();
    return out;
}

QTextStream &operator >>(QTextStream & in, const shared_ptr<Question> v)
{
    size_t id;
    in >> id;
    v->setId(id);
    QString question;
    in >> question;
    v->setQuestion(question);
    return in;
}
