#ifndef DIALOG_DISP_VENT_TERMINAL_H
#define DIALOG_DISP_VENT_TERMINAL_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>

typedef struct Disp_vent_terminal_noise
{
    QString number;  //型号
    QString brand;  //型号
    QString model;  //型号
    QString type;  //类型 圆还是方
    QString diameter;  //直径
    QString length;  //长
    QString width;  //宽
    QString size;  //尺寸
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
    QString noi_total;  //总量
    QString atten_63;     //63hz
    QString atten_125;     //125hz
    QString atten_250;     //250hz
    QString atten_500;     //500hz
    QString atten_1k;     //1khz
    QString atten_2k;     //2khz
    QString atten_4k;     //4khz
    QString atten_8k;     //8khz
    QString refl_63;     //63hz
    QString refl_125;     //125hz
    QString refl_250;     //250hz
    QString refl_500;     //500hz
    QString refl_1k;     //1khz
    QString refl_2k;     //2khz
    QString refl_4k;     //4khz
    QString refl_8k;     //8khz
    QString getMode;    //
}Disp_vent_terminal_noise;

namespace Ui {
class Dialog_disp_vent_terminal;
}

class Dialog_disp_vent_terminal : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_disp_vent_terminal(QWidget *parent = nullptr, int editRow = -1, const Disp_vent_terminal_noise& data = Disp_vent_terminal_noise());
    Dialog_disp_vent_terminal(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Disp_vent_terminal_noise& data = Disp_vent_terminal_noise()){}
    ~Dialog_disp_vent_terminal();
    void* getNoi() override;
    void total_noi();

private:
    Ui::Dialog_disp_vent_terminal *ui;
    Disp_vent_terminal_noise* noi;
    int editRow;    //当前修改行，如果是新建就为-1

private slots:
    void on_pushButton_confirm_clicked();

    void calReflNoi();

    void calTotalNoise();

    void on_close_clicked();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

    void on_radioButton_known_clicked();

    void on_radioButton_formula_clicked();
    void on_pushButton_clicked();
};

#endif // DIALOG_DISP_VENT_TERMINAL_H
