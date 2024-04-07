#ifndef WIDGET_CIRCULAR_DAMPER_INPUTTABLE_H
#define WIDGET_CIRCULAR_DAMPER_INPUTTABLE_H

#include <QWidget>
#include "componentInpuTableWidget/widget_base_inputtable.h"

class Widget_Circular_damper_inputTable : public Widget_base_inputTable
{
    Q_OBJECT

public:
    explicit Widget_Circular_damper_inputTable(QWidget *parent = nullptr);
    ~Widget_Circular_damper_inputTable();

    // Widget_base_inputTable interface
public:
    void initTableWidget() override;

public slots:
    void onAdd() override;
    void onDel() override;
    void onRevise() override;
    void onInput() override;
    void onOutput() override;
};

#endif // WIDGET_CIRCULAR_DAMPER_INPUTTABLE_H
