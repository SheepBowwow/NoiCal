#ifndef DIALOG_PIPE_H
#define DIALOG_PIPE_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

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
