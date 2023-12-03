#ifndef DIALOG_AIRCONDITION_NOISE_H
#define DIALOG_AIRCONDITION_NOISE_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>

typedef struct Aircondition_noise
{
    QString number;  //空调器编号
    QString brand;      //品牌
    QString model;      //型号
    QString air_volume; //风量
    QString static_pressure; //静压
    /*进口*/
    QString noi_in_63;     //63hz
    QString noi_in_125;     //125hz
    QString noi_in_250;     //250hz
    QString noi_in_500;     //500hz
    QString noi_in_1k;     //1khz
    QString noi_in_2k;     //2khz
    QString noi_in_4k;     //4khz
    QString noi_in_8k;     //8khz
    QString noi_in_total;  //总量
    /*出口*/
    QString noi_out_63;     //63hz
    QString noi_out_125;     //125hz
    QString noi_out_250;     //250hz
    QString noi_out_500;     //500hz
    QString noi_out_1k;     //1khz
    QString noi_out_2k;     //2khz
    QString noi_out_4k;     //4khz
    QString noi_out_8k;     //8khz
    QString noi_out_total;  //总量
}Aircondition_noise;

namespace Ui {
class Dialog_aircondition_noise;
}

class Dialog_aircondition_noise : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_aircondition_noise(QWidget *parent = nullptr, int editRow = -1, const Aircondition_noise& data = Aircondition_noise());
    Dialog_aircondition_noise(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Aircondition_noise& data = Aircondition_noise()){}
    ~Dialog_aircondition_noise();
    void* getNoi() override;

private slots:
    void on_pushButton_confirm_clicked();
    void calTotalNoise();
    void on_close_clicked();

private:
    int editRow;    //当前修改行，如果是新建就为-1
    Ui::Dialog_aircondition_noise *ui;
    Aircondition_noise* noi;

};

#endif // DIALOG_AIRCONDITION_NOISE_H
