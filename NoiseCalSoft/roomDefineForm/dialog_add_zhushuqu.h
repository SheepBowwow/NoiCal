#ifndef DIALOG_ADD_ZHUSHUQU_H
#define DIALOG_ADD_ZHUSHUQU_H

#include <QDialog>
#include "inputbasedialog.h"

namespace Ui {
class Dialog_add_zhushuqu;
}

class Dialog_add_zhushuqu : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_add_zhushuqu(QWidget *parent = nullptr);
    ~Dialog_add_zhushuqu();
    void setlabeltext(QString text);   //
    QString getname();

private slots:
    void on_close_clicked();

    void on_pushButton_clicked();

private:
    Ui::Dialog_add_zhushuqu *ui;
    virtual void* getComponent() override {};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override {};
};

#endif // DIALOG_ADD_ZHUSHUQU_H
