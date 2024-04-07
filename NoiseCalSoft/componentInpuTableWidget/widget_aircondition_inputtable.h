#ifndef WIDGET_AIRCONDITION_INPUTTABLE_H
#define WIDGET_AIRCONDITION_INPUTTABLE_H

#include <QWidget>

namespace Ui {
class Widget_aircondition_inputTable;
}

class Widget_aircondition_inputTable : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_aircondition_inputTable(QWidget *parent = nullptr);
    ~Widget_aircondition_inputTable();

private:
    Ui::Widget_aircondition_inputTable *ui;
};

#endif // WIDGET_AIRCONDITION_INPUTTABLE_H
