#ifndef VOTELISTE_H
#define VOTELISTE_H

#include <QString>

#include "question.h"

class QuestionListe : public Question
{
public:
    QuestionListe();
    const std::type_info& typeChoix();
    QVariant choix() const;
    void setChoix(QVariant c);

private:
    QStringList _choix;
};

#endif // VOTELISTE_H
