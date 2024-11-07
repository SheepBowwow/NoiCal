#ifndef DIALOG_DIFFUSER_BRANCH_H
#define DIALOG_DIFFUSER_BRANCH_H

#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"
#include "inputbasedialog.h"

namespace Ui {
class Dialog_diffuser_branch;
}

class Dialog_diffuser_branch : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_diffuser_branch(QWidget *parent = nullptr, int editRow = -1, const Diffuser_branch& data = Diffuser_branch());
    Dialog_diffuser_branch(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Diffuser_branch& data = Diffuser_branch()){}
    ~Dialog_diffuser_branch();

private:
    Ui::Dialog_diffuser_branch *ui;

private slots:
    void on_close_clicked();

    void on_pushButton_confirm_clicked();
    bool check_lineedit();

private:
    int editRow;    //当前修改行，如果是新建就为-1
    Diffuser_branch* component;     //噪音结构体
    array<QLineEdit*,8> atten_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;

    // InputBaseDialog interface
public:
    void switchToCompontDB(bool inDB) override {};
};

#endif // DIALOG_DIFFUSER_BRANCH_H
