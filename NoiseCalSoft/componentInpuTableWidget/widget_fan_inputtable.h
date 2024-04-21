#ifndef WIDGET_FAN_INPUTTABLE_H
#define WIDGET_FAN_INPUTTABLE_H

#include <QWidget>
#include "componentInpuTableWidget/widget_base_inputtable.h"

class Widget_fan_inputTable : public Widget_base_inputTable
{
    Q_OBJECT

public:
    explicit Widget_fan_inputTable(QWidget *parent = nullptr);
    ~Widget_fan_inputTable();

    // Widget_base_inputTable interface
public:
    void initTableWidget() override;

public slots:
    void onAdd() override;
    void onDel() override;
    void onRevise() override;
    void onInput() override;
    void onOutput() override;
private:
    QStringList mergeCols;

    // Widget_base_inputTable interface
public:
    void loadComponentToTable() override;
};

#endif // WIDGET_FAN_INPUTTABLE_H
