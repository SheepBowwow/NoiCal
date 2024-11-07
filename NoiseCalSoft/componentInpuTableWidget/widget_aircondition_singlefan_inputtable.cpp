#include "widget_aircondition_singlefan_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_aircondition.h"

Widget_aircondition_singleFan_inputTable::Widget_aircondition_singleFan_inputTable(bool inComponentDB, QWidget *parent) :
    Widget_base_inputTable(inComponentDB, parent)
{
    setTitle("空调器(单风机)");
    mergeRowCount = 2;
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_aircondition_singleFan_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_aircondition_singleFan_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_aircondition_singleFan_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_aircondition_singleFan_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_aircondition_singleFan_inputTable::onOutput);
}

Widget_aircondition_singleFan_inputTable::~Widget_aircondition_singleFan_inputTable()
{
}

void Widget_aircondition_singleFan_inputTable::initTableWidget()
{
    if(!inComponentDB)
    {
        colCount = 20;
        headerText << "" << "序号" << "编号" << "型号" << "品牌" << "风量\n(m³/h)" << "静压\n(Pa)" << "类型" << "噪音\n位置"
                   << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)"
                   << "4kHz\n(dB)" << "8kHz\n(dB)" << "总值\ndB(A)" << "来源" << "UUID";
        // 设置每列的宽度
        QVector<int> widths = {30, 40, 120, 120, 80, 55, 55, 55, 60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};
        columnWidths = widths;
    }
    else
    {
        colCount = 19;
        headerText << "" << "序号" << "型号" << "品牌" << "风量\n(m³/h)" << "静压\n(Pa)" << "类型" << "噪音\n位置" << "63Hz\n(dB)"
                   << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
                   << "8kHz\n(dB)" << "总值\ndB(A)" << "来源" << "UUID";
        QVector<int> widths = {30, 40, 120, 80, 55, 55, 55, 60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};
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
        "类型",
        "来源",
        "UUID"
    };
    if(inComponentDB)   merge.removeOne("编号");
    this->mergeCols = merge;

    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_aircondition_singleFan_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget *tableWidget = ui->tableWidget;
        QSharedPointer<Aircondition> component;
        Dialog_aircondition *dialog = new Dialog_aircondition(nullptr, -1, 1);
        dialog->switchToCompontDB(inComponentDB);
        if (dialog->exec() == QDialog::Accepted) {

            if(Aircondition* rawPointer = static_cast<Aircondition*>(dialog->getComponent()))
                component = QSharedPointer<Aircondition>(rawPointer);
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
        Widget_aircondition_singleFan_inputTable* inputTable = new Widget_aircondition_singleFan_inputTable(true, nullptr);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_aircondition_singleFan_inputTable::handleConfirmation);
    }
}

void Widget_aircondition_singleFan_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 2);

    mergeColumnsByNames(ui->tableWidget, mergeCols, 2);
}

void Widget_aircondition_singleFan_inputTable::onRevise()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            QString uuid = ui->tableWidget->item(row, ui->tableWidget->columnCount() - 1)->text();
            QSharedPointer<Aircondition> component = componentManager.findComponent(inComponentDB, uuid).dynamicCast<Aircondition>();

            Dialog_aircondition *dialog = new Dialog_aircondition(this, row, 1, *component);
            if (dialog->exec() == QDialog::Accepted) {
                QSharedPointer<Aircondition> newComponent = QSharedPointer<Aircondition>(static_cast<Aircondition*>(dialog->getComponent()));

                if (newComponent && componentManager.updateComponent(uuid, newComponent, inComponentDB)) {
                    int insertPosition = row; // 记录要插入新行的位置

                    // 删除旧行
                    ui->tableWidget->removeRow(row + 1); // 先删除下一行
                    ui->tableWidget->removeRow(row); // 再删除当前行

                    auto lists = dialog->getComponentDataAsStringList();

                    // 现在在记录的位置插入行
                    addRowToTable(ui->tableWidget, lists[0], insertPosition);
                    addRowToTable(ui->tableWidget, lists[1], insertPosition + 1); // 注意这里是在紧接着上一行后插入
                }
                delete dialog; // Ensure dialog is deleted to avoid memory leak
            }
        }
    }
    mergeColumnsByNames(ui->tableWidget, mergeCols, 2);
}

void Widget_aircondition_singleFan_inputTable::onInput()
{

}

void Widget_aircondition_singleFan_inputTable::onOutput()
{

}

void Widget_aircondition_singleFan_inputTable::onGenerateTemplate()
{

}

void Widget_aircondition_singleFan_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);;
        if(QSharedPointer<Aircondition> component = qSharedPointerCast<Aircondition>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<Aircondition> newComponent = QSharedPointer<Aircondition>(new Aircondition(*component));
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

void Widget_aircondition_singleFan_inputTable::loadComponentToTable()
{
    // 首先清空表格
    ui->tableWidget->setRowCount(0);

    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::AIRCONDITION);
    for (const auto& component : componentList) {
        // 假设你有一个能够将组件转换为QStringList的函数
        if (auto airComponent = dynamic_cast<Aircondition*>(component.data()))
        {
            // 确保fancount为2
            if (airComponent->fan_counts == 1) {
                // 获取组件数据
                auto lists = airComponent->getComponentDataAsStringList(inComponentDB);
                for (const auto& list : lists) {
                    addRowToTable(ui->tableWidget, list);
                }
            }
        }
    }
    mergeColumnsByNames(ui->tableWidget, mergeCols, 2);
}
