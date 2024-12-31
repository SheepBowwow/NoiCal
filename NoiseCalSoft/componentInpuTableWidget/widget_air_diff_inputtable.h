#ifndef WIDGET_AIR_DIFF_INPUTTABLE_H
#define WIDGET_AIR_DIFF_INPUTTABLE_H

#include "widget_base_inputtable.h"
#include <QObject>
#include <QWidget>

class Widget_air_diff_inputTable : public Widget_base_inputTable
{
    Q_OBJECT
public:
    Widget_air_diff_inputTable(bool inComponentDB, QWidget *parent = nullptr);



    // Widget_base_inputTable interface
public:
    void initTableWidget() override;
    void filterByBrandModel(int air_distributor_brandIndex, int air_distributor_modelIndex,
                            int diffuser_brandIndex, int diffuser_modelIndex,
                            QString air_distributor_brand, QString air_distributor_model,
                            QString diffuser_brand, QString diffuser_model);

public slots:
    void onAdd() override;
    void onDel() override;
    void onRevise() override;
    void onInput() override;
    void onOutput() override;
    void onFilter() override;
    void onGenerateTemplate() override;
    void handleConfirmation(QSet<QString> uuids) override;

    // Widget_base_inputTable interface
public:
    void loadComponentToTable() override;

private:
    QDialog* dialog;

    void addComponent(QSharedPointer<AirDiff>& component);
};

#endif // WIDGET_AIR_DIFF_INPUTTABLE_H
