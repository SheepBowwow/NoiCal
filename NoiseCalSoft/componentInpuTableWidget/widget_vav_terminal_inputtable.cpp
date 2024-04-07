#include "widget_vav_terminal_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_vav_terminal.h"

Widget_VAV_terminal_inputTable::Widget_VAV_terminal_inputTable(QWidget *parent) :
    Widget_base_inputTable(parent)
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
    int colCount = 18;
    QStringList headerText;
    headerText<< "" << "序号" << "编号" << "型号"  << "品牌" << "阀门\n开度" << "风量\n(m³/h)" << "63Hz\n(dB)" << "125Hz\n(dB)"
              << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源" << "UUID";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 120, 80, 60, 60, 55, 55, 55, 55, 55, 55, 55, 55, 55 ,60, 0};
    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);

    // 隐藏最后一列（假设隐藏列是最后一列）
    ui->tableWidget->setColumnHidden(colCount - 1, true); // 让隐藏列不可见
}

void Widget_VAV_terminal_inputTable::onAdd()
{
    QTableWidget *tableWidget = ui->tableWidget;
    Dialog_VAV_terminal *dialog = new Dialog_VAV_terminal(this);
    QSharedPointer<VAV_terminal> component;

    if (dialog->exec() == QDialog::Accepted) {
        if(VAV_terminal* rawPointer = static_cast<VAV_terminal*>(dialog->getComponent()))
            component = QSharedPointer<VAV_terminal>(rawPointer);
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
        QCheckBox* checkBox = widget ? qobject_cast<QCheckBox*>(widget) : nullptr;
        if(checkBox && checkBox->isChecked())
        {
            // 获取UUID，假设它在最后一列
            QString UUID = tableWidget->item(row, tableWidget->columnCount() - 1)->text();
            // 调用通用的修订函数，传入正确的类型参数
            componentRevision<VAV_terminal, Dialog_VAV_terminal>(tableWidget, row);
        }
    }
}

void Widget_VAV_terminal_inputTable::onInput()
{

}

void Widget_VAV_terminal_inputTable::onOutput()
{

}
