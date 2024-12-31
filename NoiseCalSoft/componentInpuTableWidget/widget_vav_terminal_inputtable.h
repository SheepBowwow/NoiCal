#ifndef WIDGET_VAV_TERMINAL_INPUTTABLE_H
#define WIDGET_VAV_TERMINAL_INPUTTABLE_H

#include <QWidget>
#include "componentInpuTableWidget/widget_base_inputtable.h"

class Widget_VAV_terminal_inputTable : public Widget_base_inputTable
{
    Q_OBJECT

public:
    explicit Widget_VAV_terminal_inputTable(bool inComponentDB, QWidget *parent = nullptr);
    ~Widget_VAV_terminal_inputTable();


    // Widget_base_inputTable interface
public:
    void initTableWidget() override;

public slots:
    void onAdd() override;
    void onDel() override;
    void onRevise() override;
    void onInput() override;
    void onOutput() override;
    void onGenerateTemplate() override;
    void handleConfirmation(QSet<QString> uuids) override;

    // Widget_base_inputTable interface
public:
    void loadComponentToTable() override;

private:
    void addComponent(QSharedPointer<VAV_terminal>& component);
};

#endif // WIDGET_VAV_TERMINAL_INPUTTABLE_H
