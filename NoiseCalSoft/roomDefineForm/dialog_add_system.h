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


    void setvalues(int,QString,QString);
    QString getType();
    QString getNumber();
    QString getModel();

private slots:
    void on_pushButton_queding_clicked();

    void on_close_clicked();

    void on_comboBox_type_currentIndexChanged(int index);

    void on_comboBox_numOrModel_currentTextChanged(const QString &arg1);

private:
    Ui::dialog_add_system *ui;
    virtual void * getComponent() override {};
    QString componentUUID;


    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override {};
    QString getComponentUUID() const;
};

#endif // DIALOG_ADD_SYSTEM_H
