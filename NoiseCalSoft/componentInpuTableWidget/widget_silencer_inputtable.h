#ifndef WIDGET_SILENCER_INPUTTABLE_H
#define WIDGET_SILENCER_INPUTTABLE_H

#include <QWidget>
#include "componentInpuTableWidget/widget_base_inputtable.h"

class Widget_Silencer_inputTable : public Widget_base_inputTable
{
    Q_OBJECT

public:
    explicit Widget_Silencer_inputTable(bool inComponentDB, const QString& type, QWidget *parent = nullptr);
    ~Widget_Silencer_inputTable();

private:
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
    void onGenerateTemplate() override;
    void handleConfirmation(QSet<QString> uuids) override;

    // Widget_base_inputTable interface
public:
    void loadComponentToTable() override;

private:
    void addComponent(QSharedPointer<Silencer>& component);
};

#endif // WIDGET_SILENCER_INPUTTABLE_H
