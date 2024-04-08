#include "widget_fan_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "Component/ComponentStructs.h"
#include "globle_var.h"
#include "inputDialog/dialog_fan.h"

Widget_fan_inputTable::Widget_fan_inputTable(QWidget *parent) :
    Widget_base_inputTable(parent)
{
    setTitle("风机");
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
    int colCount = 19;
    // 设置表头标题
    QStringList headerText;
    headerText << "" << "序号" << "编号" << "型号" << "品牌" << "风量\n(m³/h)" << "静压\n(Pa)" << "噪音\n位置" << "63Hz\n(dB)"
               << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
               << "8kHz\n(dB)" << "总值\ndB(A)" << "来源" << "UUID";
    // 设置每列的宽度
    int columnWidths[] = {30, 38, 120, 120, 60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};
    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);

    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_fan_inputTable::mergeCells(int startRow)
{
    QTableWidget *tableWidget = ui->tableWidget;
    for(int i = 0; i < tableWidget->columnCount(); i++)
    {
        if(i < 7 || i > 16)
        {
            tableWidget->setSpan(startRow, i, 2, 1);
        }
    }

    for(int i = startRow; i < startRow + 2; i++)
    {
        QWidget* widget = tableWidget->cellWidget(i, 0);
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
    tableWidget->setCellWidget(startRow, 0, widget);
}

void Widget_fan_inputTable::loadComponentToTable()
{
    auto componentList = ComponentManager::getInstance().getComponentsByType(component_type_name::FAN);
    for (const auto& component : componentList) {
        // 假设你有一个能够将组件转换为QStringList的函数
        if (auto fanComponent = dynamic_cast<Fan*>(component.data()))
        {
            auto lists = fanComponent->getComponentDataAsStringList();
            int rowCount = ui->tableWidget->rowCount();

            for (const auto& list : lists) {
                addRowToTable(ui->tableWidget, list);
            }

            mergeCells(rowCount);
        }
    }
}

/**
 * @brief Widget_fan_inputTable::onAdd
 *
 * 连接pushButton_add, 点击按钮时, 添加一行数据到tablewidget中
 */
void Widget_fan_inputTable::onAdd()
{
    QTableWidget *tableWidget = ui->tableWidget;
    int rowCount = tableWidget->rowCount(); //获取当前行数
    QSharedPointer<Fan> component;
    Dialog_fan *dialog = new Dialog_fan(nullptr);

    if (dialog->exec() == QDialog::Accepted) {

        if(Fan* rawPointer = static_cast<Fan*>(dialog->getComponent()))
            component = QSharedPointer<Fan>(rawPointer);
        else
            return;

        component->table_id = QString::number(tableWidget->rowCount() / 2 + 1);
        if (component != nullptr) {

            auto lists = component->getComponentDataAsStringList();

            // 使用通用函数添加行
            addRowToTable(tableWidget, lists[0]);
            addRowToTable(tableWidget, lists[1]);

            componentManager.addComponent(component);

            mergeCells(rowCount);
        }
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


    for(int i = 0; i < ui->tableWidget->rowCount(); i += 2)
    {
        mergeCells(i);
    }
}

/**
 * @brief Widget_fan_inputTable::onRevise
 *
 * 修改表格中选中的行的数据
 */
void Widget_fan_inputTable::onRevise()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); row+=2) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            QString uuid = ui->tableWidget->item(row, ui->tableWidget->columnCount() - 1)->text();
            QSharedPointer<Fan> component = componentManager.findComponent(uuid).dynamicCast<Fan>();
            if(!component)
                continue;
            Dialog_fan *dialog = new Dialog_fan(this, row, *component);
            if (dialog->exec() == QDialog::Accepted) {
                QSharedPointer<Fan> newComponent = QSharedPointer<Fan>(static_cast<Fan*>(dialog->getComponent()));

                if (newComponent && componentManager.updateComponent(uuid, newComponent)) {
                    int insertPosition = row; // 记录要插入新行的位置

                    // 删除旧行
                    ui->tableWidget->removeRow(row + 1); // 先删除下一行
                    ui->tableWidget->removeRow(row); // 再删除当前行

                    auto lists = dialog->getComponentDataAsStringList();

                    // 现在在记录的位置插入行
                    addRowToTable(ui->tableWidget, lists[0], insertPosition);
                    addRowToTable(ui->tableWidget, lists[1], insertPosition + 1); // 注意这里是在紧接着上一行后插入
                    mergeCells(row);
                }
                delete dialog; // Ensure dialog is deleted to avoid memory leak
            }
        }
    }
}


void Widget_fan_inputTable::onInput()
{

}

void Widget_fan_inputTable::onOutput()
{

}
