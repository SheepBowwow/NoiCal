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
#include "inputbasedialog.h"

namespace Ui {
class Widget_base_inputTable;
}

class Widget_base_inputTable : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_base_inputTable(bool inComponentDB, QWidget *parent = nullptr);
    virtual ~Widget_base_inputTable(); // 虚析构函数


    void setTableWidget(QTableWidget *tableWidget, const QStringList &headerText,const int *columnWidths, int colCount);
    void setTableWidget(QTableWidget *tableWidget, const QStringList &headerText, QVector<int> columnWidths, int colCount);
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

    template<typename ComponentType, typename DialogType>
    void componentRevision(QTableWidget* tableWidget, int mergeColCounts, int row, const QString& name = "");

    virtual void initTableWidget() = 0;
    virtual void clearTableFuc();
    // 加载组件到表格的纯虚函数，需要在子类中实现
    virtual void loadComponentToTable() = 0;
    virtual void handleConfirmation(QSet<QString> uuids) = 0;

    void setTitle(const QString& title);
    void mergeColumnsByNames(QTableWidget* table, const QStringList& columnNames, int mergeRowCount);
    void showConfirmButton();

signals:
    void confirmed(QSet<QString>);

public slots:
    virtual void onAdd() = 0;
    virtual void onDel() = 0;
    virtual void onRevise() = 0;
    virtual void onInput() = 0;
    virtual void onOutput() = 0;
    void onSelectAll();
    void onUnSelectAll();
    virtual void onConfirm();
    virtual void onFilter();
    virtual void clearTable();
    virtual void loadTable();
    void initialize() { initTableWidget(); };

protected:
    Ui::Widget_base_inputTable *ui;
    bool inComponentDB;
    void filterByBrandModel(int brandIndex, int modelIndex, int mergeRowCount, QString brand, QString model);

    QStringList mergeCols;
    int mergeRowCount{1};
    int colCount;
    QVector<int> columnWidths; // 确保这是动态分配的或者足够大的静态数组
    QStringList headerText;


    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};

template<typename ComponentType, typename DialogType>
void Widget_base_inputTable::componentRevision(QTableWidget* tableWidget, int mergeColCounts, int row, const QString& name)
{
    QString uuid = tableWidget->item(row, tableWidget->columnCount() - 1)->text();
    QSharedPointer<ComponentType> component = componentManager.findComponent(inComponentDB, uuid).dynamicCast<ComponentType>();
    if(!component)
        return;

    DialogType *dialog = nullptr;
    if(name != "")
        dialog = new DialogType(name, this, row, *component);
    else
        dialog = new DialogType(this, row, *component);

    dialog->switchToCompontDB(inComponentDB);
    if (dialog->exec() == QDialog::Accepted) {
        QSharedPointer<ComponentType> newComponent = QSharedPointer<ComponentType>(static_cast<ComponentType*>(dialog->getComponent()));

        if (newComponent && componentManager.updateComponent(uuid, newComponent, inComponentDB)) {
            int insertPosition = row; // 记录要插入新行的位置

            //删除旧行
            for(int i = mergeColCounts - 1; i >= 0; i--)
            {
                tableWidget->removeRow(row + i); // 从后往前删除
            }

            auto lists = newComponent->getComponentDataAsStringList(inComponentDB);

            for(int i = 0 ; i < mergeColCounts; i++)
            {
                addRowToTable(tableWidget, lists[i], insertPosition + i);
            }
        }
        delete dialog; // Ensure dialog is deleted to avoid memory leak
    }
}

template<typename ComponentType, typename DialogType>
void Widget_base_inputTable::componentRevision(QTableWidget *tableWidget, int row, QString name)
{
    QWidget* widget = tableWidget->cellWidget(row, 0);
    QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;

    if(!(checkBox && checkBox->isChecked()))
        return;

    QString UUID = tableWidget->item(row, tableWidget->columnCount() - 1)->text();
    auto component = componentManager.findComponent(inComponentDB, UUID).dynamicCast<ComponentType>();
    if (!component)
        return;

    DialogType* dialog = new DialogType(name, new QWidget(), row, *component);

    if (dialog->exec() == QDialog::Accepted) {
        ComponentType* updatedComponentRaw = static_cast<ComponentType*>(dialog->getComponent());
        // 将原始指针转换为QSharedPointer<ComponentBase>
        QSharedPointer<ComponentType> updatedComponent = QSharedPointer<ComponentType>(updatedComponentRaw);
        if (componentManager.updateComponent(UUID, updatedComponent, inComponentDB)) {
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
    auto component = componentManager.findComponent(inComponentDB, UUID).dynamicCast<ComponentType>();
    if (!component)
        return;

    DialogType* dialog = new DialogType(new QWidget(), row, *component);

    if (dialog->exec() == QDialog::Accepted) {
        ComponentType* updatedComponentRaw = static_cast<ComponentType*>(dialog->getComponent());
        // 将原始指针转换为QSharedPointer<ComponentBase>
        QSharedPointer<ComponentType> updatedComponent = QSharedPointer<ComponentType>(updatedComponentRaw);
        if (componentManager.updateComponent(UUID, updatedComponent, inComponentDB)) {
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
    auto component = componentManager.findComponent(inComponentDB, UUID).dynamicCast<ComponentType>();
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
    auto component = componentManager.findComponent(inComponentDB, UUID).dynamicCast<ComponentType>();
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
