#include "dlgeditvote.h"
#include "ui_dlgeditvote.h"

#include "question.h"

#include "RPListWidget.h"

using namespace std;

DlgEditQuestion::DlgEditQuestion(shared_ptr<Question> vote, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgEditQuestion),
    _question(vote)
{
    ui->setupUi(this);
    if (_question->typeChoix() == typeid(QStringList))
    {
        auto listChoix = new RPListWidget(this);
        ui->formLayout->insertRow(ui->formLayout->rowCount() - 2, new QLabel("Choix :"), listChoix);
        listChoix->addItems(_question->choix().toStringList());
    }
    ui->edtQuestion->setPlainText(_question->question());
//    ui->edtOuvertureDate->setDateTime(_vote->ouvertureDate());
}

DlgEditQuestion::~DlgEditQuestion()
{
    delete ui;
}

void DlgEditQuestion::on_buttonBox_accepted()
{
    _question->setQuestion(ui->edtQuestion->toPlainText());
    _question->setOuvertureDate(ui->edtOuvertureDate->dateTime());
    if (_question->typeChoix() == typeid(QStringList))
    {
        QStringList listeChoix;
        RPListWidget * list = nullptr;
        for (int i = 0; i < ui->formLayout->rowCount() && !list; ++i)
        {
            list = dynamic_cast<RPListWidget *>(ui->formLayout->itemAt(i, QFormLayout::FieldRole)->widget());
        }
        for (const auto &c : list->findItems("", Qt::MatchContains))
        {
            listeChoix << c->text();
        }
        _question->setChoix(listeChoix);
    }
}

