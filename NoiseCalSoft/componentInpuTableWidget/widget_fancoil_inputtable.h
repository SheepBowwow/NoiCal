#ifndef WIDGET_FANCOIL_INPUTTABLE_H
#define WIDGET_FANCOIL_INPUTTABLE_H

#include <QWidget>
#include "componentInpuTableWidget/widget_base_inputtable.h"

class Widget_fanCoil_inputTable : public Widget_base_inputTable
{
    Q_OBJECT

public:
    explicit Widget_fanCoil_inputTable(bool inComponentDB, QWidget *parent = nullptr);
    ~Widget_fanCoil_inputTable();


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

private:
    QStringList mergeCols;
    int colCount;
    QVector<int> columnWidths; // 确保这是动态分配的或者足够大的静态数组
    QStringList headerText;

    // Widget_base_inputTable interface
public:
    void loadComponentToTable() override;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // WIDGET_FANCOIL_INPUTTABLE_H
