#include "dlgeditvote.h"
#include "ui_dlgeditvote.h"

#include "fabriquequestions.h"
#include "question.h"

#include "RPListWidget.h"

using namespace std;

DlgEditQuestion::DlgEditQuestion(shared_ptr<Question> vote, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgEditQuestion),
    _question(vote)
{
    ui->setupUi(this);
    setTypes(FabriqueQuestions::getMapTypeToIds());
    if (!_question)
    {
        auto listChoix = new RPListWidget(this);
        ui->formLayout->insertRow(ui->formLayout->rowCount() - 2, new QLabel("Choix :"), listChoix);
    }
    else
    {
        if (_question->typeChoix() == typeid(QStringList))
        {
            auto listChoix = new RPListWidget(this);
            ui->formLayout->insertRow(ui->formLayout->rowCount() - 2, new QLabel("Choix :"), listChoix);
            listChoix->addItems(_question->choix().toStringList());
        }
        ui->edtQuestion->setPlainText(_question->question());
        bool absent = true;
        for (const auto& it : FabriqueQuestions::getMapTypeToIds())
        {
            // If mapped value is v->type(),
            // then print the key value
            if (it.second == _question->type())
            {
                ui->cbxType->setCurrentText(it.first);
                absent = false;
                break;
            }
        }
        // If there is not key mapped with v->type() then print -1
        if (absent)
        {
            qWarning() << "Type de scrutin non défini dans la fabrique.";
        }
    }
//    ui->edtOuvertureDate->setDateTime(_vote->ouvertureDate());
}

DlgEditQuestion::~DlgEditQuestion()
{
    delete ui;
}

void DlgEditQuestion::on_buttonBox_accepted()
{
    if (!_question)
    {
        FabriqueQuestions fabrique;
        _question = fabrique.creeScrutin(ui->cbxType->currentData().toInt());
    }
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

std::shared_ptr<Question> DlgEditQuestion::question() const
{
    return _question;
}

void DlgEditQuestion::setTypes(const std::map<const QString, int> &types)
{
    for (const auto &t : types)
    {
        ui->cbxType->addItem(t.first, t.second);
    }
}

int DlgEditQuestion::type() const
{
    return ui->cbxType->currentData().toInt();
}

void DlgEditQuestion::on_btnClore_clicked()
{
    if (!_question) return;
    _question->setVoteOuvert(false);
}

