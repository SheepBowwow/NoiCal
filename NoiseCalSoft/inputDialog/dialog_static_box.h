#ifndef DIALOG_STATIC_BOX_H
#define DIALOG_STATIC_BOX_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>

typedef struct Static_box
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
}Static_box;

namespace Ui {
class Dialog_static_box;
}

class Dialog_static_box : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_static_box(QWidget *parent = nullptr, int editRow = -1, const Static_box& data = Static_box());
    Dialog_static_box(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Static_box& data = Static_box()){}
    ~Dialog_static_box();
    void* getNoi() override;
    void total_noi();

private:
    Ui::Dialog_static_box *ui;
    Static_box* noi;
    int editRow;    //当前修改行，如果是新建就为-1

private slots:
    void on_pushButton_confirm_clicked();

    void on_close_clicked();

    void calNoise();

    void on_radioButton_known_clicked();

    void on_radioButton_formula_clicked();
};

#endif // DIALOG_STATIC_BOX_H
