#ifndef DIALOG_ADD_SYSTEM_H
#define DIALOG_ADD_SYSTEM_H

#include <QDialog>
#include "inputbasedialog.h"

namespace Ui {
class dialog_add_system;
}

class dialog_add_system : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit dialog_add_system(QWidget *parent = nullptr);
    ~dialog_add_system();
    virtual void * getNoi() override;

    void setvalues(int,QString,QString);
    QString getsyslx();
    QString getsysbh();
    QString getsysxh();

private slots:
    void on_pushButton_queding_clicked();

    void on_close_clicked();

private:
    Ui::dialog_add_system *ui;
};

#endif // DIALOG_ADD_SYSTEM_H
