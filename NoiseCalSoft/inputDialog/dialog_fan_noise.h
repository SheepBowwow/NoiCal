#ifndef DIALOG_FAN_NOISE_H
#define DIALOG_FAN_NOISE_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_fan_noise;
}

class Dialog_fan_noise : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_fan_noise(QWidget *parent = nullptr, int editRow = -1, const Fan_noise& data = Fan_noise());
    Dialog_fan_noise(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Fan_noise& data = Fan_noise()){}
    ~Dialog_fan_noise();
    void* getNoi() override;

private slots:
    void on_pushButton_confirm_clicked();
    void calTotalNoise();
    void on_close_clicked();

private:
    int editRow;    //当前修改行，如果是新建就为-1
    Ui::Dialog_fan_noise *ui;
    Fan_noise* noi;

};
#endif // DIALOG_FAN_NOISE_H
