#include "widget_pipe_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_pipe.h"

Widget_Pipe_inputTable::Widget_Pipe_inputTable(QWidget* parent)
    :Widget_base_inputTable(parent)
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
    int colCount = 16;

    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "品牌" << "类型" << "尺寸\n(mm)" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源" << "UUID";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 70, 55, 70, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};

    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_Pipe_inputTable::onAdd()
{
    QTableWidget *tableWidget = ui->tableWidget;
    Dialog_pipe *dialog = new Dialog_pipe(this);
    QSharedPointer<Pipe> component;

    if (dialog->exec() == QDialog::Accepted) {
        if(Pipe* rawPointer = static_cast<Pipe*>(dialog->getComponent()))
            component = QSharedPointer<Pipe>(rawPointer);
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
        QCheckBox* checkBox = widget ? qobject_cast<QCheckBox*>(widget) : nullptr;
        if(checkBox && checkBox->isChecked())
        {
            // 获取UUID，假设它在最后一列
            QString UUID = tableWidget->item(row, tableWidget->columnCount() - 1)->text();
            // 调用通用的修订函数，传入正确的类型参数
            componentRevision<Pipe, Dialog_pipe>(tableWidget, row);
        }
    }
}

void Widget_Pipe_inputTable::onInput()
{

}

void Widget_Pipe_inputTable::onOutput()
{

}
