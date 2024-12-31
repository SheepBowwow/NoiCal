#include "widget_fancoil_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_fancoil.h"
#include "office/excelengine.h"

Widget_fanCoil_inputTable::Widget_fanCoil_inputTable(bool inComponentDB, QWidget *parent) :
    Widget_base_inputTable(inComponentDB, parent)
{
    setTitle("风机盘管");
    mergeRowCount = 2;
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_fanCoil_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_fanCoil_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_fanCoil_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_fanCoil_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_fanCoil_inputTable::onOutput);
}

Widget_fanCoil_inputTable::~Widget_fanCoil_inputTable()
{
}

void Widget_fanCoil_inputTable::loadComponentToTable()
{
    // 首先清空表格
    ui->tableWidget->setRowCount(0);
    // 从组件管理器获取所有的风机盘管组件
    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::FANCOIL); // 请替换FanCoil的类型名称为实际的类型标识符
    for (const auto& component : componentList) {
        // 动态转换以确保组件类型正确
        if(auto fanCoilComponent = dynamic_cast<FanCoil*>(component.data()))
        {
            auto lists = fanCoilComponent->getComponentDataAsStringList(inComponentDB);

            for (const auto& list : lists) {
                addRowToTable(ui->tableWidget, list);
            }
        }
    }
    // 根据需要合并单元格
    mergeColumnsByNames(ui->tableWidget, mergeCols, 2);
}

void Widget_fanCoil_inputTable::initTableWidget()
{
    colCount = 19;
    headerText << "" << "序号" << "类型" << "型号" << "品牌" << "风量\n(m³/h)" << "静压\n(Pa)" << "噪音\n位置" << "63Hz\n(dB)"
               << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
               << "8kHz\n(dB)" << "总值\ndB(A)" << "来源" << "UUID";
    // 设置每列的宽度
    QVector<int> widths = {30, 38, 80, 120, 60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};
    columnWidths = widths;

    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);

    QStringList merge
    {
        "",
        "序号",
        "类型",
        "型号",
        "品牌",
        "风量\n(m³/h)",
        "静压\n(Pa)",
        "来源",
        "UUID"
    };
    this->mergeCols = merge;

    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_fanCoil_inputTable::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); // Call the base class implementation

    if (!ui->tableWidget)
        return;

    double totalWidth = 0;
    for (int i = 0; i < colCount; ++i)
    {
        totalWidth += columnWidths[i];
    }
    for (int i = 0; i < colCount; ++i)
    {
        int columnWidth = static_cast<int>(static_cast<double>(columnWidths[i]) * ui->tableWidget->width() / totalWidth);
        qDebug() << columnWidths[i];
        ui->tableWidget->setColumnWidth(i, columnWidth);
    }
    qDebug() << ui->tableWidget->width();
}

void Widget_fanCoil_inputTable::addComponent(QSharedPointer<FanCoil> &component)
{
    QTableWidget* tableWidget = ui->tableWidget;
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

void Widget_fanCoil_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget *tableWidget = ui->tableWidget;
        QSharedPointer<FanCoil> component;
        Dialog_fanCoil *dialog = new Dialog_fanCoil(nullptr);

        if (dialog->exec() == QDialog::Accepted) {

            if(FanCoil* rawPointer = static_cast<FanCoil*>(dialog->getComponent()))
                component = QSharedPointer<FanCoil>(rawPointer);
            else
                return;
            addComponent(component);
        }
    }
    else
    {
        Widget_fanCoil_inputTable* inputTable = new Widget_fanCoil_inputTable(true, nullptr);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_fanCoil_inputTable::handleConfirmation);
    }
}

void Widget_fanCoil_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 2);

    mergeColumnsByNames(ui->tableWidget, mergeCols, 2);
}

void Widget_fanCoil_inputTable::onRevise()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); row += 2) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            componentRevision<FanCoil, Dialog_fanCoil>(ui->tableWidget, 2, row);
            mergeColumnsByNames(ui->tableWidget, mergeCols, 2);
        }
    }
}

void Widget_fanCoil_inputTable::onInput()
{
    if(!inComponentDB)
        return;
    QStringList dataList;
    ExcelEngine* excelEngine = new ExcelEngine(this);
    excelEngine->importData(dataList);

    for(auto& data: dataList) {
        qDebug() << data;
    }
    for(int rowGroup = 0; rowGroup < dataList.size(); rowGroup += 2) {
        QList<QStringList> parsedData; // 用于存储每一行的分割结果
        for(int row = rowGroup; row < rowGroup + 2; row++) {
            QStringList fields = dataList[row].split(","); // 按逗号分割每一行
            parsedData.append(fields);
        }
        QString model = parsedData[0][2];
        QString brand = parsedData[0][3];
        QString table_id = "-1";
        QString UUID = "";
        QString data_source = parsedData[0][16];
        QString air_volume = parsedData[0][4];
        QString static_pressure = parsedData[0][5];
        QString type = parsedData[0][1];
        array<QString, 9> noi_in = {""};
        array<QString, 9> noi_out = {""};

        QVector<array<QString, 9>*> arrayList = {&noi_in, &noi_out};
        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < 9; j++) {
                (*arrayList[i])[j] = parsedData[i][j + 7];
            }
        }

        FanCoil* componentRaw = new FanCoil(model, brand, table_id, UUID, data_source, air_volume,
                                    static_pressure, type, noi_in, noi_out);

        QSharedPointer<FanCoil> component = QSharedPointer<FanCoil>(componentRaw);
        addComponent(component);
    }
}

void Widget_fanCoil_inputTable::onOutput()
{
    ExcelEngine* excelEngine = new ExcelEngine(this);
    excelEngine->deriveExecl(ui->tableWidget, "风机盘管");
}

void Widget_fanCoil_inputTable::onGenerateTemplate()
{
    getTemplate(":/componentImportTemplate/componentImportTemplate/fanCoil.xlsx",
                "风机盘管导入模板");
}

void Widget_fanCoil_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);
        if(QSharedPointer<FanCoil> component = qSharedPointerCast<FanCoil>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<FanCoil> newComponent = QSharedPointer<FanCoil>(new FanCoil(*component));
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
