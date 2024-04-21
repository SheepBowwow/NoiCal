#ifndef WIDGET_AIR_DIFF_INPUTTABLE_H
#define WIDGET_AIR_DIFF_INPUTTABLE_H

#include "widget_base_inputtable.h"
#include <QObject>
#include <QWidget>

class Widget_air_diff_inputTable : public Widget_base_inputTable
{
    Q_OBJECT
public:
    Widget_air_diff_inputTable(QWidget *parent = nullptr);



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

#endif // WIDGET_AIR_DIFF_INPUTTABLE_H
