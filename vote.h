#ifndef VOTE_H
#define VOTE_H

#include <QString>
#include <QVariant>
#include <QDateTime>

class Vote
{
public:
    Vote();

    const QString &question() const;
    void setQuestion(const QString &v);
    void setOuvertureDate(const QDateTime &newOuvertureDate);

    size_t id() const;
    void setId(size_t newId);

private:
    size_t _id;
    QString _question;
    QVariant _resultat;
    QDateTime _ouvertureDate;
};

QTextStream & operator << (QTextStream & out, const std::shared_ptr<Vote> v);
QTextStream & operator >> (QTextStream & in, const std::shared_ptr<Vote> v);

#endif // VOTE_H
