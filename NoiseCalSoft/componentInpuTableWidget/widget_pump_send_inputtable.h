#ifndef WIDGET_PUMP_SEND_INPUTTABLE_H
#define WIDGET_PUMP_SEND_INPUTTABLE_H

#include <QWidget>
#include "componentInpuTableWidget/widget_base_inputtable.h"

class Widget_Pump_Send_inputTable : public Widget_base_inputTable
{
    Q_OBJECT

public:
    explicit Widget_Pump_Send_inputTable(bool inComponentDB, const QString& type, QWidget *parent = nullptr);
    ~Widget_Pump_Send_inputTable();

    // Widget_base_inputTable interface
public:
    void initTableWidget() override;

private:
    QString type;
    QStringList mergeCols;

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
};

#endif // WIDGET_PUMP_SEND_INPUTTABLE_H
