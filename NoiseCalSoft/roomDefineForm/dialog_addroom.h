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

    void setvalues(QString,QString,QString,QString,QString,QString);

    QString getroomid();
    QString getroomname();
    QString getjiaban();
    int getmainpipe();
    QString getroomclass();
    QString getlimit();
    QString getroomcalclass();

    void change_outer();



signals:
    void dialogsent(QString name,int num, QString jiaban, QString limit);

private slots:
    void on_pushButton_clicked();

    void on_close_clicked();

    void on_comboBox_currentTextChanged(const QString &arg1);

private:
    Ui::Dialog_addroom *ui;
    virtual void * getComponent() override {};


    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override {};

    // InputBaseDialog interface
public:
    void switchToCompontDB(bool inDB) override {};
};

#endif // DIALOG_ADDROOM_H
