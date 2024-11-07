#include "widget_circular_damper_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/Dialog_circular_damper.h"

Widget_Circular_damper_inputTable::Widget_Circular_damper_inputTable(bool inComponentDB, QWidget *parent) :
    Widget_base_inputTable(inComponentDB, parent)
{    
    setTitle("圆形调风门");
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_Circular_damper_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_Circular_damper_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_Circular_damper_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_Circular_damper_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_Circular_damper_inputTable::onOutput);
}

Widget_Circular_damper_inputTable::~Widget_Circular_damper_inputTable()
{
}

void Widget_Circular_damper_inputTable::initTableWidget()
{
    colCount = 18;
    headerText<< "" << "序号" << "型号" << "品牌" << "直径\n(mm)" << "风量\n(m³/h)" << "阀门\n开度" << "63Hz\n(dB)" << "125Hz\n(dB)"
              << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源" << "UUID";  //表头标题用QStringList来表示
    QVector<int> widths = {30, 40, 120, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};
    columnWidths = widths;
    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_Circular_damper_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget *tableWidget = ui->tableWidget;
        Dialog_circular_damper *dialog = new Dialog_circular_damper(nullptr);
        QSharedPointer<Circular_damper> component;

        if (dialog->exec() == QDialog::Accepted) {
            if(Circular_damper* rawPointer = static_cast<Circular_damper*>(dialog->getComponent()))
                component = QSharedPointer<Circular_damper>(rawPointer);
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
        Widget_Circular_damper_inputTable* inputTable = new Widget_Circular_damper_inputTable(true, nullptr);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_Circular_damper_inputTable::handleConfirmation);
    }
}

void Widget_Circular_damper_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 1);
}

/**
 * @brief Widget_Circular_damper_inputTable::onRevise
 * 1、通过行拿到uuid
 * 2、根据uuid查询到部件
 * 3、修改部件
 */
void Widget_Circular_damper_inputTable::onRevise()
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
            componentRevision<Circular_damper, Dialog_circular_damper>(tableWidget,row);
        }
    }
}

void Widget_Circular_damper_inputTable::onInput()
{

}

void Widget_Circular_damper_inputTable::onOutput()
{

}

void Widget_Circular_damper_inputTable::onGenerateTemplate()
{

}

void Widget_Circular_damper_inputTable::loadComponentToTable()
{
    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::CIRCULAR_DAMPER);
    for (const auto& component : componentList) {
        if (auto damperComponent = dynamic_cast<Circular_damper*>(component.data())) {
            auto lists = damperComponent->getComponentDataAsStringList();
            for (const auto& list : lists) {
                addRowToTable(ui->tableWidget, list);
            }
        }
    }
}

void Widget_Circular_damper_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto& uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);;
        if(QSharedPointer<Circular_damper> component = qSharedPointerCast<Circular_damper>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<Circular_damper> newComponent = QSharedPointer<Circular_damper>(new Circular_damper(*component));
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
