#ifndef DLGLISTEVOTE_H
#define DLGLISTEVOTE_H

#include <QDialog>

namespace Ui {
class DlgListeVote;
}

class DlgListeVote : public QDialog
{
    Q_OBJECT

public:
    explicit DlgListeVote(QWidget *parent = nullptr);
    ~DlgListeVote();

    void setPossibilites(const QStringList &v);

private slots:
    void on_btnSelect_clicked();

    void on_btnDeselect_clicked();

private:
    Ui::DlgListeVote *ui;
};

#endif // DLGLISTEVOTE_H
