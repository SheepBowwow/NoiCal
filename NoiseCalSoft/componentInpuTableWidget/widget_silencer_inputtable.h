#ifndef WIDGET_SILENCER_INPUTTABLE_H
#define WIDGET_SILENCER_INPUTTABLE_H

#include <QWidget>
#include "componentInpuTableWidget/widget_base_inputtable.h"

namespace Ui {
class Widget_Silencer_inputTable;
}

class Widget_Silencer_inputTable : public Widget_base_inputTable
{
    Q_OBJECT

public:
    explicit Widget_Silencer_inputTable(QWidget *parent = nullptr);
    ~Widget_Silencer_inputTable();

private:
    Ui::Widget_Silencer_inputTable *ui;
    QString silencer_type;


    // Widget_base_inputTable interface
public:
    void initTableWidget() override;

public slots:
    void onAdd() override;
    void onDel() override;
    void onRevise() override;
    void onInput() override;
    void onOutput() override;
private slots:
    void on_pushButton_circle_clicked();
    void on_pushButton_rect_clicked();
    void on_pushButton_circle_elbow_clicked();
    void on_pushButton_rect_elbow_clicked();

    // Widget_base_inputTable interface
public:
    void clearTableFuc() override;


    // Widget_base_inputTable interface
public:
    void loadComponentToTable() override;
};

#endif // WIDGET_SILENCER_INPUTTABLE_H
