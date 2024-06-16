#ifndef QUESTIONLISTE_H
#define QUESTIONLISTE_H

#include <QString>

#include "question.h"

class QuestionListe : public Question
{
public:
    QuestionListe();
    const std::type_info& typeChoix() override;
    QVariant choix() const override;
    void setChoix(QVariant c) override;
    int type() const override {return 2;}

    virtual QByteArray checksum() const override;
private:
    QStringList _choix;
};

#endif // QUESTIONLISTE_H
