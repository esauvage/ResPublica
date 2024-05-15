#ifndef DLGEDITVOTE_H
#define DLGEDITVOTE_H

#include <QDialog>

class Vote;

namespace Ui {
class DlgEditVote;
}

class DlgEditVote : public QDialog
{
    Q_OBJECT

public:
    explicit DlgEditVote(std::shared_ptr<Vote> vote, QWidget *parent = nullptr);
    ~DlgEditVote();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DlgEditVote *ui;
    std::shared_ptr<Vote> _vote;
};

#endif // DLGEDITVOTE_H
