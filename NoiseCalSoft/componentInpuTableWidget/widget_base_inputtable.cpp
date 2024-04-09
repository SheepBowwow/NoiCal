#include "widget_base_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "globle_var.h"
#include "project/projectmanager.h"
#include "Component/ComponentManager.h"
#include <QQueue>

Widget_base_inputTable::Widget_base_inputTable(QWidget *parent,  bool setupUi) :
    QWidget(parent),
    ui(new Ui::Widget_base_inputTable)
{
    if (setupUi) {
        ui->setupUi(this);
    }

    connect(&ProjectManager::getInstance(), &ProjectManager::clearTable, this, &Widget_base_inputTable::clearTable);
    connect(&ComponentManager::getInstance(),&ComponentManager::loadComponentsDone, this, &Widget_base_inputTable::loadTable);
}

Widget_base_inputTable::~Widget_base_inputTable()
{
    delete ui;
}

void Widget_base_inputTable::setTitle(const QString& title)
{
    ui->label_title->setText(title);
    ui->label_title->adjustSize();
}

void Widget_base_inputTable::clearTableFuc()
{
    // 定义一个Lambda表达式用于递归清空QTableWidgets
    std::function<void(QWidget*)> clearAllTableWidgets = [&](QWidget* parent) {
        if (!parent) return;

        // 创建一个队列用于广度优先搜索
        QQueue<QWidget*> queue;
        queue.enqueue(parent);

        while (!queue.isEmpty()) {
            QWidget* currentWidget = queue.dequeue();

            // 尝试将当前控件转换为QTableWidget
            QTableWidget *tableWidget = qobject_cast<QTableWidget*>(currentWidget);
            if (tableWidget) {
                // 如果转换成功，清空QTableWidget
                tableWidget->setRowCount(0);
                tableWidget->clearContents(); // 清空内容但保留列头
            } else {
                // 如果当前控件不是QTableWidget，将它的子控件加入队列
                const auto children = currentWidget->children();
                for (QObject *child : children) {
                    QWidget *childWidget = qobject_cast<QWidget*>(child);
                    if (childWidget) {
                        queue.enqueue(childWidget);
                    }
                }
            }
        }
    };

    // 使用定义的Lambda表达式从该窗口控件开始递归清空所有QTableWidget
    clearAllTableWidgets(this);
}

void Widget_base_inputTable::clearTable()
{
    clearTableFuc();
}

void Widget_base_inputTable::loadTable()
{
    loadComponentToTable();
}

/**
 * 函数简介: 初始化表格
 * 参数说明:
 *  - tableWidget : 要操作的表格
 *  - headerText : 表头文本
 *  - columnWidths : 列宽比
 *  - colCount : 列数
 * 返回值: void
 */
void Widget_base_inputTable::setTableWidget(QTableWidget *tableWidget, const QStringList &headerText, const int *columnWidths, int colCount)
{
    tableWidget->setColumnCount(colCount);
    tableWidget->setRowCount(0);
    tableWidget->setHorizontalHeaderLabels(headerText);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    tableWidget->setFocusPolicy(Qt::NoFocus);
    int totalWidth = 0;
    for (int i = 0; i < colCount; ++i)
    {
        totalWidth += columnWidths[i];
    }

    for (int i = 0; i < colCount; ++i)
    {
        double ratio = static_cast<double>(columnWidths[i]) / totalWidth;
        int columnWidth = static_cast<int>(ratio * tableWidget->width());
        tableWidget->setColumnWidth(i, columnWidth);
    }

    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

/**
 * 函数简介: 添加一行数据到表格中, 第一列添加
 * 参数说明:
 *  - tableWidget : 要操作的表格
 *  - data : 要插入的数据
 * 返回值: void
 */
void Widget_base_inputTable::addRowToTable(QTableWidget *tableWidget, const QStringList &data, int position)
{
    int rowCount = tableWidget->rowCount();
    // 如果position为负数，添加到末尾；否则，插入到指定位置
    if (position < 0 || position > rowCount) {
        tableWidget->setRowCount(rowCount + 1);
        position = rowCount; // 添加到末尾
    } else {
        tableWidget->insertRow(position);
    }

    for (int i = 0; i < data.size() + 1; ++i) {
        if (i == 0) {
            // 处理复选框
            QCheckBox* checkBox = new QCheckBox();
            QWidget* widget = new QWidget();
            widget->setStyleSheet("background-color: #C0C0C0;");
            QHBoxLayout* layout = new QHBoxLayout(widget);
            layout->addWidget(checkBox);
            layout->setAlignment(Qt::AlignCenter);
            layout->setContentsMargins(0, 0, 0, 0);
            tableWidget->setCellWidget(position, i, widget);

        } else {
            QTableWidgetItem *item = new QTableWidgetItem(data[i - 1]);
            tableWidget->setItem(position, i, item);
            item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 设置为只读
            item->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
            item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色
        }
    }
}

/**
 * 函数简介: 删除行(非末端部件)
 * 参数说明:
 *  - tableWidget : 要操作的表格
 *  - deleteRowNum : 要删除的行数
 *  - componentName : 要删除的部件名称
 * 返回值: void
 */
void Widget_base_inputTable::deleteRowFromTable(QTableWidget *tableWidget, int deleteRowNum)
{
    // 获取选中的行索引
    QList<int> selectedRows;
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        QWidget* widget = tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox && checkBox->isChecked()) {
            selectedRows.append(row);
        }
    }

    // 弹窗确认
    QString confirmationMessage = "确认删除以下行吗？\n";
    for (int i = 0; i < selectedRows.size(); ++i)
    {
        int row = selectedRows[i];
        confirmationMessage += QString::number((row + deleteRowNum) / deleteRowNum) + "\n"; // 从1开始计数
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("确认删除");
    msgBox.setText(confirmationMessage);
    msgBox.setIcon(QMessageBox::Warning);
    QPushButton *yesButton = msgBox.addButton("确认", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("取消", QMessageBox::NoRole);
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton)
    {
        QString UUID;
        for (int i = selectedRows.size() - 1; i >= 0; --i)
        {
            int row = selectedRows[i];
            UUID = tableWidget->item(row, tableWidget->columnCount() - 1)->text();
            componentManager.removeComponent(UUID);
            tableWidget->removeRow(row);
            if(deleteRowNum != 1)
            {
                for(int i = 1; i <= deleteRowNum - 1; i++)
                {
                    tableWidget->removeRow(row);
                }
            }
        }


        // 重新编号
        for (int row = 0; row < tableWidget->rowCount(); ++row) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(deleteRowNum == 1 ? (row + 1) : (row / deleteRowNum + 1)));
            tableWidget->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(Qt::ItemIsEditable);
            item->setBackground(QBrush(Qt::lightGray));
            item->setData(Qt::ForegroundRole, QColor(70, 70, 70));
        }

        // 更新组件信息
        for (int row = 0; row < tableWidget->rowCount(); row+=deleteRowNum) {
            QString uuid = tableWidget->item(row, tableWidget->columnCount() - 1)->text(); // 获取组件uuid
            QSharedPointer<ComponentBase> component = componentManager.findComponent(uuid); // 查找组件

            if (component) {
                component->setTableID((QString::number((row + deleteRowNum) / deleteRowNum))); // 设置新的table_id，假设组件有这个方法
                componentManager.updateComponent(uuid, component); // 更新组件
            }
        }
    }
    else if (msgBox.clickedButton() == noButton)
    {
        return;
    }

}

