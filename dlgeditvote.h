#ifndef DLGEDITVOTE_H
#define DLGEDITVOTE_H

#include <QDialog>

class Question;

namespace Ui {
class DlgEditQuestion;
}

class DlgEditQuestion : public QDialog
{
    Q_OBJECT

public:
    explicit DlgEditQuestion(std::shared_ptr<Question> vote, QWidget *parent = nullptr);
    ~DlgEditQuestion();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DlgEditQuestion *ui;
    std::shared_ptr<Question> _question;
};

#endif // DLGEDITVOTE_H
