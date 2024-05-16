#include "dlgeditvote.h"
#include "ui_dlgeditvote.h"

#include "vote.h"

using namespace std;

DlgEditVote::DlgEditVote(shared_ptr<Vote> vote, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgEditVote),
    _vote(vote)
{
    ui->setupUi(this);
    ui->edtQuestion->setPlainText(_vote->question());
//    ui->edtOuvertureDate->setDateTime(_vote->ouvertureDate());
}

DlgEditVote::~DlgEditVote()
{
    delete ui;
}

void DlgEditVote::on_buttonBox_accepted()
{
    _vote->setQuestion(ui->edtQuestion->toPlainText());
    _vote->setOuvertureDate(ui->edtOuvertureDate->dateTime());
}

