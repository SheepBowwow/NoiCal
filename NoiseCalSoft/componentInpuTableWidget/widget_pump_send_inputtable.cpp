#include "widget_pump_send_inputtable.h"
#include "ui_widget_pump_send_inputtable.h"
#include "inputDialog/dialog_pump_send.h"
#include <QQueue>

Widget_Pump_Send_inputTable::Widget_Pump_Send_inputTable(QWidget *parent) :
    Widget_base_inputTable(parent),
    ui(new Ui::Widget_Pump_Send_inputTable)
{
    ui->setupUi(this);
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_Pump_Send_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_Pump_Send_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_Pump_Send_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_Pump_Send_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_Pump_Send_inputTable::onOutput);
}

Widget_Pump_Send_inputTable::~Widget_Pump_Send_inputTable()
{
    delete ui;
}

void Widget_Pump_Send_inputTable::initTableWidget()
{
    int colCount = 17;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "品牌" << "末端\n类型" << "末端\n尺寸"  << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源" << "UUID";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 80, 60, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};

    setTableWidget(ui->tableWidget_noi_pump, headerText, columnWidths, colCount);
    setTableWidget(ui->tableWidget_noi_send, headerText, columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget_noi_pump->setColumnHidden(colCount - 1, true);
    ui->tableWidget_noi_send->setColumnHidden(colCount - 1, true);

    colCount = 16;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "型号" << "品牌" << "末端\n类型" << "末端\n尺寸"  << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                    << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                    << "来源" << "UUID";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {30, 40, 120, 80, 60, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};

    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget_atten_pump, headerText_atten, atten_columnWidths, colCount);
    setTableWidget(ui->tableWidget_atten_send, headerText_atten, atten_columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget_atten_pump->setColumnHidden(colCount - 1, true);
    ui->tableWidget_atten_send->setColumnHidden(colCount - 1, true);

    colCount = 16;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "型号" << "品牌" << "末端\n类型" << "末端\n尺寸"  << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                   << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                   << "来源" << "UUID";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {30, 40, 120, 80, 60, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};

    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget_refl_pump, headerText_refl, refl_columnWidths, colCount);
    setTableWidget(ui->tableWidget_refl_send, headerText_refl, refl_columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget_refl_pump->setColumnHidden(colCount - 1, true);
    ui->tableWidget_refl_send->setColumnHidden(colCount - 1, true);
}

void Widget_Pump_Send_inputTable::setCurrentTable(const int& index)
{
    ui->stackedWidget->setCurrentIndex(index);
    if(ui->stackedWidget->currentWidget()->findChild<QStackedWidget*>()->currentIndex() == 0)
    {
        on_pushButton_pump_clicked();
    }
    else
    {
        on_pushButton_send_clicked();
    }
}

void Widget_Pump_Send_inputTable::onAdd()
{
    int index = ui->stackedWidget->currentWidget()->findChild<QStackedWidget*>()->currentIndex();
    QTableWidget *tableWidget_noise = nullptr;
    QTableWidget *tableWidget_atten = nullptr;
    QTableWidget *tableWidget_refl = nullptr;
    Dialog_pump_send *dialog = nullptr;
    QSharedPointer<PumpSend> component;

    if(index == 0)
    {
        tableWidget_noise = ui->tableWidget_noi_pump;
        tableWidget_atten = ui->tableWidget_atten_pump;
        tableWidget_refl = ui->tableWidget_refl_pump;
        dialog = new Dialog_pump_send("抽风头",nullptr);
    }
    else if(index == 1)
    {
        tableWidget_noise = ui->tableWidget_noi_send;
        tableWidget_atten = ui->tableWidget_atten_send;
        tableWidget_refl = ui->tableWidget_refl_send;
        dialog = new Dialog_pump_send("送风头",nullptr);
    }
    else
    {
        qDebug() << "index 不为预期值";
        return;
    }

    if (dialog->exec() == QDialog::Accepted) {
        if(PumpSend* rawPointer = static_cast<PumpSend*>(dialog->getComponent()))
            component = QSharedPointer<PumpSend>(rawPointer);
        else
            return;
        component->table_id = QString::number(tableWidget_noise->rowCount() + 1);
        if (component != nullptr) {
            auto lists = dialog->getComponentDataAsStringList();
            // 使用通用函数添加行
            addRowToTable(tableWidget_noise, lists[0]);
            addRowToTable(tableWidget_atten, lists[1]);
            addRowToTable(tableWidget_refl, lists[2]);

            componentManager.addComponent(component);
        }
    }
}

void Widget_Pump_Send_inputTable::onDel()
{
    int index = ui->stackedWidget->currentWidget()->findChild<QStackedWidget*>()->currentIndex();
    if(index == 0)
        deleteRowFromTable(ui->tableWidget_noi_pump, ui->tableWidget_atten_pump, ui->tableWidget_refl_pump);
    else if(index == 1)
        deleteRowFromTable(ui->tableWidget_noi_send, ui->tableWidget_atten_send, ui->tableWidget_refl_send);
}

void Widget_Pump_Send_inputTable::onRevise()
{
    int index = ui->stackedWidget->currentWidget()->findChild<QStackedWidget*>()->currentIndex();
    QTableWidget* currentTableWidget = ui->stackedWidget->currentWidget()->findChild<QStackedWidget*>()->findChild<QTableWidget*>();
    QTableWidget *tableWidget_noise = nullptr;
    QTableWidget *tableWidget_atten = nullptr;
    QTableWidget *tableWidget_refl = nullptr;
    Dialog_pump_send *dialog = nullptr;
    QString typeName = "";

    if(index == 0)
    {
        tableWidget_noise = ui->tableWidget_noi_pump;
        tableWidget_atten = ui->tableWidget_atten_pump;
        tableWidget_refl = ui->tableWidget_refl_pump;
        typeName = "抽风头";
    }
    else if(index == 1)
    {
        tableWidget_noise = ui->tableWidget_noi_send;
        tableWidget_atten = ui->tableWidget_atten_send;
        tableWidget_refl = ui->tableWidget_refl_send;
        typeName = "送风头";
    }
    else
        return;

    QVector<QTableWidget*> tableWidgets = { tableWidget_noise, tableWidget_atten, tableWidget_refl};

    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        // 假设你的复选框在第一列
        QWidget* widget = currentTableWidget->cellWidget(row, 0);
        QCheckBox* checkBox = widget ? qobject_cast<QCheckBox*>(widget) : nullptr;
        if(checkBox && checkBox->isChecked())
        {
            // 调用通用的修订函数，传入正确的类型参数
            componentRevision<PumpSend, Dialog_pump_send>(tableWidgets, currentTableWidget, row, typeName);
        }
    }
}

