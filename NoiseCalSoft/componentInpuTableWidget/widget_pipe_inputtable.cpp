#include "widget_pipe_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_pipe.h"
#include "office/excelengine.h"

Widget_Pipe_inputTable::Widget_Pipe_inputTable(bool inComponentDB, QWidget* parent)
    :Widget_base_inputTable(inComponentDB, parent)
{
    setTitle("直管");
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_Pipe_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_Pipe_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_Pipe_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_Pipe_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_Pipe_inputTable::onOutput);
}

void Widget_Pipe_inputTable::initTableWidget()
{
    colCount = 16;

    headerText<< "" << "序号" << "型号" << "品牌" << "类型" << "尺寸\n(mm)" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源" << "UUID";  //表头标题用QStringList来表示
    columnWidths = {30, 40, 120, 70, 55, 70, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};

    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_Pipe_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget *tableWidget = ui->tableWidget;
        Dialog_pipe *dialog = new Dialog_pipe(nullptr);
        QSharedPointer<Pipe> component;

        if (dialog->exec() == QDialog::Accepted) {
            if(Pipe* rawPointer = static_cast<Pipe*>(dialog->getComponent()))
                component = QSharedPointer<Pipe>(rawPointer);
            else
                return;
            addComponent(component);
        }
    }
    else
    {
        Widget_Pipe_inputTable* inputTable = new Widget_Pipe_inputTable(true, nullptr);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_Pipe_inputTable::handleConfirmation);
    }
}

void Widget_Pipe_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 1);
}

void Widget_Pipe_inputTable::onRevise()
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
            componentRevision<Pipe, Dialog_pipe>(tableWidget, row);
        }
    }
}

void Widget_Pipe_inputTable::onInput()
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
        QString pipe_shape = parsedData[3];
        QString size = parsedData[4];

        array<QString, 8> atten = {""};

        for(int i = 0; i < 8; i++) {
            atten[i] = parsedData[i][i + 5];
        }

        Pipe* componentRaw = new Pipe(model, brand, table_id, UUID, data_source, pipe_shape, size, atten);

        QSharedPointer<Pipe> component = QSharedPointer<Pipe>(componentRaw);
        addComponent(component);
    }
}

void Widget_Pipe_inputTable::onOutput()
{
    ExcelEngine* excelEngine = new ExcelEngine(this);
    excelEngine->deriveExecl(ui->tableWidget, "直管");
}

void Widget_Pipe_inputTable::onGenerateTemplate()
{
    getTemplate(":/componentImportTemplate/componentImportTemplate/pipe.xlsx",
                "直管导入模板");
}

void Widget_Pipe_inputTable::loadComponentToTable()
{
    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::PIPE);
    for (const auto& component : componentList) {
        if (auto pipeComponent = dynamic_cast<Pipe*>(component.data())) {
            auto lists = pipeComponent->getComponentDataAsStringList(inComponentDB);
            for (const auto& list : lists) {
                addRowToTable(ui->tableWidget, list);
            }
        }
    }
}

void Widget_Pipe_inputTable::addComponent(QSharedPointer<Pipe> &component)
{
    QTableWidget* tableWidget = ui->tableWidget;
    component->table_id = QString::number(tableWidget->rowCount() + 1);
    if (component != nullptr) {
        auto lists = component->getComponentDataAsStringList();

        // 使用通用函数添加行
        addRowToTable(tableWidget, lists[0]);

        componentManager.addComponent(component, inComponentDB);
    }
}

void Widget_Pipe_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto& uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);;
        if(QSharedPointer<Pipe> component = qSharedPointerCast<Pipe>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<Pipe> newComponent = QSharedPointer<Pipe>(new Pipe(*component));
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
