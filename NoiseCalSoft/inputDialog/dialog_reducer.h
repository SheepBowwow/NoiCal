#ifndef DIALOG_REDUCER_H
#define DIALOG_REDUCER_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

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