void Widget_Pump_Send_inputTable::onInput()
{

}

void Widget_Pump_Send_inputTable::onOutput()
{

}

void Widget_Pump_Send_inputTable::on_pushButton_pump_clicked()
{
    if(QStackedWidget* child = ui->stackedWidget->currentWidget()->findChild<QStackedWidget*>())
    {
        child->setCurrentIndex(0);
        ui->pushButton_send->setStyleSheet("QPushButton { background-color: white; }");
        ui->pushButton_pump->setStyleSheet("QPushButton { background-color: #E0EEF9; }");

    }
}


void Widget_Pump_Send_inputTable::on_pushButton_send_clicked()
{
    if(QStackedWidget* child = ui->stackedWidget->currentWidget()->findChild<QStackedWidget*>())
    {
        child->setCurrentIndex(1);
        ui->pushButton_send->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
        ui->pushButton_pump->setStyleSheet("QPushButton { background-color: white; }");
    }
}

void Widget_Pump_Send_inputTable::clearTableFuc()
{
    // 定义一个Lambda表达式用于递归清空QTableWidgets
    std::function<void(QWidget*)> clearAllTableWidgets = [&](QWidget* parent) {
        if (!parent) return;

        // 创建一个队列用于广度优先搜索
        QQueue<QWidget*> queue;
        queue.enqueue(parent);

        while (!queue.isEmpty()) {
            QWidget* currentWidget = queue.dequeue();

            // 尝试将当前控件转换为QTableWidget
            QTableWidget *tableWidget = qobject_cast<QTableWidget*>(currentWidget);
            if (tableWidget) {
                // 如果转换成功，清空QTableWidget
                tableWidget->setRowCount(0);
                tableWidget->clearContents(); // 清空内容但保留列头
            } else {
                // 如果当前控件不是QTableWidget，将它的子控件加入队列
                const auto children = currentWidget->children();
                for (QObject *child : children) {
                    QWidget *childWidget = qobject_cast<QWidget*>(child);
                    if (childWidget) {
                        queue.enqueue(childWidget);
                    }
                }
            }
        }
    };

    // 使用定义的Lambda表达式从该窗口控件开始递归清空所有QTableWidget
    clearAllTableWidgets(this);
}

void Widget_Pump_Send_inputTable::loadComponentToTable()
{
    // 从组件管理器获取所有可能相关的组件
    auto componentList = ComponentManager::getInstance().getComponentsByType(component_type_name::PUMPSEND); // 假设"PumpSend"是通用类型标识符
    for (const auto& component : componentList) {
        if (auto specificComponent = dynamic_cast<PumpSend*>(component.data())) {
            auto lists = specificComponent->getComponentDataAsStringList();
            if (specificComponent->type_pump_or_send == "抽风头") {
                // 假定lists中的数据已经正确分组对应噪声、衰减、反射
                if (lists.size() >= 3) {
                    // 添加数据到对应的表格
                    addRowToTable(ui->tableWidget_noi_pump, lists[0]);
                    addRowToTable(ui->tableWidget_atten_pump, lists[1]);
                    addRowToTable(ui->tableWidget_refl_pump, lists[2]);
                }
            } else if (specificComponent->type_pump_or_send == "送风头") {
                if (lists.size() >= 3) {
                    // 添加数据到对应的表格
                    addRowToTable(ui->tableWidget_noi_send, lists[0]);
                    addRowToTable(ui->tableWidget_atten_send, lists[1]);
                    addRowToTable(ui->tableWidget_refl_send, lists[2]);
                }
            }
        }
    }
}

