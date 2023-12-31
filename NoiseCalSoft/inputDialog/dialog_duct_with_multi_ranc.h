#ifndef DIALOG_DUCT_WITH_MULTI_RANC_H
#define DIALOG_DUCT_WITH_MULTI_RANC_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_duct_with_multi_ranc;
}

class Dialog_duct_with_multi_ranc : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_duct_with_multi_ranc(QWidget *parent = nullptr, int editRow = -1, const Multi_ranc_atten& data = Multi_ranc_atten());
    Dialog_duct_with_multi_ranc(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Multi_ranc_atten& data = Multi_ranc_atten()){}
    ~Dialog_duct_with_multi_ranc();
    void total_noi();
    void* getNoi() override;

private slots:
    void on_close_clicked();

    void on_pushButton_confirm_clicked();

    void calNoise();

    void on_radioButton_known_clicked();

    void on_radioButton_formula_clicked();

private:
    Ui::Dialog_duct_with_multi_ranc *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    Multi_ranc_atten* noi;     //噪音结构体
};

#endif // DIALOG_DUCT_WITH_MULTI_RANC_H
