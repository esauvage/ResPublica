#include "vote.h"

using namespace std;

Vote::Vote()
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
    out << v->question();
    return out;
}

QTextStream &operator >>(QTextStream & in, const shared_ptr<Vote> v)
{
    QString question;
    in >> question;
    v->setQuestion(question);
    return in;
}
