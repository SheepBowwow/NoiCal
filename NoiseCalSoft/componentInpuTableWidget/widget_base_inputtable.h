#ifndef WIDGET_BASE_INPUTTABLE_H
#define WIDGET_BASE_INPUTTABLE_H

#include <QWidget>
#include <QObject>
#include <QTableWidget>
#include <QHeaderView>
#include <QCheckBox>
#include <QHBoxLayout>
#include "Component/ComponentManager.h"
#include <QMessageBox>
#include "globle_var.h"
#include <QPushButton>

namespace Ui {
class Widget_base_inputTable;
}

class Widget_base_inputTable : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_base_inputTable(QWidget *parent = nullptr, bool setupUi = true);
    virtual ~Widget_base_inputTable(); // 虚析构函数


    void setTableWidget(QTableWidget *tableWidget, const QStringList &headerText, const int *columnWidths, int colCount);
    void addRowToTable(QTableWidget *tableWidget, const QStringList &data, int position = -1);
    void deleteRowFromTable(QTableWidget *tableWidget, int deleteRowNum);
    void deleteRowFromTable(QTableWidget *tableWidget_noise, QTableWidget *tableWidget_atten, QTableWidget *tableWidget_refl);

    template<typename ComponentType, typename DialogType>
    void componentRevision(QTableWidget *tableWidget, int row, QString name);
    template<typename ComponentType, typename DialogType>
    void componentRevision(QTableWidget *tableWidget, int row);
    template<typename ComponentType, typename DialogType>
    void componentRevision(QVector<QTableWidget*> tableWidgets, QTableWidget *currentTableWidget, int row, QString name);
    template<typename ComponentType, typename DialogType>
    void componentRevision(QVector<QTableWidget*> tableWidgets, QTableWidget *currentTableWidget, int row);

    virtual void initTableWidget() = 0;
    virtual void clearTableFuc();
    // 加载组件到表格的纯虚函数，需要在子类中实现
    virtual void loadComponentToTable() = 0;

    void initialize() { initTableWidget(); };

    void setTitle(const QString& title);
public slots:
    virtual void onAdd() = 0;
    virtual void onDel() = 0;
    virtual void onRevise() = 0;
    virtual void onInput() = 0;
    virtual void onOutput() = 0;
    virtual void clearTable();
    virtual void loadTable();

protected:
    Ui::Widget_base_inputTable *ui;
};

template<typename ComponentType, typename DialogType>
void Widget_base_inputTable::componentRevision(QTableWidget *tableWidget, int row, QString name)
{
    QWidget* widget = tableWidget->cellWidget(row, 0);
    QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;

    if(!(checkBox && checkBox->isChecked()))
        return;

    QString UUID = tableWidget->item(row, tableWidget->columnCount() - 1)->text();
    auto component = componentManager.findComponent(UUID).dynamicCast<ComponentType>();
    if (!component)
        return;

    DialogType* dialog = new DialogType(name, new QWidget(), row, *component);

    if (dialog->exec() == QDialog::Accepted) {
        ComponentType* updatedComponentRaw = static_cast<ComponentType*>(dialog->getComponent());
        // 将原始指针转换为QSharedPointer<ComponentBase>
        QSharedPointer<ComponentType> updatedComponent = QSharedPointer<ComponentType>(updatedComponentRaw);
        if (componentManager.updateComponent(UUID, updatedComponent)) {
            int insertPosition = row; // 记录要插入新行的位置
            tableWidget->removeRow(row); // 再删除当前行
            auto lists = dialog->getComponentDataAsStringList();
            addRowToTable(tableWidget, lists[0], insertPosition);
        }
    }
}

