#include "questionliste.h"

#include <QCryptographicHash>

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

QByteArray QuestionListe::checksum() const
{
    QCryptographicHash hasher(QCryptographicHash::Blake2s_256);
    QStringList sl;
    sl << question();
    sl << _choix;
    return hasher.hash(sl.join(",").toLatin1(), QCryptographicHash::Blake2s_256);
}
