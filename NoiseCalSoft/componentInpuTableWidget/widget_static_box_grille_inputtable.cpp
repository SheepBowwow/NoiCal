#include "widget_static_box_grille_inputtable.h"
#include "ui_widget_terminal_base_inputtable.h"
#include "inputDialog/dialog_staticBox_grille.h"

Widget_static_box_grille_inputTable::Widget_static_box_grille_inputTable(QWidget *parent)
    : Widget_terminal_base_inputTable(parent)
{
    setTitle("静压箱+格栅");
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_static_box_grille_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_static_box_grille_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_static_box_grille_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_static_box_grille_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_static_box_grille_inputTable::onOutput);
}

void Widget_static_box_grille_inputTable::initTableWidget()
{
    int colCount = 19;
    QStringList headerText;
    headerText<< "" << "序号" << "静压箱型号" << "静压箱品牌" << "格栅型号" << "格栅品牌" << "末端\n类型" << "末端\n尺寸"  << "63Hz\n(dB)"
              << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
              << "8kHz\n(dB)" << "总值\ndB(A)" << "来源" << "UUID";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 100, 120, 100, 65, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};
    setTableWidget(ui->tableWidget_noi, headerText, columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget_noi->setColumnHidden(colCount - 1, true);


    colCount = 18;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "静压箱型号" << "静压箱品牌" << "格栅型号" << "格栅品牌" << "末端\n类型" << "末端\n尺寸"  << "63Hz\n(dB)"
                    << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
                    << "8kHz\n(dB)" << "来源" << "UUID";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {30, 40, 120, 100, 120, 100, 65, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};

    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget_atten, headerText_atten, atten_columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget_atten->setColumnHidden(colCount - 1, true);


    colCount = 18;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "静压箱型号" << "静压箱品牌" << "格栅型号" << "格栅品牌" << "末端\n类型" << "末端\n尺寸"  << "63Hz\n(dB)"
                   << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
                   << "8kHz\n(dB)" << "来源" << "UUID";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {30, 40, 120, 100, 120, 100, 65, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};

    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget_refl, headerText_refl, refl_columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget_refl->setColumnHidden(colCount - 1, true);
}

void Widget_static_box_grille_inputTable::onAdd()
{
    QTableWidget *tableWidget_noise = ui->tableWidget_noi;
    QTableWidget *tableWidget_atten = ui->tableWidget_atten;
    QTableWidget *tableWidget_refl = ui->tableWidget_refl;
    Dialog_staticBox_grille *dialog = new Dialog_staticBox_grille(this);
    QSharedPointer<StaticBox_grille> component;

    if (dialog->exec() == QDialog::Accepted) {
        if(StaticBox_grille* rawPointer = static_cast<StaticBox_grille*>(dialog->getComponent()))
            component = QSharedPointer<StaticBox_grille>(rawPointer);
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

void Widget_static_box_grille_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget_noi, ui->tableWidget_atten, ui->tableWidget_refl);
}

void Widget_static_box_grille_inputTable::onRevise()
{
    QTableWidget* currentTableWidget = ui->stackedWidget->currentWidget()->findChild<QTableWidget*>();
    QTableWidget* tableWidget_noise = ui->tableWidget_noi;
    QTableWidget* tableWidget_atten = ui->tableWidget_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_refl;
    QVector<QTableWidget*> tableWidgets = {ui->tableWidget_noi, ui->tableWidget_atten, ui->tableWidget_refl};

    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        // 假设你的复选框在第一列
        QWidget* widget = currentTableWidget->cellWidget(row, 0);
        QCheckBox* checkBox = widget ? qobject_cast<QCheckBox*>(widget) : nullptr;
        if(checkBox && checkBox->isChecked())
        {
            // 获取UUID，假设它在最后一列
            QString UUID = currentTableWidget->item(row, currentTableWidget->columnCount() - 1)->text();
            // 调用通用的修订函数，传入正确的类型参数
            componentRevision<StaticBox_grille, Dialog_staticBox_grille>(tableWidgets,currentTableWidget,row);
        }
    }
}

void Widget_static_box_grille_inputTable::onInput()
{

}

void Widget_static_box_grille_inputTable::onOutput()
{

}



