#include "widget_aircondition_doublefan_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_aircondition.h"
#include "office/excelengine.h"
#include <QFileDialog>
#include <QResource>

Widget_aircondition_doubleFan_inputTable::Widget_aircondition_doubleFan_inputTable(bool inComponentDB, QWidget *parent) :
    Widget_base_inputTable(inComponentDB, parent)
{
    setTitle("空调器(双风机)");
    mergeRowCount = 4;
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_aircondition_doubleFan_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_aircondition_doubleFan_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_aircondition_doubleFan_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_aircondition_doubleFan_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_aircondition_doubleFan_inputTable::onOutput);
}

Widget_aircondition_doubleFan_inputTable::~Widget_aircondition_doubleFan_inputTable()
{
}

void Widget_aircondition_doubleFan_inputTable::mergeCells(int startRow)
{
    QTableWidget *tableWidget = ui->tableWidget;
    for(int i = 0; i < tableWidget->columnCount(); i++)
    {
        if(inComponentDB) {
            if(i == 4 || i == 5 || i == 6)
            {
                tableWidget->setSpan(startRow, i, 2, 1);
                tableWidget->setSpan(startRow + 2, i, 2, 1);
            }
            else if(i < 4 || i > 16)
            {
                tableWidget->setSpan(startRow, i, 4, 1);
            }
        } else {
            if(i == 2 || i == 5 || i == 6 || i == 7)
            {
                tableWidget->setSpan(startRow, i, 2, 1);
                tableWidget->setSpan(startRow + 2, i, 2, 1);
            }
            else if(i < 5 || i > 17)
            {
                tableWidget->setSpan(startRow, i, 4, 1);
            }
        }
    }

    for(int i = startRow; i < tableWidget->rowCount(); i++)
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

void Widget_aircondition_doubleFan_inputTable::addComponent(QSharedPointer<Aircondition> &component)
{
    component->table_id = QString::number(ui->tableWidget->rowCount() / 4 + 1);
    if (component != nullptr) {
        auto lists = component->getComponentDataAsStringList(inComponentDB);

        // 使用通用函数添加行
        addRowToTable(ui->tableWidget, lists[0]);
        addRowToTable(ui->tableWidget, lists[1]);
        addRowToTable(ui->tableWidget, lists[2]);
        addRowToTable(ui->tableWidget, lists[3]);
        componentManager.addComponent(component, inComponentDB);

        mergeCells(ui->tableWidget->rowCount() - 4);
    }
}

void Widget_aircondition_doubleFan_inputTable::loadComponentToTable()
{
    // 首先清空表格
    ui->tableWidget->setRowCount(0);

    // 获取所有Aircondition组件
    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::AIRCONDITION);
    for (const auto& component : componentList) {
        // 使用dynamic_cast尝试将ComponentBase转换为Aircondition指针
        if (auto airconditionComponent = dynamic_cast<Aircondition*>(component.data())) {
            // 确保fancount为2
            if (airconditionComponent->fan_counts == 2) {
                // 获取组件数据
                auto lists = airconditionComponent->getComponentDataAsStringList(inComponentDB);

                // 添加数据到表格
                int rowCount = ui->tableWidget->rowCount();
                for (const auto& list : lists) {
                    addRowToTable(ui->tableWidget, list);
                }

                // 合并单元格操作（根据你的逻辑合并）
                mergeCells(rowCount);
            }
        }
    }
}

void Widget_aircondition_doubleFan_inputTable::initTableWidget()
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

    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_aircondition_doubleFan_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget *tableWidget = ui->tableWidget;
        Dialog_aircondition *dialog = new Dialog_aircondition(nullptr, -1, 2);
        QSharedPointer<Aircondition> component;

        int rowCount = tableWidget->rowCount(); //获取当前行数
        dialog->switchToCompontDB(inComponentDB);
        if (dialog->exec() == QDialog::Accepted) {
            if(Aircondition* rawPointer = static_cast<Aircondition*>(dialog->getComponent()))
                component = QSharedPointer<Aircondition>(rawPointer);
            else
                return;
            addComponent(component);
        }
    }
    else
    {
        Widget_aircondition_doubleFan_inputTable* inputTable = new Widget_aircondition_doubleFan_inputTable(true, nullptr);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_aircondition_doubleFan_inputTable::handleConfirmation);
    }

}

void Widget_aircondition_doubleFan_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 4);

    for(int i = 0; i < ui->tableWidget->rowCount(); i += 4)
    {
        mergeCells(i);
    }
}

void Widget_aircondition_doubleFan_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto& uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);;
        if(QSharedPointer<Aircondition> component = qSharedPointerCast<Aircondition>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<Aircondition> newComponent = QSharedPointer<Aircondition>(new Aircondition(*component));
            if (newComponent != nullptr) {

                if(newComponent->send_number.isEmpty() && newComponent->exhaust_number.isEmpty()) {
                    newComponent->send_number = "-";
                    newComponent->exhaust_number = "-";
                }

                auto lists = newComponent->getComponentDataAsStringList(false);

                // 使用通用函数添加行
                addRowToTable(ui->tableWidget, lists[0]);
                addRowToTable(ui->tableWidget, lists[1]);
                addRowToTable(ui->tableWidget, lists[2]);
                addRowToTable(ui->tableWidget, lists[3]);

                componentManager.addComponent(newComponent, false);

                // 重新编号
                for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
                    QTableWidgetItem* item = new QTableWidgetItem(QString::number(row / 4 + 1));
                    ui->tableWidget->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
                    item->setTextAlignment(Qt::AlignCenter);
                    item->setFlags(Qt::ItemIsEditable);
                    item->setBackground(QBrush(Qt::lightGray));
                    item->setData(Qt::ForegroundRole, QColor(70, 70, 70));
                }

                // 更新组件信息
                for (int row = 0; row < ui->tableWidget->rowCount(); row+=4) {
                    QString uuid = ui->tableWidget->item(row, ui->tableWidget->columnCount() - 1)->text(); // 获取组件uuid
                    QSharedPointer<ComponentBase> component = componentManager.findComponent(inComponentDB, uuid); // 查找组件

                    if (component) {
                        component->setTableID((QString::number((row + 4) / 4))); // 设置新的table_id，假设组件有这个方法
                        componentManager.updateComponent(uuid, component, inComponentDB); // 更新组件
                    }
                }

                for(int i = 0; i < ui->tableWidget->rowCount(); i += 4)
                {
                    mergeCells(i);
                }
            }
        }
    }
}

