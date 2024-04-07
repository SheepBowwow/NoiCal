#include "widget_reducer_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_reducer.h"

Widget_Reducer_inputTable::Widget_Reducer_inputTable(QWidget *parent)
    :Widget_base_inputTable(parent)
{
    setTitle("变径");
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_Reducer_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_Reducer_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_Reducer_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_Reducer_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_Reducer_inputTable::onOutput);
}

void Widget_Reducer_inputTable::initTableWidget()
{
    int colCount = 17;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "品牌" << "类型" << "变径前尺寸\n(mm)" << "变径后尺寸\n(mm)" << "63Hz\n(dB)"
              << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)"
              << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源" << "UUID";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 80, 70, 70, 70, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};

    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_Reducer_inputTable::onAdd()
{
    QTableWidget *tableWidget = ui->tableWidget;
    Dialog_reducer *dialog = new Dialog_reducer(this);
    QSharedPointer<Reducer> component;

    if (dialog->exec() == QDialog::Accepted) {
        if(Reducer* rawPointer = static_cast<Reducer*>(dialog->getComponent()))
            component = QSharedPointer<Reducer>(rawPointer);
        else
            return;
        component->table_id = QString::number(tableWidget->rowCount() + 1);
        if (component != nullptr) {
            auto lists = dialog->getComponentDataAsStringList();

            // 使用通用函数添加行
            addRowToTable(tableWidget, lists[0]);

            componentManager.addComponent(component);
        }
    }
}

void Widget_Reducer_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 1);
}

void Widget_Reducer_inputTable::onRevise()
{
    QTableWidget* tableWidget = ui->tableWidget;

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        // 假设你的复选框在第一列
        QWidget* widget = tableWidget->cellWidget(row, 0);
        QCheckBox* checkBox = widget ? qobject_cast<QCheckBox*>(widget) : nullptr;
        if(checkBox && checkBox->isChecked())
        {
            // 获取UUID，假设它在最后一列
            QString UUID = tableWidget->item(row, tableWidget->columnCount() - 1)->text();
            // 调用通用的修订函数，传入正确的类型参数
            componentRevision<Reducer, Dialog_reducer>(tableWidget, row);
        }
    }
}

void Widget_Reducer_inputTable::onInput()
{

}

void Widget_Reducer_inputTable::onOutput()
{

}
