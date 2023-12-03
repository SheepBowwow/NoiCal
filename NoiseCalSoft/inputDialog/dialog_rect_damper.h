#ifndef DIALOG_RECT_DAMPER_H
#define DIALOG_RECT_DAMPER_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>

typedef struct Rect_damper_noi
{
    QString number;  //编号
    QString brand;  //编号
    QString model;  //型号
    QString length;  //长
    QString width;   //宽
    QString angle;   //叶片角度
    QString air_volume;   //风量
    QString getMode;   //得到噪声的方式
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
    QString noi_total;     //总值
}Rect_damper_noi;

namespace Ui {
class Dialog_rect_damper;
}

class Dialog_rect_damper : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_rect_damper(QWidget *parent = nullptr, int editRow = -1, const Rect_damper_noi& data = Rect_damper_noi());
    Dialog_rect_damper(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Rect_damper_noi& data = Rect_damper_noi()){}
    ~Dialog_rect_damper();
    void* getNoi() override;

private slots:

    void on_close_clicked();

    void on_pushButton_confirm_clicked();

    void calNoise();

    void calTotalNoise();

    void on_radioButton_known_clicked();

    void on_radioButton_formula_clicked();

private:
    Ui::Dialog_rect_damper *ui;
    QMap<int,QVector<int>> fixNumber;
    int editRow;    //当前修改行，如果是新建就为-1
    Rect_damper_noi* noi;     //噪音结构体
};

#endif // DIALOG_RECT_DAMPER_H
