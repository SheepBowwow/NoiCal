#include "widget_other_send_terminal_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_other_send_terminal.h"

Widget_Other_send_terminal_inputTable::Widget_Other_send_terminal_inputTable(bool inComponentDB, QWidget *parent)
    : Widget_base_inputTable(inComponentDB, parent)
{
    setTitle("其他通风末端");
    mergeRowCount = 3;
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_Other_send_terminal_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_Other_send_terminal_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_Other_send_terminal_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_Other_send_terminal_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_Other_send_terminal_inputTable::onOutput);
}

void Widget_Other_send_terminal_inputTable::initTableWidget()
{
    colCount = 19;
    headerText<< "" << "序号" << "型号" << "品牌" << "末端\n类型" << "末端\n尺寸" << "数据类型" << "63Hz\n(dB)" << "125Hz\n(dB)"
              << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源" << "备注" << "UUID";  //表头标题用QStringList来表示
    QVector<int> widths = {30, 40, 120, 80, 60, 80, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 120, 0};
    columnWidths = widths;
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);
    QStringList merge
    {
        "",
        "序号",
        "型号",
        "品牌",
        "末端\n类型",
        "静压\n(Pa)",
        "末端\n尺寸",
        "备注",
        "UUID"
    };
    this->mergeCols = merge;
    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_Other_send_terminal_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget *tableWidget = ui->tableWidget;
        Dialog_other_send_terminal *dialog = new Dialog_other_send_terminal(nullptr);
        QSharedPointer<Other_send_terminal> component = nullptr;

        if (dialog->exec() == QDialog::Accepted) {
            if(Other_send_terminal* rawPointer = static_cast<Other_send_terminal*>(dialog->getComponent()))
                component = QSharedPointer<Other_send_terminal>(rawPointer);
            else
                return;
            component->table_id = QString::number(tableWidget->rowCount() / 3 + 1);
            if (component != nullptr) {
                auto lists = component->getComponentDataAsStringList();
                // 使用通用函数添加行
                addRowToTable(tableWidget, lists[0]);
                addRowToTable(tableWidget, lists[1]);
                addRowToTable(tableWidget, lists[2]);

                mergeColumnsByNames(tableWidget, mergeCols, 3);

                componentManager.addComponent(component, inComponentDB);
            }
        }
    }
    else
    {
        Widget_Other_send_terminal_inputTable* inputTable = new Widget_Other_send_terminal_inputTable(true, nullptr);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_Other_send_terminal_inputTable::handleConfirmation);
    }
}

void Widget_Other_send_terminal_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 3);

    mergeColumnsByNames(ui->tableWidget, mergeCols, 3);
}

void Widget_Other_send_terminal_inputTable::onRevise()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); row += 3) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            componentRevision<Other_send_terminal, Dialog_other_send_terminal>(ui->tableWidget, 3, row);
        }
    }
    mergeColumnsByNames(ui->tableWidget, mergeCols, 3);
}

void Widget_Other_send_terminal_inputTable::onInput()
{

}

void Widget_Other_send_terminal_inputTable::onOutput()
{

}

void Widget_Other_send_terminal_inputTable::loadComponentToTable()
{
    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::OTHER_SEND_TERMINAL);
    for (const auto& component : componentList) {
        // 使用dynamic_cast尝试将ComponentBase转换为AirDiff指针
        if (auto otherSendComponent = dynamic_cast<Other_send_terminal*>(component.data())) {
            // 获取组件数据
            auto lists = otherSendComponent->getComponentDataAsStringList(inComponentDB);

            // 假定lists中的数据已经正确分组对应噪声、衰减、反射
            if (lists.size() >= 3) {
                // 添加数据到对应的表格
                addRowToTable(ui->tableWidget, lists[0]);
                addRowToTable(ui->tableWidget, lists[1]);
                addRowToTable(ui->tableWidget, lists[2]);
            }
        }
    }
    mergeColumnsByNames(ui->tableWidget, mergeCols, 3);
}

void Widget_Other_send_terminal_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto& uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);;
        if(QSharedPointer<Other_send_terminal> component = qSharedPointerCast<Other_send_terminal>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<Other_send_terminal> newComponent = QSharedPointer<Other_send_terminal>(new Other_send_terminal(*component));
            if (newComponent != nullptr) {

                auto lists = newComponent->getComponentDataAsStringList(false);

                // 使用通用函数添加行
                addRowToTable(ui->tableWidget, lists[0]);
                addRowToTable(ui->tableWidget, lists[1]);
                addRowToTable(ui->tableWidget, lists[2]);

                componentManager.addComponent(newComponent, false);

                // 重新编号
                for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
                    QTableWidgetItem* item = new QTableWidgetItem(QString::number(row / 3 + 1));
                    ui->tableWidget->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
                    item->setTextAlignment(Qt::AlignCenter);
                    item->setFlags(Qt::ItemIsEditable);
                    item->setBackground(QBrush(Qt::lightGray));
                    item->setData(Qt::ForegroundRole, QColor(70, 70, 70));
                }

                // 更新组件信息
                for (int row = 0; row < ui->tableWidget->rowCount(); row += 3) {
                    QString uuid = ui->tableWidget->item(row, ui->tableWidget->columnCount() - 1)->text(); // 获取组件uuid
                    QSharedPointer<ComponentBase> component = componentManager.findComponent(inComponentDB, uuid); // 查找组件

                    if (component) {
                        component->setTableID((QString::number((row + 3) / 3))); // 设置新的table_id，假设组件有这个方法
                        componentManager.updateComponent(uuid, component, inComponentDB); // 更新组件
                    }
                }

                mergeColumnsByNames(ui->tableWidget, mergeCols, 3);
            }
        }
    }
}
