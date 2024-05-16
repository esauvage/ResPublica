#include "voteliste.h"

using namespace std;

VoteListe::VoteListe() {}

 const std::type_info& VoteListe::typeChoix()
{
    return typeid(_choix);
}

QVariant VoteListe::choix() const
{
    return _choix;
}

void VoteListe::setChoix(QVariant newChoix)
{
    _choix = newChoix.toStringList();
}
