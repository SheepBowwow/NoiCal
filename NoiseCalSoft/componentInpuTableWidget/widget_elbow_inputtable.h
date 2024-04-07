#ifndef WIDGET_ELBOW_INPUTTABLE_H
#define WIDGET_ELBOW_INPUTTABLE_H

#include "widget_base_inputtable.h"
#include <QObject>
#include <QWidget>

class Widget_Elbow_inputTable : public Widget_base_inputTable
{
    Q_OBJECT
public:
    Widget_Elbow_inputTable(QWidget* parent = nullptr);


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

#endif // WIDGET_ELBOW_INPUTTABLE_H
