#include "voteliste.h"

using namespace std;

QuestionListe::QuestionListe() {}

 const std::type_info& QuestionListe::typeChoix()
{
    return typeid(_choix);
}

QVariant QuestionListe::choix() const
{
    return _choix;
}

void QuestionListe::setChoix(QVariant newChoix)
{
    _choix = newChoix.toStringList();
}
