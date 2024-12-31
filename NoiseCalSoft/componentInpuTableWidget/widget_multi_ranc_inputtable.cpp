#include "widget_multi_ranc_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_duct_with_multi_ranc.h"
#include "office/excelengine.h"

Widget_Multi_ranc_inputTable::Widget_Multi_ranc_inputTable(bool inComponentDB, QWidget* parent)
    :Widget_base_inputTable(inComponentDB, parent)
{
    setTitle("风道多分支");
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_Multi_ranc_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_Multi_ranc_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_Multi_ranc_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_Multi_ranc_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_Multi_ranc_inputTable::onOutput);
}

void Widget_Multi_ranc_inputTable::initTableWidget()
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

void Widget_Multi_ranc_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget *tableWidget = ui->tableWidget;
        Dialog_duct_with_multi_ranc *dialog = new Dialog_duct_with_multi_ranc(nullptr);
        QSharedPointer<Multi_ranc> component;

        if (dialog->exec() == QDialog::Accepted) {
            if(Multi_ranc* rawPointer = static_cast<Multi_ranc*>(dialog->getComponent()))
                component = QSharedPointer<Multi_ranc>(rawPointer);
            else
                return;
            addComponent(component);
        }
    }
    else
    {
        Widget_Multi_ranc_inputTable* inputTable = new Widget_Multi_ranc_inputTable(true, nullptr);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_Multi_ranc_inputTable::handleConfirmation);
    }
}

void Widget_Multi_ranc_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 1);
}

void Widget_Multi_ranc_inputTable::onRevise()
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
            componentRevision<Multi_ranc, Dialog_duct_with_multi_ranc>(tableWidget, row);
        }
    }
}

void Widget_Multi_ranc_inputTable::onInput()
{
    if(!inComponentDB)
        return;
    QStringList dataList;
    ExcelEngine* excelEngine = new ExcelEngine(this);
    excelEngine->importData(dataList);

    for(auto& data: dataList) {
        qDebug() << data;
    }
    for(int row = 0; row < dataList.size(); row++) {
        QStringList parsedData = dataList[row].split(","); // 用于存储每一行的分割结果

        QString model = parsedData[1];
        QString brand = parsedData[2];
        QString table_id = "-1";
        QString UUID = "";
        QString data_source = parsedData[13];
        QString q = parsedData[3];
        QString q1 = parsedData[4];

        array<QString, 8> atten = {""};

        for(int i = 0; i < 8; i++) {
            atten[i] = parsedData[i][i + 5];
        }

        Multi_ranc* componentRaw = new Multi_ranc(model, brand, table_id, UUID, data_source, q1, q,
                                                            atten);

        QSharedPointer<Multi_ranc> component = QSharedPointer<Multi_ranc>(componentRaw);
        addComponent(component);
    }
}

void Widget_Multi_ranc_inputTable::onOutput()
{
    ExcelEngine* excelEngine = new ExcelEngine(this);
    excelEngine->deriveExecl(ui->tableWidget, "风道多分支");
}

void Widget_Multi_ranc_inputTable::onGenerateTemplate()
{
    getTemplate(":/componentImportTemplate/componentImportTemplate/multi_ranc.xlsx",
                "风道多分支导入模板");
}

void Widget_Multi_ranc_inputTable::loadComponentToTable()
{
    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::MULTI_RANC);
    for (const auto& component : componentList) {
        if (auto multiRancComponent = dynamic_cast<Multi_ranc*>(component.data())) {
            auto lists = multiRancComponent->getComponentDataAsStringList(inComponentDB);
            for (const auto& list : lists) {
                addRowToTable(ui->tableWidget, list);
            }
        }
    }
}

void Widget_Multi_ranc_inputTable::addComponent(QSharedPointer<Multi_ranc> &component)
{
    QTableWidget* tableWidget = ui->tableWidget;
    component->table_id = QString::number(tableWidget->rowCount() + 1);
    if (component != nullptr) {
        auto lists = component->getComponentDataAsStringList(inComponentDB);

        // 使用通用函数添加行
        addRowToTable(tableWidget, lists[0]);

        componentManager.addComponent(component, inComponentDB);
    }
}

void Widget_Multi_ranc_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto& uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);;
        if(QSharedPointer<Multi_ranc> component = qSharedPointerCast<Multi_ranc>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<Multi_ranc> newComponent = QSharedPointer<Multi_ranc>(new Multi_ranc(*component));
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
