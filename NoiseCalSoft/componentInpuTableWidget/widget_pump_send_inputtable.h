#ifndef WIDGET_PUMP_SEND_INPUTTABLE_H
#define WIDGET_PUMP_SEND_INPUTTABLE_H

#include <QWidget>
#include "componentInpuTableWidget/widget_base_inputtable.h"

namespace Ui {
class Widget_Pump_Send_inputTable;
}

class Widget_Pump_Send_inputTable : public Widget_base_inputTable
{
    Q_OBJECT

public:
    explicit Widget_Pump_Send_inputTable(QWidget *parent = nullptr);
    ~Widget_Pump_Send_inputTable();

private:
    Ui::Widget_Pump_Send_inputTable *ui;

    // Widget_base_inputTable interface
public:
    void initTableWidget() override;
    void setCurrentTable(const int& index);

public slots:
    void onAdd() override;
    void onDel() override;
    void onRevise() override;
    void onInput() override;
    void onOutput() override;
private slots:
    void on_pushButton_pump_clicked();
    void on_pushButton_send_clicked();
};

#endif // WIDGET_PUMP_SEND_INPUTTABLE_H
