#include "widget_diffuser_branch_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_diffuser_branch.h"

Widget_Diffuser_branch_inputTable::Widget_Diffuser_branch_inputTable(bool inComponentDB, QWidget* parent)
    :Widget_base_inputTable(inComponentDB, parent)
{
    setTitle("空调器风口分支");
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_Diffuser_branch_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_Diffuser_branch_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_Diffuser_branch_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_Diffuser_branch_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_Diffuser_branch_inputTable::onOutput);
}

void Widget_Diffuser_branch_inputTable::initTableWidget()
{
    colCount = 16;
    headerText<< "" << "序号" << "型号" << "品牌" << "总风量Q\n(m³/h)" << "支管风量Q1\n(m³/h)" << "63Hz\n(dB)"
               << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)"
               << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源" << "UUID";  //表头标题用QStringList来表示
    columnWidths = {30, 40, 120, 80, 70, 70, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_Diffuser_branch_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget *tableWidget = ui->tableWidget;
        Dialog_diffuser_branch *dialog = new Dialog_diffuser_branch(nullptr);
        QSharedPointer<Diffuser_branch> component;

        if (dialog->exec() == QDialog::Accepted) {
            if(Diffuser_branch* rawPointer = static_cast<Diffuser_branch*>(dialog->getComponent()))
                component = QSharedPointer<Diffuser_branch>(rawPointer);
            else
                return;
            component->table_id = QString::number(tableWidget->rowCount() + 1);
            if (component != nullptr) {
                auto lists = component->getComponentDataAsStringList(inComponentDB);

                // 使用通用函数添加行
                addRowToTable(tableWidget, lists[0]);

                componentManager.addComponent(component, inComponentDB);
            }
        }
    }
    else
    {
        Widget_Diffuser_branch_inputTable* inputTable = new Widget_Diffuser_branch_inputTable(true, nullptr);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_Diffuser_branch_inputTable::handleConfirmation);
    }
}

void Widget_Diffuser_branch_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 1);
}

void Widget_Diffuser_branch_inputTable::onRevise()
{
    QTableWidget* tableWidget = ui->tableWidget;

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        // 假设你的复选框在第一列
        QWidget* widget = tableWidget->cellWidget(row, 0);
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if(checkBox && checkBox->isChecked())
        {
            // 调用通用的修订函数，传入正确的类型参数
            componentRevision<Diffuser_branch, Dialog_diffuser_branch>(tableWidget, row);
        }
    }
}

void Widget_Diffuser_branch_inputTable::onInput()
{

}

void Widget_Diffuser_branch_inputTable::onOutput()
{

}

void Widget_Diffuser_branch_inputTable::onGenerateTemplate()
{

}

void Widget_Diffuser_branch_inputTable::loadComponentToTable()
{
    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::DIFFUSER_BRANCH);
    for (const auto& component : componentList) {
        if (auto diffuserBranchComponent = dynamic_cast<Diffuser_branch*>(component.data())) {
            auto lists = diffuserBranchComponent->getComponentDataAsStringList(inComponentDB);
            for (const auto& list : lists) {
                addRowToTable(ui->tableWidget, list);
            }
        }
    }
}

void Widget_Diffuser_branch_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto& uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);;
        if(QSharedPointer<Diffuser_branch> component = qSharedPointerCast<Diffuser_branch>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<Diffuser_branch> newComponent = QSharedPointer<Diffuser_branch>(new Diffuser_branch(*component));
            if (newComponent != nullptr) {

                auto lists = newComponent->getComponentDataAsStringList(false);

                // 使用通用函数添加行
                addRowToTable(ui->tableWidget, lists[0]);

                componentManager.addComponent(newComponent, false);

                // 重新编号
                for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
                    QTableWidgetItem* item = new QTableWidgetItem(QString::number(row + 1));
                    ui->tableWidget->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
                    item->setTextAlignment(Qt::AlignCenter);
                    item->setFlags(Qt::ItemIsEditable);
                    item->setBackground(QBrush(Qt::lightGray));
                    item->setData(Qt::ForegroundRole, QColor(70, 70, 70));
                }

                // 更新组件信息
                for (int row = 0; row < ui->tableWidget->rowCount(); row += 1) {
                    QString uuid = ui->tableWidget->item(row, ui->tableWidget->columnCount() - 1)->text(); // 获取组件uuid
                    QSharedPointer<ComponentBase> component = componentManager.findComponent(inComponentDB, uuid); // 查找组件

                    if (component) {
                        component->setTableID((QString::number(row + 1))); // 设置新的table_id，假设组件有这个方法
                        componentManager.updateComponent(uuid, component, inComponentDB); // 更新组件
                    }
                }
            }
        }
    }
}