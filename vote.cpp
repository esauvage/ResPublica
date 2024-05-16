#include "vote.h"

using namespace std;

size_t Vote::suivantId = 1;

Vote::Vote()
    :_id(0)
{

}

const QString &Vote::question() const
{
    return _question;
}

void Vote::setQuestion(const QString &v)
{
    _question = v;
}

void Vote::setOuvertureDate(const QDateTime &newOuvertureDate)
{
    _ouvertureDate = newOuvertureDate;
}

size_t Vote::id() const
{
    return _id;
}

void Vote::setId(size_t newId)
{
    _id = newId;
}

QTextStream &operator <<(QTextStream & out, const shared_ptr<Vote> v)
{
    v->setId(v->id() ? v->id() : Vote::suivantId++);
    out << v->id();
    out << v->question();
    return out;
}

QTextStream &operator >>(QTextStream & in, const shared_ptr<Vote> v)
{
    size_t id;
    in >> id;
    v->setId(id);
    QString question;
    in >> question;
    v->setQuestion(question);
    return in;
}
