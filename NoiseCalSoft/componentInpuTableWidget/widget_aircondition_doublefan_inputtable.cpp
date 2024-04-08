#include "widget_aircondition_doublefan_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_aircondition.h"

Widget_aircondition_doubleFan_inputTable::Widget_aircondition_doubleFan_inputTable(QWidget *parent) :
    Widget_base_inputTable(parent)
{
    setTitle("空调器(双风机)");

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
        if(i == 7 || i == 2 || i == 5 || i == 6)
        {
            tableWidget->setSpan(startRow, i, 2, 1);
            tableWidget->setSpan(startRow + 2, i, 2, 1);
        }
        else if(i < 7 || i > 17)
        {
            tableWidget->setSpan(startRow, i, 4, 1);
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

void Widget_aircondition_doubleFan_inputTable::initTableWidget()
{
    int colCount = 20;

    QStringList headerText;
    headerText << "" << "序号" << "编号" << "型号" << "品牌" << "风量\n(m³/h)" << "静压\n(Pa)" << "类型"
               << "噪音\n位置" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
               << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "总值\ndB(A)" << "来源" << "UUID";

    int columnWidths[] = {30, 40, 120, 120, 80, 55, 55, 55, 60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_aircondition_doubleFan_inputTable::onAdd()
{
    QTableWidget *tableWidget = ui->tableWidget;
    Dialog_aircondition *dialog = new Dialog_aircondition(nullptr, -1, 2);
    QSharedPointer<Aircondition> component;

    int rowCount = tableWidget->rowCount(); //获取当前行数
    if (dialog->exec() == QDialog::Accepted) {
        if(Aircondition* rawPointer = static_cast<Aircondition*>(dialog->getComponent()))
            component = QSharedPointer<Aircondition>(rawPointer);
        else
            return;
        component->table_id = QString::number(tableWidget->rowCount() / 4 + 1);
        if (component != nullptr) {
            auto lists = dialog->getComponentDataAsStringList();

            // 使用通用函数添加行
            addRowToTable(tableWidget, lists[0]);
            addRowToTable(tableWidget, lists[1]);
            addRowToTable(tableWidget, lists[2]);
            addRowToTable(tableWidget, lists[3]);

            componentManager.addComponent(component);

            mergeCells(rowCount);
        }
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

void Widget_aircondition_doubleFan_inputTable::onRevise()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            QString uuid = ui->tableWidget->item(row, ui->tableWidget->columnCount() - 1)->text();
            QSharedPointer<Aircondition> component = componentManager.findComponent(uuid).dynamicCast<Aircondition>();

            Dialog_aircondition *dialog = new Dialog_aircondition(this, row, 2, *component);
            if (dialog->exec() == QDialog::Accepted) {
                QSharedPointer<Aircondition> newComponent = QSharedPointer<Aircondition>(static_cast<Aircondition*>(dialog->getComponent()));

                if (newComponent && componentManager.updateComponent(uuid, newComponent)) {
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

}

void Widget_aircondition_doubleFan_inputTable::onOutput()
{

}

