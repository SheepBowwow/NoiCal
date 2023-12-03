#ifndef DIALOG_VAV_TERMINAL_H
#define DIALOG_VAV_TERMINAL_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>

typedef struct VAV_terminal_noise
{
    QString number;  //编号
    QString brand;  //品牌
    QString model;  //型号
    QString valve_open_degree;  //阀门开度
    QString air_volume;  //风量
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
    QString noi_total;  //总量
}VAV_terminal_noise;

namespace Ui {
class Dialog_VAV_terminal;
}

class Dialog_VAV_terminal : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_VAV_terminal(QWidget *parent = nullptr, int editRow = -1, const VAV_terminal_noise& data = VAV_terminal_noise());
    Dialog_VAV_terminal(const QString& name, QWidget *parent = nullptr, int editRow = -1, const VAV_terminal_noise& data = VAV_terminal_noise()){}
    ~Dialog_VAV_terminal();
    void* getNoi() override;
    void total_noi();

private:
    Ui::Dialog_VAV_terminal *ui;
    VAV_terminal_noise* noi;
    int editRow;    //当前修改行，如果是新建就为-1

private slots:
    void on_pushButton_confirm_clicked();
    void calTotalNoise();
    void on_close_clicked();
};

#endif // DIALOG_VAV_TERMINAL_H
