#ifndef DIALOG_CIRCULAR_DAMPER_H
#define DIALOG_CIRCULAR_DAMPER_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_circular_damper;
}

class Dialog_circular_damper : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_circular_damper(QWidget *parent = nullptr, int editRow = -1, const Circular_damper_noi& data = Circular_damper_noi());
    Dialog_circular_damper(const QString &name, QWidget *parent = nullptr, int editRow = -1, const Circular_damper_noi& data = Circular_damper_noi()){}
    ~Dialog_circular_damper();
    void* getNoi() override;

private slots:

    void on_close_clicked();

    void on_pushButton_confirm_clicked();

    void calNoise();

    void on_radioButton_known_clicked();

    void on_radioButton_formula_clicked();

    void calTotalNoise();

private:
    Ui::Dialog_circular_damper *ui;
    QMap<int,QVector<int>> fixNumber;
    int editRow;    //当前修改行，如果是新建就为-1
    Circular_damper_noi* noi;     //噪音结构体
};

#endif // DIALOG_CIRCULAR_DAMPER_H
