#include "widget_vav_terminal_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_vav_terminal.h"
#include "office/excelengine.h"

Widget_VAV_terminal_inputTable::Widget_VAV_terminal_inputTable(bool inComponentDB, QWidget *parent) :
    Widget_base_inputTable(inComponentDB, parent)
{
    setTitle("变风量末端");
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_VAV_terminal_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_VAV_terminal_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_VAV_terminal_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_VAV_terminal_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_VAV_terminal_inputTable::onOutput);
}

Widget_VAV_terminal_inputTable::~Widget_VAV_terminal_inputTable()
{
}

void Widget_VAV_terminal_inputTable::initTableWidget()
{
    colCount = 17;
    headerText<< "" << "序号" << "型号"  << "品牌" << "阀门\n开度" << "风量\n(m³/h)" << "63Hz\n(dB)" << "125Hz\n(dB)"
              << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源" << "UUID";  //表头标题用QStringList来表示
    QVector<int> widths = {30, 40, 120, 80, 60, 60, 55, 55, 55, 55, 55, 55, 55, 55, 55 ,60, 0};
    // 调用封装好的初始化表格函数
    columnWidths = widths;


    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);

    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_VAV_terminal_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget *tableWidget = ui->tableWidget;
        QSharedPointer<VAV_terminal> component;
        Dialog_VAV_terminal *dialog = new Dialog_VAV_terminal(nullptr);
        if (dialog->exec() == QDialog::Accepted) {

            if(VAV_terminal* rawPointer = static_cast<VAV_terminal*>(dialog->getComponent()))
                component = QSharedPointer<VAV_terminal>(rawPointer);
            else
                return;
            addComponent(component);
        }
    }
    else
    {
        Widget_VAV_terminal_inputTable* inputTable = new Widget_VAV_terminal_inputTable(true, nullptr);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_VAV_terminal_inputTable::handleConfirmation);
    }
}

void Widget_VAV_terminal_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 1);
}

void Widget_VAV_terminal_inputTable::onRevise()
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
            componentRevision<VAV_terminal, Dialog_VAV_terminal>(tableWidget, row);
        }
    }
}

void Widget_VAV_terminal_inputTable::onInput()
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
        QString data_source = parsedData[14];
        QString air_volume = parsedData[4];
        QString angle = parsedData[3];

        array<QString, 9> noi = {""};

        for(int i = 0; i < 9; i++) {
            noi[i] = parsedData[i][i + 5];
        }

        VAV_terminal* componentRaw = new VAV_terminal(model, brand, table_id, UUID, data_source, angle,
                                                    air_volume, noi);

        QSharedPointer<VAV_terminal> component = QSharedPointer<VAV_terminal>(componentRaw);
        addComponent(component);
    }
}

void Widget_VAV_terminal_inputTable::onOutput()
{
    ExcelEngine* excelEngine = new ExcelEngine(this);
    excelEngine->deriveExecl(ui->tableWidget, "变风量末端");
}

void Widget_VAV_terminal_inputTable::onGenerateTemplate()
{
    getTemplate(":/componentImportTemplate/componentImportTemplate/vav_terminal.xlsx",
                "变风量末端导入模板");
}

void Widget_VAV_terminal_inputTable::loadComponentToTable()
{
    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::VAV_TERMINAL);
    for (const auto& component : componentList) {
        if (auto vavComponent = dynamic_cast<VAV_terminal*>(component.data())) {
            auto lists = vavComponent->getComponentDataAsStringList(inComponentDB);
            for (const auto& list : lists) {
                addRowToTable(ui->tableWidget, list);
            }
        }
    }
}

void Widget_VAV_terminal_inputTable::addComponent(QSharedPointer<VAV_terminal> &component)
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

void Widget_VAV_terminal_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto& uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);;
        if(QSharedPointer<VAV_terminal> component = qSharedPointerCast<VAV_terminal>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<VAV_terminal> newComponent = QSharedPointer<VAV_terminal>(new VAV_terminal(*component));
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
