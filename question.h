#ifndef QUESTION_H
#define QUESTION_H

#include <QString>
#include <QVariant>
#include <QDateTime>

class Question
{
public:
    Question();

    virtual int type() const = 0;
    virtual const std::type_info& typeChoix() = 0;
    virtual QVariant choix() const = 0;
    virtual void setChoix(QVariant c) = 0;
    const QString &question() const;
    void setQuestion(const QString &v);
    void setOuvertureDate(const QDateTime &newOuvertureDate);

    size_t id() const;
    void setId(size_t newId);
    virtual QByteArray checksum() const = 0;

    static size_t suivantId;

    bool voteOuvert() const;
    void setVoteOuvert(bool newVoteOuvert);

private:
    size_t _id;
    QString _question;
    QVariant _resultat;
    QDateTime _ouvertureDate;
    bool _voteOuvert;
};

QTextStream & operator << (QTextStream & out, const std::shared_ptr<Question> v);
QTextStream & operator >> (QTextStream & in, const std::shared_ptr<Question> v);

#endif // QUESTION_H
