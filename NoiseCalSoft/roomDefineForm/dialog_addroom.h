#ifndef DIALOG_ADDROOM_H
#define DIALOG_ADDROOM_H

#include <QDialog>
#include "inputbasedialog.h"

namespace Ui {
class Dialog_addroom;
}

class Dialog_addroom : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_addroom(QWidget *parent = nullptr);
    ~Dialog_addroom();

    QString getroomid();
    QString getroomname();
    int getmainpipe();
    QString getroomclass();
    QString getlimit();
    QString getroomcalclass();

    virtual void * getNoi() override;

signals:
    void dialogsent(QString name,int num);

private slots:
    void on_pushButton_clicked();

    void on_close_clicked();

private:
    Ui::Dialog_addroom *ui;
};

#endif // DIALOG_ADDROOM_H
