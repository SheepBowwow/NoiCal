#ifndef DIALOG_REDUCER_H
#define DIALOG_REDUCER_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>

typedef struct Reducer_atten
{
    QString number;  //编号
    QString model;  //型号
    QString type;  //类型 圆还是方
    QString getMode;  //得到噪声的方式
    QString diameter_before;  //变径前直径
    QString diameter_after;  //变径后直径
    QString length_before;  //变径前长
    QString length_after;  //变径后长
    QString width_before;  //变径前宽
    QString width_after;  //变径后宽
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
}Reducer_atten;

namespace Ui {
class Dialog_reducer;
}

class Dialog_reducer : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_reducer(QWidget *parent = nullptr, int editRow = -1, const Reducer_atten& data = Reducer_atten());
    Dialog_reducer(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Reducer_atten& data = Reducer_atten()){}
    ~Dialog_reducer();
    void total_noi();
    void* getNoi() override;

private slots:
    void on_close_clicked();

    void on_pushButton_confirm_clicked();

    void calNoise();

    void on_radioButton_known_clicked();

    void on_radioButton_formula_clicked();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

private:
    Ui::Dialog_reducer *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    Reducer_atten* noi;     //噪音结构体
};

#endif // DIALOG_REDUCER_H
