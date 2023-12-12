#ifndef DIALOG_ELBOW_H
#define DIALOG_ELBOW_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

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
