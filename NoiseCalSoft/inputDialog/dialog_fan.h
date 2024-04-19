#ifndef DIALOG_FAN_H
#define DIALOG_FAN_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_fan;
}

class Dialog_fan : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_fan(QWidget *parent = nullptr, int editRow = -1, const Fan& data = Fan());
    Dialog_fan(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Fan& data = Fan()){}
    ~Dialog_fan();

private slots:
    void on_pushButton_confirm_clicked();
    bool check_lineedit();
    void calTotalNoise();
    void on_close_clicked();

private:
    int editRow;    //当前修改行，如果是新建就为-1
    Ui::Dialog_fan *ui;
    Fan* component;
    array<QLineEdit*,9> noi_in_lineEdits;
    array<QLineEdit*,9> noi_out_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;
    void switchToComponentDB();
};
#endif // DIALOG_FAN_H
