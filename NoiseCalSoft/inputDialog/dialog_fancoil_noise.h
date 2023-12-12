#ifndef DIALOG_FANCOIL_NOISE_H
#define DIALOG_FANCOIL_NOISE_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_fanCoil_noise;
}

class Dialog_fanCoil_noise : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_fanCoil_noise(QWidget *parent = nullptr, int editRow = -1, const FanCoil_noise& data = FanCoil_noise());
    Dialog_fanCoil_noise(const QString& name, QWidget *parent = nullptr, int editRow = -1, const FanCoil_noise& data = FanCoil_noise()){}
    ~Dialog_fanCoil_noise();
    void* getNoi() override;

private:
    Ui::Dialog_fanCoil_noise *ui;
    FanCoil_noise* noi;
    int editRow;    //当前修改行，如果是新建就为-1

private slots:
    void on_pushButton_confirm_clicked();
    void calTotalNoise();
    void on_close_clicked();
};

#endif // DIALOG_FANCOIL_NOISE_H
