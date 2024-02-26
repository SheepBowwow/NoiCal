#ifndef DIALOG_AIRCONDITION_NOISE_H
#define DIALOG_AIRCONDITION_NOISE_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_aircondition_noise;
}

class Dialog_aircondition_noise : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_aircondition_noise(QWidget *parent = nullptr, int editRow = -1, int fan = 0, const Aircondition_noise& data = Aircondition_noise());
    Dialog_aircondition_noise(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Aircondition_noise& data = Aircondition_noise()){}
    ~Dialog_aircondition_noise();
    void* getNoi() override;

private slots:
    void on_pushButton_confirm_clicked();
    void calTotalNoise();
    void on_close_clicked();

    void on_radioButton_single_clicked();

    void on_radioButton_double_clicked();
private:
    int editRow;    //当前修改行，如果是新建就为-1
    Ui::Dialog_aircondition_noise *ui;
    Aircondition_noise* noi;

};

#endif // DIALOG_AIRCONDITION_NOISE_H
