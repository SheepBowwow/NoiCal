#ifndef DIALOG_SET_NAME_H
#define DIALOG_SET_NAME_H

#include <QDialog>
#include "inputbasedialog.h"

namespace Ui {
class Dialog_set_name;
}

class Dialog_set_name : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_set_name(QString title, QWidget *parent = nullptr);
    ~Dialog_set_name();
    QString getName();

private slots:
    void on_close_clicked();

    void on_pushButton_clicked();

private:
    Ui::Dialog_set_name *ui;
    virtual void* getComponent() override {};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override {};

    // InputBaseDialog interface
public:
    void switchToCompontDB(bool inDB) override {};
};

#endif // DIALOG_SET_NAME_H
