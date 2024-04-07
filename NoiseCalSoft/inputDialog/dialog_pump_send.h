#ifndef DIALOG_PUMP_SEND_H
#define DIALOG_PUMP_SEND_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_pump_send;
}

class Dialog_pump_send : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_pump_send(QString type = "", QWidget *parent = nullptr, int editRow = -1, const PumpSend& data = PumpSend());
    Dialog_pump_send(QWidget *parent = nullptr, int editRow = -1, const PumpSend& data = PumpSend()){}
    ~Dialog_pump_send();

private slots:
    void on_close_clicked();

    void calTotalNoise();

    void on_pushButton_confirm_clicked();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

private:
    Ui::Dialog_pump_send *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    PumpSend* component;     //噪音结构体

    array<QLineEdit*,9> noi_lineEdits;
    array<QLineEdit*,8> atten_lineEdits;
    array<QLineEdit*,8> refl_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;
};

#endif // DIALOG_PUMP_SEND_H
