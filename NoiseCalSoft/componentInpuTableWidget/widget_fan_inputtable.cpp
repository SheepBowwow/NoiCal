#include "widget_fan_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "Component/ComponentStructs.h"
#include "globle_var.h"
#include "inputDialog/dialog_fan.h"

Widget_fan_inputTable::Widget_fan_inputTable(bool inComponentDB, QWidget *parent) :
    Widget_base_inputTable(inComponentDB, parent)
{
    setTitle("风机 ");
    mergeRowCount = 2;
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_fan_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_fan_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_fan_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_fan_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_fan_inputTable::onOutput);
}

Widget_fan_inputTable::~Widget_fan_inputTable()
{
}

/**
 * @brief Widget_fan_inputTable::initTableWidget
 *
 * 初始化表格, 设置表头
 */
void Widget_fan_inputTable::initTableWidget()
{
    if(!inComponentDB)
    {
        colCount = 19;
        headerText << "" << "序号" << "编号" << "品牌" << "型号" << "风量\n(m³/h)" << "静压\n(Pa)" << "噪音\n位置" << "63Hz\n(dB)"
                   << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
                   << "8kHz\n(dB)" << "总值\ndB(A)" << "来源" << "UUID";
        // 设置每列的宽度
        QVector<int> widths = {30, 38, 120, 60, 120, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};
        columnWidths = widths;
    }
    else
    {
        colCount = 18;
        headerText << "" << "序号" << "品牌" << "型号" << "风量\n(m³/h)" << "静压\n(Pa)" << "噪音\n位置" << "63Hz\n(dB)"
                   << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
                   << "8kHz\n(dB)" << "总值\ndB(A)" << "来源" << "UUID";
        QVector<int> widths = {30, 38, 60, 120, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};
        columnWidths = widths;
    }

    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);
    QStringList merge
    {
        "",
        "序号",
        "编号",
        "型号",
        "品牌",
        "风量\n(m³/h)",
        "静压\n(Pa)",
        "来源",
        "UUID"
    };
    if(inComponentDB)   merge.removeOne("编号");
    this->mergeCols = merge;

    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_fan_inputTable::loadComponentToTable()
{  
    // 首先清空表格
    ui->tableWidget->setRowCount(0);
    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::FAN);
    for (const auto& component : componentList) {
        // 假设你有一个能够将组件转换为QStringList的函数
        if (auto fanComponent = dynamic_cast<Fan*>(component.data()))
        {
            auto lists = fanComponent->getComponentDataAsStringList(inComponentDB);
            //int rowCount = ui->tableWidget->rowCount();

            for (auto& list : lists) {
                addRowToTable(ui->tableWidget, list);
            }
        }
    }
    mergeColumnsByNames(ui->tableWidget, mergeCols, 2);
}
/**
 * @brief Widget_fan_inputTable::onAdd
 *
 * 连接pushButton_add, 点击按钮时, 添加一行数据到tablewidget中
 */
void Widget_fan_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget *tableWidget = ui->tableWidget;
        QSharedPointer<Fan> component;
        Dialog_fan *dialog = new Dialog_fan(nullptr);
        dialog->switchToCompontDB(inComponentDB);
        if (dialog->exec() == QDialog::Accepted) {

            if(Fan* rawPointer = static_cast<Fan*>(dialog->getComponent()))
                component = QSharedPointer<Fan>(rawPointer);
            else
                return;

            component->table_id = QString::number(tableWidget->rowCount() / 2 + 1);
            if (component != nullptr) {

                auto lists = component->getComponentDataAsStringList(inComponentDB);

                // 使用通用函数添加行
                addRowToTable(tableWidget, lists[0]);
                addRowToTable(tableWidget, lists[1]);

                componentManager.addComponent(component, inComponentDB);

                mergeColumnsByNames(ui->tableWidget, mergeCols, 2);
            }
        }
    }
    else
    {
        Widget_fan_inputTable* inputTable = new Widget_fan_inputTable(true, nullptr);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_fan_inputTable::handleConfirmation);
    }
}

/**
 * @brief Widget_fan_inputTable::onDel
 *
 * 从表格中删除一行选中的数据
 */
void Widget_fan_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 2);

    mergeColumnsByNames(ui->tableWidget, mergeCols, 2);
}

/**
 * @brief Widget_fan_inputTable::onRevise
 *
 * 修改表格中选中的行的数据
 */
void Widget_fan_inputTable::onRevise()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); row += 2) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            componentRevision<Fan, Dialog_fan>(ui->tableWidget, 2, row);
            mergeColumnsByNames(ui->tableWidget, mergeCols, 2);
        }
    }
}


void Widget_fan_inputTable::onInput()
{

}

void Widget_fan_inputTable::onOutput()
{

}

void Widget_fan_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto& uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);;
        if(QSharedPointer<Fan> component = qSharedPointerCast<Fan>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<Fan> newComponent = QSharedPointer<Fan>(new Fan(*component));
            if (newComponent != nullptr) {

                auto lists = newComponent->getComponentDataAsStringList(false);

                // 使用通用函数添加行
                addRowToTable(ui->tableWidget, lists[0]);
                addRowToTable(ui->tableWidget, lists[1]);

                componentManager.addComponent(newComponent, false);

                // 重新编号
                for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
                    QTableWidgetItem* item = new QTableWidgetItem(QString::number(2 == 1 ? (row + 1) : (row / 2 + 1)));
                    ui->tableWidget->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
                    item->setTextAlignment(Qt::AlignCenter);
                    item->setFlags(Qt::ItemIsEditable);
                    item->setBackground(QBrush(Qt::lightGray));
                    item->setData(Qt::ForegroundRole, QColor(70, 70, 70));
                }

                // 更新组件信息
                for (int row = 0; row < ui->tableWidget->rowCount(); row+=2) {
                    QString uuid = ui->tableWidget->item(row, ui->tableWidget->columnCount() - 1)->text(); // 获取组件uuid
                    QSharedPointer<ComponentBase> component = componentManager.findComponent(inComponentDB, uuid); // 查找组件

                    if (component) {
                        component->setTableID((QString::number((row + 2) / 2))); // 设置新的table_id，假设组件有这个方法
                        componentManager.updateComponent(uuid, component, inComponentDB); // 更新组件
                    }
                }

                mergeColumnsByNames(ui->tableWidget, mergeCols, 2);
            }
        }
    }
}
