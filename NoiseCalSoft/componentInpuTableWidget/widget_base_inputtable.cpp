#include "widget_base_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "globle_var.h"
#include "project/projectmanager.h"
#include "Component/ComponentManager.h"
#include <QQueue>
#include <QComboBox>
#include <QLabel>

Widget_base_inputTable::Widget_base_inputTable(bool inComponentDB, QWidget *parent) :
    QWidget(parent),
    inComponentDB(inComponentDB),
    ui(new Ui::Widget_base_inputTable)
{
    ui->setupUi(this);

    // 假设horizontalLayout包含一个按钮和一个弹簧
    ui->pushButton_confirm->hide(); // 隐藏按钮

    if(!inComponentDB) {
        ui->pushButton_input->hide();
        ui->pushButton_generate_template->hide();
    }

    // 假设horizontalLayout是在verticalLayout中
    // 我们需要找到并移除弹簧，因为弹簧不能直接隐藏
    for (int i = 0; i < ui->horizontalLayout_confirm->count(); ++i) {
        QLayoutItem* item = ui->horizontalLayout_confirm->itemAt(i);
        if (item->spacerItem()) {
            ui->horizontalLayout_confirm->removeItem(item);
            delete item; // 删除弹簧
            break;
        }
    }

    // 假设我们还想确保布局适当地调整大小
    ui->widget->layout()->invalidate(); // 标记布局需要重新计算
    ui->widget->layout()->update(); // 请求更新布局

    connect(&ProjectManager::getInstance(), &ProjectManager::clearTable, this, &Widget_base_inputTable::clearTable);
    connect(&ComponentManager::getInstance(),&ComponentManager::loadComponentsDone, this, &Widget_base_inputTable::loadTable);
    connect(ui->pushButton_filter, &QPushButton::clicked, this, &Widget_base_inputTable::onFilter);
    connect(ui->pushButton_confirm, &QPushButton::clicked, this, &Widget_base_inputTable::onConfirm);
    connect(ui->pushButton_selectAll, &QPushButton::clicked, this, &Widget_base_inputTable::onSelectAll);
    connect(ui->pushButton_unSelectAll, &QPushButton::clicked, this, &Widget_base_inputTable::onUnSelectAll);
    connect(ui->pushButton_generate_template, &QPushButton::clicked, this, &Widget_base_inputTable::onGenerateTemplate);
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

void Widget_base_inputTable::filterByBrandModel(int brandIndex, int modelIndex, int mergeRowCount, QString brand, QString model)
{
    // 如果选择所有品牌，取消所有隐藏并返回
    if (brand == "所有品牌") {
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
            ui->tableWidget->setRowHidden(i, false);
        }
    }
    else
    {
        // 遍历表格中的行，考虑mergeRowCount定义的行合并
        for (int i = 0; i < ui->tableWidget->rowCount(); i += mergeRowCount) {
            bool match = true;

            // 检查当前行和下一行是否匹配品牌
            for (int j = 0; j < mergeRowCount; ++j) {
                int currentRow = i + j;
                if (currentRow >= ui->tableWidget->rowCount()) break;

                QTableWidgetItem *brandItem = ui->tableWidget->item(currentRow, brandIndex);
                QTableWidgetItem *modelItem = ui->tableWidget->item(currentRow, modelIndex);

                // 检查品牌匹配，如果品牌匹配且未选择所有型号，则检查型号
                if (!brandItem || brandItem->text() != brand || (model != "所有型号" && (!modelItem || modelItem->text() != model))) {
                    match = false;
                    break;
                }
            }

            // 根据匹配结果显示或隐藏整个逻辑行
            for (int j = 0; j < mergeRowCount; ++j) {
                int currentRow = i + j;
                if (currentRow >= ui->tableWidget->rowCount()) break;

                ui->tableWidget->setRowHidden(currentRow, !match);
            }
        }
    }

    int visibleIndex = 0;  // 用于给可见行编号，起始编号为0
    // 重新编号
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        if (!ui->tableWidget->isRowHidden(row))
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(mergeRowCount == 1 ? (visibleIndex++ + 1) : (visibleIndex++ / mergeRowCount + 1)));
            ui->tableWidget->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(Qt::ItemIsEditable);
            item->setBackground(QBrush(Qt::lightGray));
            item->setData(Qt::ForegroundRole, QColor(70, 70, 70));
        }
    }
}

