#ifndef WIDGET_OTHER_SEND_TERMINAL_INPUTTABLE_H
#define WIDGET_OTHER_SEND_TERMINAL_INPUTTABLE_H

#include "widget_terminal_base_inputtable.h"
#include <QObject>
#include <QWidget>

class Widget_Other_send_terminal_inputTable : public Widget_terminal_base_inputTable
{
    Q_OBJECT
public:
    Widget_Other_send_terminal_inputTable(QWidget *parent = nullptr);



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

#endif // WIDGET_OTHER_SEND_TERMINAL_INPUTTABLE_H