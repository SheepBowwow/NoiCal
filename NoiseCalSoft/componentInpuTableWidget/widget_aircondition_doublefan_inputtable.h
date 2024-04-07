#ifndef WIDGET_AIRCONDITION_DOUBLEFAN_INPUTTABLE_H
#define WIDGET_AIRCONDITION_DOUBLEFAN_INPUTTABLE_H

#include <QWidget>
#include "componentInpuTableWidget/widget_base_inputtable.h"

class Widget_aircondition_doubleFan_inputTable : public Widget_base_inputTable
{
    Q_OBJECT

public:
    explicit Widget_aircondition_doubleFan_inputTable(QWidget *parent = nullptr);
    ~Widget_aircondition_doubleFan_inputTable();

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
    void mergeCells(int startRow);
};

#endif // WIDGET_AIRCONDITION_DOUBLEFAN_INPUTTABLE_H
