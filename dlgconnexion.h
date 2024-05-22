#ifndef DLGCONNEXION_H
#define DLGCONNEXION_H

#include <QDialog>

namespace Ui {
class DlgConnexion;
}

class DlgConnexion : public QDialog
{
    Q_OBJECT

public:
    explicit DlgConnexion(QWidget *parent = nullptr);
    ~DlgConnexion();
    const QString &pseudonyme() const;

private:
    Ui::DlgConnexion *ui;
};

#endif // DLGCONNEXION_H
