#ifndef DIALOG_ADDOUTER_H
#define DIALOG_ADDOUTER_H

#include <QDialog>

namespace Ui {
class Dialog_addouter;
}

class Dialog_addouter : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_addouter(QWidget *parent = nullptr);
    ~Dialog_addouter();

private:
    Ui::Dialog_addouter *ui;
};

#endif // DIALOG_ADDOUTER_H
