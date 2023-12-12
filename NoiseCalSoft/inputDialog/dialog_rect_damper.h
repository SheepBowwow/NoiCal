#ifndef DIALOG_RECT_DAMPER_H
#define DIALOG_RECT_DAMPER_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

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
