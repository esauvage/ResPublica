#ifndef SCRUTINUNINOMINAL2TOURS_H
#define SCRUTINUNINOMINAL2TOURS_H

#include "questionliste.h"

class ScrutinUninominal2Tours : public QuestionListe
{
public:
    ScrutinUninominal2Tours();
    int type() const override {return 1;}
};

#endif // SCRUTINUNINOMINAL2TOURS_H