void Widget_base_inputTable::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); // Call the base class implementation

    if (!ui->tableWidget)
        return;

    double totalWidth = 0;
    for (int i = 0; i < colCount; ++i)
    {
        totalWidth += columnWidths[i];
    }
    for (int i = 0; i < colCount; ++i)
    {
        int columnWidth = static_cast<int>(static_cast<double>(columnWidths[i]) * ui->tableWidget->width() / totalWidth);
        ui->tableWidget->setColumnWidth(i, columnWidth);
    }
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
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    // 强制更新表格布局
    tableWidget->update();
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
void Widget_base_inputTable::setTableWidget(QTableWidget *tableWidget, const QStringList &headerText, QVector<int> columnWidths, int colCount)
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
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    // 强制更新表格布局
    tableWidget->update();
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
        QString rowNum = QString::number((row + deleteRowNum) / deleteRowNum);
        if(!confirmationMessage.contains(rowNum))
            confirmationMessage += rowNum + "\n"; // 从1开始计数
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
            componentManager.removeComponent(UUID, inComponentDB);
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
            QSharedPointer<ComponentBase> component = componentManager.findComponent(inComponentDB, uuid); // 查找组件

            if (component) {
                component->setTableID((QString::number((row + deleteRowNum) / deleteRowNum))); // 设置新的table_id，假设组件有这个方法
                componentManager.updateComponent(uuid, component, inComponentDB); // 更新组件
            }
        }

        // 清除选中状态和焦点
        tableWidget->clearSelection();
        tableWidget->clearFocus();
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
            ComponentManager::getInstance().removeComponent(UUID, inComponentDB);
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
            QSharedPointer<ComponentBase> component = componentManager.findComponent(inComponentDB, uuid); // 查找组件

            if (component) {
                component->setTableID(QString::number(row + 1)); // 设置新的table_id，假设组件有这个方法
                componentManager.updateComponent(uuid, component, inComponentDB); // 更新组件
            }
        }
    }
    else if(msgBox.clickedButton() == noButton)
    {
        return;
    }

}

void Widget_base_inputTable::mergeColumnsByNames(QTableWidget* table, const QStringList& columnNames, int mergeRowCount) {
    if (table == nullptr || mergeRowCount < 2 || table->columnCount() == 0) return;

    // 找到需要合并的列索引
    QList<int> columnsToMerge;
    for (const QString& name : columnNames) {
        for (int i = 0; i < table->columnCount(); ++i) {
            if (table->horizontalHeaderItem(i)->text() == name) {
                columnsToMerge.append(i);
                break;
            }
        }
    }

    // 对指定的列进行行合并
    for (int col : columnsToMerge) {
        for (int row = 0; row < table->rowCount(); row += mergeRowCount) {
            int mergeTill = row + mergeRowCount - 1;
            if (mergeTill >= table->rowCount()) mergeTill = table->rowCount() - 1;

            QTableWidgetItem* firstItem = table->item(row, col);
            if (!firstItem) {
                firstItem = new QTableWidgetItem();
                table->setItem(row, col, firstItem);
            }
            table->setSpan(row, col, mergeTill - row + 1, 1);

            for(int i = row; i < row + 2; i++)
            {
                QWidget* widget = table->cellWidget(row, 0);
                if (widget != nullptr) {
                    // 获取widget里所有的子部件
                    QList<QWidget*> widgets = widget->findChildren<QWidget*>();
                    // 遍历子部件并删除
                    foreach(QWidget* childWidget, widgets) {
                        delete childWidget;
                    }
                }
            }

            // 处理复选框
            QCheckBox* checkBox = new QCheckBox();
            QWidget* widget = new QWidget();
            widget->setStyleSheet("background-color: #C0C0C0;");
            QHBoxLayout* layout = new QHBoxLayout(widget);
            layout->addWidget(checkBox);
            layout->setAlignment(Qt::AlignCenter);
            layout->setContentsMargins(0, 0, 0, 0);
            table->setCellWidget(row, 0, widget);
        }
    }
}

void Widget_base_inputTable::showConfirmButton()
{
    // 显示之前隐藏的按钮
    ui->pushButton_confirm->show();
    ui->pushButton_input->hide();
    ui->pushButton_output->hide();
    ui->pushButton_revise->hide();
    ui->pushButton_del->hide();
    ui->pushButton_add->hide();

    // 重新添加弹簧到水平布局
    QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->horizontalLayout_confirm->addSpacerItem(spacer);
    ui->horizontalLayout_confirm->addWidget(ui->pushButton_confirm); // 确保按钮在水平布局中的正确位置

    // 假设我们还想确保布局适当地调整大小
    ui->widget->layout()->invalidate(); // 标记布局需要重新计算
    ui->widget->layout()->update(); // 请求更新布局
}

