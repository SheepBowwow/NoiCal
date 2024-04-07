#include "widget_fancoil_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_fancoil.h"

Widget_fanCoil_inputTable::Widget_fanCoil_inputTable(QWidget *parent) :
    Widget_base_inputTable(parent)
{
    setTitle("风机盘管");
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


void Widget_fanCoil_inputTable::mergeCells(int startRow)
{
    QTableWidget *tableWidget = ui->tableWidget;
    for(int i = 0; i < tableWidget->columnCount(); i++)
    {
        if(i < 7 || i > 16)
        {
            tableWidget->setSpan(startRow, i, 2, 1);
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

void Widget_fanCoil_inputTable::initTableWidget()
{
    int colCount = 19;
    QStringList headerText;
    headerText << "" << "序号" << "类型" << "型号" << "品牌" << "风量\n(m³/h)" << "静压\n(Pa)" << "噪音\n位置" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
               << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "总值\ndB(A)" << "来源" << "UUID";
    int columnWidths[] = {30, 40, 90, 150, 95, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 55, 0};
    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);

    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_fanCoil_inputTable::onAdd()
{
    //修改
    QTableWidget *tableWidget = ui->tableWidget;
    Dialog_fanCoil *dialog = new Dialog_fanCoil(this);
    QSharedPointer<FanCoil> component;

    int rowCount = tableWidget->rowCount(); //获取当前行数
    if (dialog->exec() == QDialog::Accepted) {
        if(FanCoil* rawPointer = static_cast<FanCoil*>(dialog->getComponent()))
            component = QSharedPointer<FanCoil>(rawPointer);
        else
            return;

        component->table_id = QString::number(tableWidget->rowCount() / 2 + 1);
        if (component != nullptr) {

            auto lists = dialog->getComponentDataAsStringList();
            // 使用通用函数添加行
            addRowToTable(tableWidget, lists[0]);
            addRowToTable(tableWidget, lists[1]);

            componentManager.addComponent(component);

            mergeCells(rowCount);
        }
    }
}

void Widget_fanCoil_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 2);


    for(int i = 0; i < ui->tableWidget->rowCount(); i += 2)
    {
        mergeCells(i);
    }
}

void Widget_fanCoil_inputTable::onRevise()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            QString uuid = ui->tableWidget->item(row, ui->tableWidget->columnCount() - 1)->text();
            QSharedPointer<FanCoil> component = componentManager.findComponent(uuid).dynamicCast<FanCoil>();

            Dialog_fanCoil *dialog = new Dialog_fanCoil(this, row, *component);
            if (dialog->exec() == QDialog::Accepted) {
                QSharedPointer<FanCoil> newComponent = QSharedPointer<FanCoil>(static_cast<FanCoil*>(dialog->getComponent()));

                if (newComponent && componentManager.updateComponent(uuid, newComponent)) {
                    int insertPosition = row; // 记录要插入新行的位置

                    // 删除旧行
                    ui->tableWidget->removeRow(row + 1); // 先删除下一行
                    ui->tableWidget->removeRow(row); // 再删除当前行

                    auto lists = dialog->getComponentDataAsStringList();

                    // 现在在记录的位置插入行
                    addRowToTable(ui->tableWidget, lists[0], insertPosition);
                    addRowToTable(ui->tableWidget, lists[1], insertPosition + 1); // 注意这里是在紧接着上一行后插入
                    mergeCells(row);
                }
                delete dialog; // Ensure dialog is deleted to avoid memory leak
            }
        }
    }
}

void Widget_fanCoil_inputTable::onInput()
{

}

void Widget_fanCoil_inputTable::onOutput()
{

}

