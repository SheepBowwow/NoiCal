#ifndef DIALOG_PIPE_H
#define DIALOG_PIPE_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>

typedef struct Pipe_atten
{
    QString number;  //编号
    QString model;  //型号
    QString diameter;   //直径
    QString length;   //长
    QString width;   //宽
    QString type;   //类型
    QString getMode;  //得到噪声的方式
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
}Pipe_atten;

namespace Ui {
class Dialog_pipe;
}

class Dialog_pipe : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_pipe(QWidget *parent = nullptr, int editRow = -1, const Pipe_atten& data = Pipe_atten());
    Dialog_pipe(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Pipe_atten& data = Pipe_atten()){}
    ~Dialog_pipe();
    void* getNoi() override;

private slots:
    void on_close_clicked();

    void calNoise();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

    void on_pushButton_confirm_clicked();

    void on_radioButton_known_clicked();

    void on_radioButton_formula_clicked();

private:
    Ui::Dialog_pipe *ui;
    QMap<int,QVector<double>> rectData;
    QMap<int,QVector<double>> roundData;
    int editRow;    //当前修改行，如果是新建就为-1
    Pipe_atten* noi;     //噪音结构体
    QString type;
};

#endif // DIALOG_PIPE_H
