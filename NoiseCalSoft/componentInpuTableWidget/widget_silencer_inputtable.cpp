#include "widget_silencer_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_silencer.h"
#include <QQueue>
#include "office/excelengine.h"

Widget_Silencer_inputTable::Widget_Silencer_inputTable(bool inComponentDB, const QString& type, QWidget *parent) :
    Widget_base_inputTable(inComponentDB, parent),
    silencer_type(type)
{
    setTitle(type);
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_Silencer_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_Silencer_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_Silencer_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_Silencer_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_Silencer_inputTable::onOutput);
}

Widget_Silencer_inputTable::~Widget_Silencer_inputTable()
{
}

void Widget_Silencer_inputTable::initTableWidget()
{
    colCount = 14;
    headerText<< "" << "序号" << "型号" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                    << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                    << "来源" << "UUID";  //表头标题用QStringList来表示
    columnWidths = {30, 40, 180, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};

    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);

    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_Silencer_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget* tableWidget = ui->tableWidget;
        Dialog_silencer *dialog = new Dialog_silencer(silencer_type, nullptr);
        QSharedPointer<Silencer> component;

        if (dialog->exec() == QDialog::Accepted) {
            if(Silencer* rawPointer = static_cast<Silencer*>(dialog->getComponent()))
                component = QSharedPointer<Silencer>(rawPointer);
            else
                return;
            addComponent(component);
        }
    }
    else
    {
        Widget_Silencer_inputTable* inputTable = new Widget_Silencer_inputTable(true, silencer_type);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_Silencer_inputTable::handleConfirmation);
    }
}

void Widget_Silencer_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 1);
}

void Widget_Silencer_inputTable::onRevise()
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
            componentRevision<Silencer, Dialog_silencer>(tableWidget, row, silencer_type);
        }
    }
}

void Widget_Silencer_inputTable::onInput()
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
        QString data_source = parsedData[11];

        array<QString, 8> atten = {""};

        for(int i = 0; i < 8; i++) {
            atten[i] = parsedData[i][i + 3];
        }

        Silencer* componentRaw = new Silencer(model, brand, table_id, UUID, data_source, silencer_type, atten);

        QSharedPointer<Silencer> component = QSharedPointer<Silencer>(componentRaw);
        addComponent(component);
    }
}

void Widget_Silencer_inputTable::onOutput()
{
    ExcelEngine* excelEngine = new ExcelEngine(this);
    excelEngine->deriveExecl(ui->tableWidget, silencer_type);
}

void Widget_Silencer_inputTable::onGenerateTemplate()
{
    getTemplate(":/componentImportTemplate/componentImportTemplate/silencer.xlsx",
                silencer_type + "导入模板");
}

void Widget_Silencer_inputTable::loadComponentToTable()
{
    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::SILENCER);
    for (const auto& component : componentList) {
        if (auto silencerComponent = dynamic_cast<Silencer*>(component.data())) {
            if(silencerComponent->silencer_type != silencer_type)
                continue;
            auto lists = silencerComponent->getComponentDataAsStringList(inComponentDB);
            QTableWidget* tableWidget = ui->tableWidget;

            for (const auto& list : lists) {
                addRowToTable(tableWidget, list);
            }
        }
    }
}

void Widget_Silencer_inputTable::addComponent(QSharedPointer<Silencer> &component)
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

void Widget_Silencer_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto& uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);;
        if(QSharedPointer<Silencer> component = qSharedPointerCast<Silencer>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<Silencer> newComponent = QSharedPointer<Silencer>(new Silencer(*component));
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
