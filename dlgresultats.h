#ifndef DLGRESULTATS_H
#define DLGRESULTATS_H

#include <QDialog>

#include "vote.h"

namespace Ui {
class DlgResultats;
}

class DlgResultats : public QDialog
{
    Q_OBJECT

public:
    explicit DlgResultats(QWidget *parent = nullptr);
    ~DlgResultats();

    void setVotes(const std::list<Vote> &newVotes);

private:
    Ui::DlgResultats *ui;
    std::list<Vote> _votes;
};

#endif // DLGRESULTATS_H