template<typename ComponentType, typename DialogType>
void Widget_base_inputTable::componentRevision(QTableWidget *tableWidget, int row)
{
    QWidget* widget = tableWidget->cellWidget(row, 0);
    QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;

    if(!(checkBox && checkBox->isChecked()))
        return;

    QString UUID = tableWidget->item(row, tableWidget->columnCount() - 1)->text();
    auto component = componentManager.findComponent(UUID).dynamicCast<ComponentType>();
    if (!component)
        return;

    DialogType* dialog = new DialogType(new QWidget(), row, *component);

    if (dialog->exec() == QDialog::Accepted) {
        ComponentType* updatedComponentRaw = static_cast<ComponentType*>(dialog->getComponent());
        // 将原始指针转换为QSharedPointer<ComponentBase>
        QSharedPointer<ComponentType> updatedComponent = QSharedPointer<ComponentType>(updatedComponentRaw);
        if (componentManager.updateComponent(UUID, updatedComponent)) {
            int insertPosition = row; // 记录要插入新行的位置
            tableWidget->removeRow(row); // 再删除当前行
            auto lists = dialog->getComponentDataAsStringList();
            addRowToTable(tableWidget, lists[0], insertPosition);
        }
    }
}

template<typename ComponentType, typename DialogType>
void Widget_base_inputTable::componentRevision(QVector<QTableWidget *> tableWidgets, QTableWidget *currentTableWidget, int row, QString name)
{
    QWidget* widget = currentTableWidget->cellWidget(row, 0);
    QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;

    if(!(checkBox && checkBox->isChecked()))
        return;

    QString UUID = currentTableWidget->item(row, currentTableWidget->columnCount() - 1)->text();
    auto component = componentManager.findComponent(UUID).dynamicCast<ComponentType>();
    if (!component)
        return;

    DialogType* dialog = new DialogType(name, new QWidget(), row, *component);

    if (dialog->exec() == QDialog::Accepted) {
        ComponentType* updatedComponentRaw = static_cast<ComponentType*>(dialog->getComponent());
        // 将原始指针转换为QSharedPointer<ComponentBase>
        QSharedPointer<ComponentType> updatedComponent = QSharedPointer<ComponentType>(updatedComponentRaw);
        if (componentManager.updateComponent(UUID, updatedComponent)) {
            int insertPosition = row; // 记录要插入新行的位置
            auto lists = dialog->getComponentDataAsStringList();
            for(int i = 0; i < tableWidgets.size(); i++)
            {
                tableWidgets[i]->removeRow(row); // 再删除当前行
                addRowToTable(tableWidgets[i], lists[i], insertPosition);
            }
        }
    }
}


template<typename ComponentType, typename DialogType>
void Widget_base_inputTable::componentRevision(QVector<QTableWidget *> tableWidgets, QTableWidget *currentTableWidget, int row)
{
    QWidget* widget = currentTableWidget->cellWidget(row, 0);
    QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;

    if(!(checkBox && checkBox->isChecked()))
        return;

    QString UUID = currentTableWidget->item(row, currentTableWidget->columnCount() - 1)->text();
    auto component = componentManager.findComponent(UUID).dynamicCast<ComponentType>();
    if (!component)
        return;

    DialogType* dialog = new DialogType(new QWidget(), row, *component);

    if (dialog->exec() == QDialog::Accepted) {
        ComponentType* updatedComponentRaw = static_cast<ComponentType*>(dialog->getComponent());
        // 将原始指针转换为QSharedPointer<ComponentBase>
        QSharedPointer<ComponentType> updatedComponent = QSharedPointer<ComponentType>(updatedComponentRaw);
        if (componentManager.updateComponent(UUID, updatedComponent)) {
            int insertPosition = row; // 记录要插入新行的位置
            auto lists = dialog->getComponentDataAsStringList();
            for(int i = 0; i < tableWidgets.size(); i++)
            {
                tableWidgets[i]->removeRow(row); // 再删除当前行
                addRowToTable(tableWidgets[i], lists[i], insertPosition);
            }
        }
    }
}

#endif // WIDGET_BASE_INPUTTABLE_H