void Widget_base_inputTable::onSelectAll()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox) {
            checkBox->setChecked(true);
        }
    }
}

void Widget_base_inputTable::onUnSelectAll()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox) {
            checkBox->setChecked(false);
        }
    }
}

void Widget_base_inputTable::onConfirm()
{
    QSet<QString> uuids;
    int colCount = ui->tableWidget->columnCount();
    // 重新编号
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox && checkBox->isChecked()) {
            QString uuid = ui->tableWidget->item(row, colCount - 1)->text();
            uuids.insert(uuid);
        }
    }
    this->close();

    emit confirmed(uuids);
}

void Widget_base_inputTable::onFilter()
{
    QString selectedBrand;
    QString selectedModel;
    QDialog* dialog = new QDialog(nullptr);
    dialog->setWindowTitle("筛选");
    // 创建标签和下拉选择框
    QLabel *modelLabel = new QLabel("型号:");
    QComboBox *modelComboBox = new QComboBox();
    int modelIndex = -1;
    for (int i = 0; i < ui->tableWidget->columnCount(); ++i) {
        if (ui->tableWidget->horizontalHeaderItem(i)->text() == "型号") {
            modelIndex = i;
            break;
        }
    }
    modelComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QLabel *brandLabel = new QLabel("品牌:");
    QComboBox *brandComboBox = new QComboBox();
    int brandIndex = -1;
    for (int i = 0; i < ui->tableWidget->columnCount(); ++i) {
        if (ui->tableWidget->horizontalHeaderItem(i)->text() == "品牌") {
            brandIndex = i;
            break;
        }
    }
    brandComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    brandComboBox->addItem("所有品牌");
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QString brand = ui->tableWidget->item(i, brandIndex)->text();
        if(brandComboBox->findText(brand) == -1)
            brandComboBox->addItem(brand);
    }
    brandComboBox->setCurrentIndex(-1);
    connect(brandComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=, this](int index) {
        if (index < 0 || brandIndex == -1 || modelIndex == -1) return;  // 验证索引有效性

        QString selectedBrand = brandComboBox->itemText(index);
        modelComboBox->clear();  // 清空型号下拉列表
        modelComboBox->addItem("所有型号");
        // 遍历表格，寻找匹配的型号
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
            QTableWidgetItem* brandItem = ui->tableWidget->item(i, brandIndex);
            if (brandItem && (brandItem->text() == selectedBrand)) {
                QTableWidgetItem* modelItem = ui->tableWidget->item(i, modelIndex);
                if (modelItem && modelComboBox->findText(modelItem->text()) == -1) {
                    modelComboBox->addItem(modelItem->text());
                }
            }
        }
    });

    // 创建确认按钮
    QPushButton *confirmButton = new QPushButton("确认");
    connect(confirmButton, &QPushButton::clicked, dialog, &QDialog::accept);

    // 创建水平布局来放置型号标签和下拉框
    QHBoxLayout *modelLayout = new QHBoxLayout;
    modelLayout->addWidget(modelLabel);
    modelLayout->addWidget(modelComboBox);

    // 创建水平布局来放置品牌标签和下拉框
    QHBoxLayout *brandLayout = new QHBoxLayout;
    brandLayout->addWidget(brandLabel);
    brandLayout->addWidget(brandComboBox);

    // 创建水平布局来放置确认按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    // 创建一个弹簧
    QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    // 将弹簧添加到布局中
    buttonLayout->addSpacerItem(spacer);
    // 将确认按钮添加到布局中
    buttonLayout->addWidget(confirmButton);

    // 创建垂直布局来组合以上布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(brandLayout);
    mainLayout->addLayout(modelLayout);
    mainLayout->addLayout(buttonLayout);

    // 设置对话框的主布局
    dialog->setLayout(mainLayout);

    if(dialog->exec() == QDialog::Accepted)
    {
        // 在对话框被接受后保存数据
        selectedBrand = brandComboBox->currentText();
        selectedModel = modelComboBox->currentText();

        // 现在可以使用 selectedBrand 和 selectedModel 进行其他操作
        qDebug() << "Selected Brand: " << selectedBrand;
        qDebug() << "Selected Model: " << selectedModel;

        filterByBrandModel(brandIndex, modelIndex, mergeRowCount, selectedBrand, selectedModel);
    }
    delete dialog;
}
