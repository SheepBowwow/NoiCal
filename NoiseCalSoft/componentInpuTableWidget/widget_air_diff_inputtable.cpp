#include "widget_air_diff_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_air_diff.h"

Widget_air_diff_inputTable::Widget_air_diff_inputTable(QWidget *parent)
    : Widget_base_inputTable(parent)
{
    setTitle("布风器+散流器");
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_air_diff_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_air_diff_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_air_diff_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_air_diff_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_air_diff_inputTable::onOutput);
}

void Widget_air_diff_inputTable::initTableWidget()
{
    int colCount = 20;
    QStringList headerText;
    headerText<< "" << "序号" << "布风器型号" << "布风器品牌" << "散流器型号" << "散流器品牌" << "末端\n类型" << "末端\n尺寸"
              << "数据类型" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)"
              << "4kHz\n(dB)" << "8kHz\n(dB)" << "总值\ndB(A)" << "来源" << "UUID";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 100, 120, 100, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};
    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);
    QStringList merge
    {
        "",
        "序号",
        "布风器型号",
        "布风器品牌",
        "散流器型号",
        "散流器品牌",
        "末端\n类型",
        "静压\n(Pa)",
        "末端\n尺寸",
        "UUID"
    };
    this->mergeCols = merge;
    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_air_diff_inputTable::onAdd()
{
    QTableWidget *tableWidget = ui->tableWidget;
    Dialog_air_diff *dialog = new Dialog_air_diff();
    QSharedPointer<AirDiff> component = nullptr;

    if (dialog->exec() == QDialog::Accepted) {
        if(AirDiff* rawPointer = static_cast<AirDiff*>(dialog->getComponent()))
            component = QSharedPointer<AirDiff>(rawPointer);
        else
            return;
        component->table_id = QString::number(tableWidget->rowCount() / 3 + 1);
        if (component != nullptr) {
            auto lists = component->getComponentDataAsStringList();
            // 使用通用函数添加行
            addRowToTable(tableWidget, lists[0]);
            addRowToTable(tableWidget, lists[1]);
            addRowToTable(tableWidget, lists[2]);

            mergeColumnsByNames(tableWidget, mergeCols, 3);

            componentManager.addComponent(component);
        }
    }
}

void Widget_air_diff_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 3);
}

void Widget_air_diff_inputTable::onRevise()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); row += 3) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            componentRevision<AirDiff, Dialog_air_diff>(ui->tableWidget, 3, row);
            mergeColumnsByNames(ui->tableWidget, mergeCols, 3);
        }
    }
}

void Widget_air_diff_inputTable::onInput()
{

}

void Widget_air_diff_inputTable::onOutput()
{

}

void Widget_air_diff_inputTable::loadComponentToTable()
{
    // 获取所有AirDiff组件
    auto componentList = ComponentManager::getInstance().getComponentsByType(component_type_name::AIRDIFF);
    for (const auto& component : componentList) {
        // 使用dynamic_cast尝试将ComponentBase转换为AirDiff指针
        if (auto airDiffComponent = dynamic_cast<AirDiff*>(component.data())) {
            // 获取组件数据
            auto lists = airDiffComponent->getComponentDataAsStringList();

            // 假定lists中的数据已经正确分组对应噪声、衰减、反射
            if (lists.size() >= 3) {
                // 添加数据到对应的表格
                addRowToTable(ui->tableWidget, lists[0]);
                addRowToTable(ui->tableWidget, lists[1]);
                addRowToTable(ui->tableWidget, lists[2]);
            }
        }
    }
    mergeColumnsByNames(ui->tableWidget, mergeCols, 3);
}
