#include "vote.h"

Vote::Vote(QVariant choix, bool aVerifier)
    :_choix(choix), _aConfirmer(aVerifier)
{

}

QVariant Vote::choix() const
{
    return _choix;
}

void Vote::setChoix(const QVariant &newChoix)
{
    _choix = newChoix;
}

bool Vote::aConfirmer() const
{
    return _aConfirmer;
}

void Vote::setAConfirmer(bool newAConfirmer)
{
    _aConfirmer = newAConfirmer;
}
