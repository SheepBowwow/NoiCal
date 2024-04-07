#ifndef WIDGET_TERMINAL_BASE_INPUTTABLE_H
#define WIDGET_TERMINAL_BASE_INPUTTABLE_H

#include <QWidget>
#include "componentInpuTableWidget/widget_base_inputtable.h"

namespace Ui {
class Widget_terminal_base_inputTable;
}

class Widget_terminal_base_inputTable : public Widget_base_inputTable
{
    Q_OBJECT

public:
    explicit Widget_terminal_base_inputTable(QWidget *parent = nullptr);
    virtual ~Widget_terminal_base_inputTable();

    virtual void initTableWidget() = 0;
    void setTitle(const QString& title);
    void setCurrentTable(const int& index);
protected:
    Ui::Widget_terminal_base_inputTable *ui;

public slots:
    virtual void onAdd() = 0;
    virtual void onDel() = 0;
    virtual void onRevise() = 0;
    virtual void onInput() = 0;
    virtual void onOutput() = 0;
};

#endif // WIDGET_TERMINAL_BASE_INPUTTABLE_H