/**
 * 函数简介: 删除行(末端部件)
 * 参数说明:
 *  - tableWidget_noise : 要操作的末端噪音表格
 *  - tableWidget_atten : 要操作的末端衰减表格
 *  - tableWidget_refl : 要操作的末端反射衰减表格
 *  - componentName : 要删除的部件名称
 * 返回值: void
 */
void Widget_base_inputTable::deleteRowFromTable(QTableWidget *tableWidget_noise, QTableWidget *tableWidget_atten, QTableWidget *tableWidget_refl)
{
    // 获取选中的行索引
    QList<int> selectedRows;
    QVector<QTableWidget *> tableWidgets= {tableWidget_noise,tableWidget_atten,tableWidget_refl};
    for(int i = 0; i < tableWidgets.size(); i++)
    {
        for (int row = 0; row < tableWidgets[i]->rowCount(); ++row) {
            QWidget* widget = tableWidgets[i]->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
            QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

            if (checkBox && checkBox->isChecked()) {
                selectedRows.append(row);
            }
        }
    }

    // 弹窗确认
    QString confirmationMessage = "确认删除以下行吗？\n";
    for (int i = 0; i < selectedRows.size(); ++i)
    {
        int row = selectedRows[i];
        confirmationMessage += QString::number(row + 1) + "\n"; // 从1开始计数
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("确认删除");
    msgBox.setText(confirmationMessage);
    msgBox.setIcon(QMessageBox::Warning);
    QPushButton *yesButton = msgBox.addButton("确认", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("取消", QMessageBox::NoRole);
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton)
    {        
        QString UUID;
        for (int i = selectedRows.size() - 1; i >= 0; --i)
        {
            int row = selectedRows[i];
            UUID = tableWidget_noise->item(row , tableWidget_noise->columnCount() - 1)->text();
            ComponentManager::getInstance().removeComponent(UUID);
            tableWidget_noise->removeRow(row);
            tableWidget_atten->removeRow(row);
            tableWidget_refl->removeRow(row);
        }


        // 重新编号
        for(int i = 0; i < tableWidgets.size(); i++)
        {
            for (int row = 0; row < tableWidgets[i]->rowCount(); ++row) {
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(row + 1));
                tableWidgets[i]->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
                item->setTextAlignment(Qt::AlignCenter);
                item->setFlags(Qt::ItemIsEditable);
                item->setBackground(QBrush(Qt::lightGray));
                item->setData(Qt::ForegroundRole, QColor(70, 70, 70));
            }
        }

        // 更新组件信息
        for (int row = 0; row < tableWidget_noise->rowCount(); row++) {
            QString uuid = tableWidget_noise->item(row, tableWidget_noise->columnCount() - 1)->text(); // 获取组件uuid
            QSharedPointer<ComponentBase> component = componentManager.findComponent(uuid); // 查找组件

            if (component) {
                component->setTableID(QString::number(row + 1)); // 设置新的table_id，假设组件有这个方法
                componentManager.updateComponent(uuid, component); // 更新组件
            }
        }
    }
    else if(msgBox.clickedButton() == noButton)
    {
        return;
    }

}
