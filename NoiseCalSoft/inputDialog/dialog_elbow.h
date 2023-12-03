#ifndef DIALOG_ELBOW_H
#define DIALOG_ELBOW_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>

typedef struct Elbow_atten
{
    QString number;  //编号
    QString model;  //型号
    QString type;  //类型 圆还是方
    QString getMode;  //得到噪声的方式
    QString diameter;  //直径
    QString width;  //宽
    QString isLined;  //是否有内衬
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
}Elbow_atten;

namespace Ui {
class Dialog_elbow;
}

class Dialog_elbow : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_elbow(QWidget *parent = nullptr, int editRow = -1, const Elbow_atten& data = Elbow_atten());
    Dialog_elbow(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Elbow_atten& data = Elbow_atten()){}
    ~Dialog_elbow();
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
    Ui::Dialog_elbow *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    Elbow_atten* noi;     //噪音结构体
};

#endif // DIALOG_ELBOW_H
