#ifndef DIALOG_DUCT_WITH_MULTI_RANC_H
#define DIALOG_DUCT_WITH_MULTI_RANC_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>

typedef struct Multi_ranc_atten
{
    QString number;  //编号
    QString brand;  //品牌
    QString model;  //型号
    QString getMode;  //得到噪声的方式
    QString q1;       //总风量
    QString q;   //支管风量
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
}Multi_ranc_atten;

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
