#ifndef DIALOG_TEE_H
#define DIALOG_TEE_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_tee;
}

class Dialog_tee : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_tee(QWidget *parent = nullptr, int editRow = -1, const Tee& data = Tee());
    Dialog_tee(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Tee& data = Tee()){}
    ~Dialog_tee();

private slots:
    void on_close_clicked();

    void on_pushButton_confirm_clicked();
    bool check_lineedit();

private:
    Ui::Dialog_tee *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    Tee* component;     //噪音结构体
    array<QLineEdit*,8> atten_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;
};

#endif // DIALOG_TEE_H
