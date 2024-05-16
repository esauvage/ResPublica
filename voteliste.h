#ifndef VOTELISTE_H
#define VOTELISTE_H

#include <QString>

#include "vote.h"

class VoteListe : public Vote
{
public:
    VoteListe();
    const std::type_info& typeChoix();
    QVariant choix() const;
    void setChoix(QVariant c);

private:
    QStringList _choix;
};

#endif // VOTELISTE_H
