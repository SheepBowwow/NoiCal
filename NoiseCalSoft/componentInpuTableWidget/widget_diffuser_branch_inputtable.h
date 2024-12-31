#ifndef WIDGET_DIFFUSER_BRANCH_INPUTTABLE_H
#define WIDGET_DIFFUSER_BRANCH_INPUTTABLE_H

#include <QObject>
#include <QWidget>
#include "widget_base_inputtable.h"

class Widget_Diffuser_branch_inputTable : public Widget_base_inputTable
{
    Q_OBJECT
public:
    Widget_Diffuser_branch_inputTable(bool inComponentDB, QWidget* parent = nullptr);

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
    void addComponent(QSharedPointer<Diffuser_branch>& component);
};

#endif // WIDGET_DIFFUSER_BRANCH_INPUTTABLE_H
