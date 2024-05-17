#ifndef DLGLISTEVOTE_H
#define DLGLISTEVOTE_H

#include <QDialog>

namespace Ui {
class DlgListeQuestion;
}

class DlgListeQuestion : public QDialog
{
    Q_OBJECT

public:
    explicit DlgListeQuestion(QWidget *parent = nullptr);
    ~DlgListeQuestion();

    void setPossibilites(const QStringList &v);
    QStringList selection() const;

private slots:
    void on_btnSelect_clicked();

    void on_btnDeselect_clicked();

private:
    Ui::DlgListeQuestion *ui;
};

#endif // DLGLISTEVOTE_H
