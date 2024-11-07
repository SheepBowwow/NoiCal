#ifndef WIDGET_RECT_DAMPER_INPUTTABLE_H
#define WIDGET_RECT_DAMPER_INPUTTABLE_H

#include "widget_base_inputtable.h"
#include <QObject>
#include <QWidget>

class Widget_Rect_damper_inputTable : public Widget_base_inputTable
{
    Q_OBJECT
public:
    Widget_Rect_damper_inputTable(bool inComponentDB, QWidget* parent = nullptr);

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
};

#endif // WIDGET_RECT_DAMPER_INPUTTABLE_H
