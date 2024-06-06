#ifndef DLGCONNEXION_H
#define DLGCONNEXION_H

#include <QDialog>

class QAbstractButton;

namespace Ui {
class DlgConnexion;
}

class DlgConnexion : public QDialog
{
    Q_OBJECT

public:
    explicit DlgConnexion(QWidget *parent = nullptr);
    ~DlgConnexion();
    const QString pseudonyme() const;
    void setSel(QString v);
    const QByteArray hash() const;

private slots:
    void accept() override;

private:
    Ui::DlgConnexion *ui;
    QString _sel;
};

#endif // DLGCONNEXION_H
