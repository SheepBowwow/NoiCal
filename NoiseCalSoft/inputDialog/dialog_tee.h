#ifndef DIALOG_TEE_H
#define DIALOG_TEE_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>

typedef struct Tee_atten
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
}Tee_atten;

namespace Ui {
class Dialog_tee;
}

class Dialog_tee : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_tee(QWidget *parent = nullptr, int editRow = -1, const Tee_atten& data = Tee_atten());
    Dialog_tee(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Tee_atten& data = Tee_atten()){}
    ~Dialog_tee();
    void total_noi();
    void* getNoi() override;

private slots:
    void on_close_clicked();

    void on_pushButton_confirm_clicked();

    void calNoise();

    void on_radioButton_known_clicked();

    void on_radioButton_formula_clicked();

private:
    Ui::Dialog_tee *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    Tee_atten* noi;     //噪音结构体
};

#endif // DIALOG_TEE_H