void Widget_aircondition_doubleFan_inputTable::onRevise()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            QString uuid = ui->tableWidget->item(row, ui->tableWidget->columnCount() - 1)->text();
            QSharedPointer<Aircondition> component = componentManager.findComponent(inComponentDB, uuid).dynamicCast<Aircondition>();

            Dialog_aircondition *dialog = new Dialog_aircondition(this, row, 2, *component);
            if (dialog->exec() == QDialog::Accepted) {
                QSharedPointer<Aircondition> newComponent = QSharedPointer<Aircondition>(static_cast<Aircondition*>(dialog->getComponent()));

                if (newComponent && componentManager.updateComponent(uuid, newComponent, inComponentDB)) {
                    int insertPosition = row; // 记录要插入新行的位置

                    // 删除旧行
                    ui->tableWidget->removeRow(row + 3); // 先删除下一行
                    ui->tableWidget->removeRow(row + 2); // 先删除下一行
                    ui->tableWidget->removeRow(row + 1); // 先删除下一行
                    ui->tableWidget->removeRow(row); // 再删除当前行

                    auto lists = dialog->getComponentDataAsStringList();

                    // 现在在记录的位置插入行
                    addRowToTable(ui->tableWidget, lists[0], insertPosition);
                    addRowToTable(ui->tableWidget, lists[1], insertPosition + 1); // 注意这里是在紧接着上一行后插入
                    addRowToTable(ui->tableWidget, lists[2], insertPosition + 2); // 注意这里是在紧接着上一行后插入
                    addRowToTable(ui->tableWidget, lists[3], insertPosition + 3); // 注意这里是在紧接着上一行后插入
                    mergeCells(row);
                }
                delete dialog; // Ensure dialog is deleted to avoid memory leak
            }
        }
    }
}

void Widget_aircondition_doubleFan_inputTable::onInput()
{
    if(!inComponentDB)
        return;
    QStringList dataList;
    ExcelEngine* excelEngine = new ExcelEngine(this);
    excelEngine->importData(dataList);

    for(auto& data: dataList) {
        qDebug() << data;
    }
    for(int rowGroup = 0; rowGroup < dataList.size(); rowGroup += 4) {
        QList<QStringList> parsedData; // 用于存储每一行的分割结果
        for(int row = rowGroup; row < rowGroup + 4; row++) {
            QStringList fields = dataList[row].split(","); // 按逗号分割每一行
            parsedData.append(fields);
        }
        QString model = parsedData[0][1];
        QString brand = parsedData[0][2];
        QString table_id = "-1";
        QString UUID = "";
        QString data_source = parsedData[0][16];
        int fan_counts = 2;
        QString send_number = "-";
        QString send_air_volume = parsedData[0][3];
        QString send_static_pressure = parsedData[0][4];
        QString exhaust_number = "-";
        QString exhaust_air_volume = parsedData[2][3];
        QString exhaust_static_pressure = parsedData[2][4];

        array<QString, 9> noi_send_in = {""};
        array<QString, 9> noi_send_out = {""};
        array<QString, 9> noi_exhaust_in = {""};
        array<QString, 9> noi_exhaust_out = {""};

        QVector<array<QString, 9>*> arrayList = {&noi_send_in, &noi_send_out, &noi_exhaust_in, &noi_exhaust_out};
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 9; j++) {
                (*arrayList[i])[j] = parsedData[i][j + 7];
            }
        }

        Aircondition* componentRaw = new Aircondition(model, brand, table_id, UUID, data_source, fan_counts, send_number,
                                                   send_air_volume, send_static_pressure, exhaust_number, exhaust_air_volume,
                                                   exhaust_static_pressure, noi_send_in,
                                                   noi_send_out, noi_exhaust_in, noi_exhaust_out);

        QSharedPointer<Aircondition> component = QSharedPointer<Aircondition>(componentRaw);
        addComponent(component);
    }

}

void Widget_aircondition_doubleFan_inputTable::onOutput()
{
    ExcelEngine* excelEngine = new ExcelEngine(this);
    excelEngine->deriveExecl(ui->tableWidget, "空调器(双风机)");
}

void Widget_aircondition_doubleFan_inputTable::onGenerateTemplate()
{
    // 打开文件对话框获取保存路径
    QString savePath = QFileDialog::getSaveFileName(this, "保存文件", "空调器(双风机)导入模板", "Excel文件 (*.xlsx)");

    if (!savePath.isEmpty()) {
        // 读取qrc中的文件内容
        QResource resource(":/componentImportTemplate/componentImportTemplate/aircondition_doublefan.xlsx");
        if (resource.isValid()) {
            QFile file(savePath);
            if (file.open(QIODevice::WriteOnly)) {
                // 将QResource的数据转换为QByteArray
                QByteArray data(reinterpret_cast<const char*>(resource.data()), resource.size());
                file.write(data);  // 写入数据
                file.close();
            }
        }
    }
}

