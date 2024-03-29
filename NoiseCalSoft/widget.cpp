#include "widget.h"
#include "ui_widget.h"
/**窗口类**/
#include "inputDialog/dialog_prj_manager.h""
#include "inputDialog/dialog_fan_noise.h"
#include "inputDialog/dialog_fancoil_noise.h"
#include "inputDialog/dialog_air_diff.h"
#include "inputDialog/dialog_pump_send.h"
#include "inputDialog/dialog_staticBox_grille.h"
#include "inputDialog/dialog_vav_terminal.h"
#include "inputDialog/dialog_disp_vent_terminal.h"
#include "inputDialog/dialog_static_box.h"
#include "inputDialog/dialog_other_send_terminal.h"
#include "inputDialog/dialog_silencer.h"
#include "inputDialog/dialog_tee.h"
#include "inputDialog/dialog_duct_with_multi_ranc.h"
#include "inputDialog/dialog_circular_damper.h"
#include "inputDialog/dialog_rect_damper.h"
#include "inputDialog/dialog_pipe.h"
#include "inputDialog/dialog_aircondition_noise.h"
#include "inputDialog/dialog_reducer.h"
#include "inputDialog/dialog_elbow.h"
#include "roomDefineForm/form_room_define.h"
#include "roomDefineForm/form_system_list.h"
#include "roomDefineForm/dialog_add_zhushuqu.h"
#include "roomCal/room_cal_basewidget.h"
#include "roomCal/room_cal_total.h"
/**窗口类**/
#include <QDebug>
#include <QVector>
#include <QQueue>
#include <QColor>
#include <QTimer>
#include <QStandardItemModel>
#include <QCheckBox>
#include <QSharedPointer>
#include <QFileDialog>
#include "Component/ComponentManager.h"
#include "wordEngine/wordengine.h"
#include <QResource>
#include <QDesktopServices>
#include "global_constant.h"

#include <iostream>
#include <string>
#include <regex>
#include <QDateTime>

WordEngine* wordEngine = new WordEngine();

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->page_login);
    this->initTableWidget_noi_limit();
    this->initTableWidget_drawing_list();
    this->initTableWidget_fan_noi();
    this->initTableWidget_fanCoil_noi();
    this->initTableWidget_air_diff();
    this->initTableWidget_pump_send_tuyere();
    this->initTableWidget_staticBox_grille();
    this->initTableWidget_VAV_terminal();
    this->initTableWidget_disp_vent_terminal();
    this->initTableWidget_static_box();
    this->initTableWidget_other_send_terminal();
    this->initTableWidget_silencer();
    this->initTableWidegt_tee();
    this->initTableWidegt_duct_with_multi_ranc();
    this->initTableWidget_circular_damper();
    this->initTableWidget_rect_damper();
    this->initTableWidget_pipe();
    this->initTableWidget_air_noi_single();
    this->initTableWidget_air_noi_double();
    this->initTableWidget_reducer();
    this->initTableWidget_elbow();
    this->initRightButtonMenu();
    this->initTableWidget_system_list();
    this->initTableWidget_report_cal_room();
    this->initTableWidget_project_attachment();
}

Widget::~Widget()
{
    wordEngine->close();
    delete wordEngine;
    delete ui;
}




/**********输入界面表格初始化及其他按钮设置**********/
#pragma region "input_table_func" {
void Widget::initTableWidget(QTableWidget *tableWidget, const QStringList &headerText, const int *columnWidths, int colCount)
{
    tableWidget->setColumnCount(colCount);
    tableWidget->setRowCount(0);
    tableWidget->setHorizontalHeaderLabels(headerText);
    tableWidget->verticalHeader()->setVisible(false);

    int totalWidth = 0;
    for (int i = 0; i < colCount; ++i)
    {
        totalWidth += columnWidths[i];
    }

    for (int i = 0; i < colCount; ++i)
    {
        double ratio = static_cast<double>(columnWidths[i]) / totalWidth;
        int columnWidth = static_cast<int>(ratio * tableWidget->width());
        tableWidget->setColumnWidth(i, columnWidth);
    }

    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

void Widget::buttonToHeader(QTableWidget *tableWidget, QWidget *buttonWidget, const char *addButtonSlot, const char *delButtonSlot)
{
    QHeaderView *header = tableWidget->horizontalHeader();
    int lastColumnIndex = header->count() - 1;

    int x = header->sectionViewportPosition(lastColumnIndex);
    int y = 0;
    int width = header->sectionSize(lastColumnIndex);
    int height = header->height();

    // 将相对坐标映射到当前页的坐标系
    QPoint relativePos = header->mapTo(tableWidget->parentWidget(), QPoint(x, y - height));

    // 设置按钮的几何位置
    buttonWidget->setGeometry(
        relativePos.x() + 1,
        relativePos.y(),
        width - 2,
        height - 2
    );

    QHBoxLayout* layout = new QHBoxLayout(buttonWidget);
    // 创建"+"按钮
    QPushButton* addButton = new QPushButton("+");
    addButton->setFixedSize(15, 15);  // 设置按钮的尺寸为正方形，例如30x30像素
    layout->addWidget(addButton);
    // 创建"-"按钮
    QPushButton* delButton = new QPushButton("-");
    delButton->setFixedSize(15, 15);  // 设置按钮的尺寸为正方形，例如30x30像素
    layout->addWidget(delButton);
    // 在构造函数或者初始化函数中连接信号和槽函数
    connect(addButton, SIGNAL(clicked()), this, addButtonSlot);
    connect(delButton, SIGNAL(clicked()), this, delButtonSlot);

    buttonWidget->setStyleSheet(QString("#%1{background : rgb(157, 198, 230);}").arg(buttonWidget->objectName()));
}

void Widget::addRowToTable(QTableWidget *tableWidget, const QStringList &data)
{
    int rowCount = tableWidget->rowCount();
    tableWidget->setRowCount(rowCount + 1);

    for (int i = 0; i < data.size() + 1; ++i) {
        if (i == 0) {
            // 处理复选框
            QCheckBox* checkBox = new QCheckBox();
            QWidget* widget = new QWidget();
            widget->setStyleSheet("background-color: #C0C0C0;");
            QHBoxLayout* layout = new QHBoxLayout(widget);
            layout->addWidget(checkBox);
            layout->setAlignment(Qt::AlignCenter);
            layout->setContentsMargins(0, 0, 0, 0);
            tableWidget->setCellWidget(rowCount, i, widget);
        }
        else
        {
            QTableWidgetItem *item = new QTableWidgetItem(data[i - 1]);
            tableWidget->setItem(rowCount, i, item);
            item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
            item->setFlags(Qt::ItemIsEditable); // 设置为只读
            item->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
            item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色
        }
    }
}

void Widget::deleteRowFromTable(QTableWidget *tableWidget, int deleteRowNum, QString componentName)
{
    // 获取选中的行索引
    QList<int> selectedRows;
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        QWidget* widget = tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox && checkBox->isChecked()) {
            selectedRows.append(row);
        }
    }

    // 弹窗确认
    QString confirmationMessage = "确认删除以下行吗？\n";
    for (int i = 0; i < selectedRows.size(); ++i)
    {
        int row = selectedRows[i];
        confirmationMessage += QString::number((row + 1) / deleteRowNum) + "\n"; // 从1开始计数
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("确认删除");
    msgBox.setText(confirmationMessage);
    msgBox.setIcon(QMessageBox::Warning);
    QPushButton *yesButton = msgBox.addButton("确认", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("取消", QMessageBox::NoRole);
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton)
    {
        for (int i = selectedRows.size() - 1; i >= 0; --i)
        {
            int row = selectedRows[i];
            tableWidget->removeRow(row);
            if(deleteRowNum != 1)
            {
                for(int i = 1; i <= deleteRowNum - 1; i++)
                {
                    tableWidget->removeRow(row - i);
                }
            }
            if(deleteRowNum != 1)
                componentManager.del_and_updateTableID((row + 1) / deleteRowNum, componentName);
            else
                componentManager.del_and_updateTableID(row + 1, componentName);
        }


        // 重新编号
        for (int row = 0; row < tableWidget->rowCount(); ++row) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(deleteRowNum == 1 ? (row + 1) : (row / deleteRowNum + 1)));
            tableWidget->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(Qt::ItemIsEditable);
            item->setBackground(QBrush(Qt::lightGray));
            item->setData(Qt::ForegroundRole, QColor(70, 70, 70));
        }
        if(deleteRowNum == 2)
        {
            QString columnName_1 = "63Hz"; // 这是你想合并单元格的列的名称
            QString columnName_2 = "来源"; // 这是你想合并单元格的列的名称
            int targetColumnIndex_1 = -1;
            int targetColumnIndex_2 = -1;
            // 找到该列名称对应的列索引
            for(int j = 0; j < tableWidget->columnCount(); ++j) {
                if(tableWidget->horizontalHeaderItem(j)->text() == columnName_1) {
                    targetColumnIndex_1 = j;
                }
                else if(tableWidget->horizontalHeaderItem(j)->text() == columnName_2) {
                    targetColumnIndex_2 = j;
                }

                if(targetColumnIndex_1 != -1 && targetColumnIndex_2 != -1)  break;
            }

            for(int i = 0; i < tableWidget->rowCount(); i += 2)
            {
                for(int j = 0; j < tableWidget->columnCount(); j++)
                {
                    if(j < targetColumnIndex_1 || j == targetColumnIndex_2)
                    {
                        tableWidget->setSpan(i, j, 2, 1);
                    }
                }
            }
        }
        else if(deleteRowNum == 4)
        {
            QString columnName_1 = "类型"; // 这是你想合并单元格的列的名称
            QString columnName_2 = "来源"; // 这是你想合并单元格的列的名称
            int targetColumnIndex_1 = -1;
            int targetColumnIndex_2 = -1;

            for(int j = 0; j < tableWidget->columnCount(); ++j) {
                if(tableWidget->horizontalHeaderItem(j)->text() == columnName_1) {
                    targetColumnIndex_1 = j;
                }
                else if(tableWidget->horizontalHeaderItem(j)->text() == columnName_2) {
                    targetColumnIndex_2 = j;
                }

                if(targetColumnIndex_1 != -1 && targetColumnIndex_2 != -1)  break;
            }

            // 找到该列名称对应的列索引
            for(int i = 0; i < tableWidget->rowCount(); i += 4)
            {
                for(int j = 0; j < tableWidget->columnCount(); j++)
                {
                    if(j < targetColumnIndex_1)
                    {
                        tableWidget->setSpan(i, j, 4, 1);
                    }
                    else if(j == targetColumnIndex_1 || j == targetColumnIndex_2)
                    {
                        tableWidget->setSpan(i, j, 2, 1);
                        tableWidget->setSpan(i + 2, j, 2, 1);
                    }
                }
            }
        }
    }
    else if (msgBox.clickedButton() == noButton)
    {
        return;
    }
}

void Widget::deleteRowFromTable(QTableWidget *tableWidget_noise, QTableWidget *tableWidget_atten, QTableWidget *tableWidget_refl, QString componentName)
{
    // 获取选中的行索引
    QList<int> selectedRows;
    QVector<QTableWidget *> tableWidgets= {tableWidget_noise,tableWidget_atten,tableWidget_refl};
    for(int i = 0; i < tableWidgets.size(); i++)
    {
        for (int row = 0; row < tableWidgets[i]->rowCount(); ++row) {
            QWidget* widget = tableWidgets[i]->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
            QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

            if (checkBox && checkBox->isChecked()) {
                selectedRows.append(row);
            }
        }
    }

    // 弹窗确认
    QString confirmationMessage = "确认删除以下行吗？\n";
    for (int i = 0; i < selectedRows.size(); ++i)
    {
        int row = selectedRows[i];
        confirmationMessage += QString::number(row + 1) + "\n"; // 从1开始计数
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("确认删除");
    msgBox.setText(confirmationMessage);
    msgBox.setIcon(QMessageBox::Warning);
    QPushButton *yesButton = msgBox.addButton("确认", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("取消", QMessageBox::NoRole);
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton)
    {

        for (int row : selectedRows)
        {
            tableWidget_noise->removeRow(row);
            tableWidget_atten->removeRow(row);
            tableWidget_refl->removeRow(row);

            componentManager.del_and_updateTableID(row + 1, componentName);
        }


        // 重新编号
        for(int i = 0; i < tableWidgets.size(); i++)
        {
            for (int row = 0; row < tableWidgets[i]->rowCount(); ++row) {
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(row + 1));
                tableWidgets[i]->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
                item->setTextAlignment(Qt::AlignCenter);
                item->setFlags(Qt::ItemIsEditable);
                item->setBackground(QBrush(Qt::lightGray));
                item->setData(Qt::ForegroundRole, QColor(70, 70, 70));
            }
        }
    }
    else if(msgBox.clickedButton() == noButton)
    {
        return;
    }
}

template <typename NoiType, typename DialogType>
void Widget::noiseRevision(QTableWidget *tableWidget, int row,NoiType *& noi, QVector<QString*>& items, int* cols, int table_id_col, QString& table_id, QString name)
{
    QWidget* widget = tableWidget->cellWidget(row, 0);
    QCheckBox* checkBox = widget->findChild<QCheckBox*>();

    if(checkBox && checkBox->isChecked())
    {
        for(int i = 0; i < items.size(); i++)
        {
            *(items[i]) = tableWidget->item(row,cols[i])->text();
        }
    }
    else
    {
        return;
    }

    table_id = tableWidget->item(row, table_id_col)->text();

    // 创建模态对话框，并设置为模态
    DialogType* dialog;
    if(name != "")
    {
        dialog = new DialogType(name,this,row,*noi);
    }
    else
    {
        dialog = new DialogType(this,row,*noi);
    }
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->setModal(true);

    // 显示对话框,等待用户操作
    if (dialog->exec() == QDialog::Accepted)
    {
        // 使用 static_cast 将 void* 转换为期望的类型
        NoiType* dialogNoi = static_cast<NoiType*>(dialog->getNoi());
        // 直接通过引用更新 'noi' 对象的内容
        dialogNoi->table_id = table_id;
        *noi = *dialogNoi;
        for(int i = 0; i < items.size(); i++)
        {
            tableWidget->item(row,cols[i])->setText(*(items[i]));
        }
        delete dialogNoi;  // 删除 dialogNoi，而不是 noi
    }
    delete dialog;
}

template <typename NoiType, typename DialogType>
void Widget::noiseRevision(QTableWidget *currentTableWidget, QTableWidget *tableWidget_noise, QTableWidget *tableWidget_atten, QTableWidget *tableWidget_refl,
                   int row,NoiType *& noi, QVector<QString*>& items_noise, QVector<QString*>& items_atten, QVector<QString*>& items_refl,
                   int* cols_noise, int* cols_atten, int* cols_refl, int table_id_col, QString& table_id, QString name)
{
    QVector<QTableWidget *> tableWidgets = {tableWidget_noise, tableWidget_atten, tableWidget_refl};
    QVector<QVector<QString*>> items = {items_noise, items_atten, items_refl};
    QVector<int*> cols = {cols_noise, cols_atten, cols_refl};

    QWidget* widget = currentTableWidget->cellWidget(row, 0);
    QCheckBox* checkBox = widget->findChild<QCheckBox*>();

    if(checkBox && checkBox->isChecked())
    {
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < items[i].size(); j++)
            {
                *(items[i][j]) = tableWidgets[i]->item(row,cols[i][j])->text();
            }
        }
    }
    else
    {
        return;
    }

    table_id = (tableWidget_noise->item(row, table_id_col)->text());

    // 创建模态对话框，并设置为模态
    DialogType* dialog;
    if(name != "")
    {
        dialog = new DialogType(name,this,row,*noi);
    }
    else
    {
        dialog = new DialogType(this,row,*noi);
    }
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->setModal(true);

    // 显示对话框,等待用户操作
    if (dialog->exec() == QDialog::Accepted)
    {
        // 使用 static_cast 将 void* 转换为期望的类型
        NoiType* dialogNoi = static_cast<NoiType*>(dialog->getNoi());
        // 直接通过引用更新 'noi' 对象的内容
        dialogNoi->table_id = table_id;
        *noi = *dialogNoi;
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < items[i].size(); j++)
            {
                tableWidgets[i]->item(row,cols[i][j])->setText(*(items[i][j]));
            }
        }
        delete dialogNoi;  // 删除 dialogNoi，而不是 noi
    }
    delete dialog;  // 只在这里删除对话框，而不是 noi
}

void Widget::mergeSimilarCellsInColumn(QTableWidget* tableWidget, int column, int startRow, int numRows, int rowsPerGroup) {
    if (!tableWidget || column < 0 || column >= tableWidget->columnCount()) return;

    int rowCount = tableWidget->rowCount();

    // 确保不超过表格的总行数
    int endRow = std::min(startRow + numRows, rowCount);

    // 当不使用分组（rowsPerGroup <= 0）时，将整个范围作为一个大组处理
    if (rowsPerGroup <= 0) {
        rowsPerGroup = endRow - startRow;
    }

    for (int groupStartRow = startRow; groupStartRow < endRow; groupStartRow += rowsPerGroup) {
        // 计算当前组的结束行，确保不会超出范围
        int groupEndRow = std::min(groupStartRow + rowsPerGroup, endRow);

        QString lastValue;
        int spanStartRow = groupStartRow;

        for (int row = groupStartRow; row < groupEndRow; ++row) {
            QTableWidgetItem* item = tableWidget->item(row, column);
            QString currentValue = item ? item->text() : QString();

            if (row == groupStartRow) {
                lastValue = currentValue;
                continue;
            }

            if (currentValue == lastValue) {
                // 如果是组内的最后一行，合并
                if (row == groupEndRow - 1) {
                    tableWidget->setSpan(spanStartRow, column, row - spanStartRow + 1, 1);
                }
                continue;
            }

            // 当前行和上一行不同，且有超过一行相同
            if (row - spanStartRow > 1) {
                // 合并这些行
                tableWidget->setSpan(spanStartRow, column, row - spanStartRow, 1);
            }

            // 更新上一行的值和起始行位置，为下一组做准备
            lastValue = currentValue;
            spanStartRow = row;
        }
    }
}
#pragma endregion }
/**********输入界面表格初始化及其他按钮设置**********/





/**********主界面**********/
#pragma region "main" {

//初始化列表
void Widget::initializeTreeWidget()
{
    ui->treeWidget->clear();    //先清空
    item_prj = new QTreeWidgetItem(QStringList("工程-" + project.prj_name));  //工程名
    ui->treeWidget->setHeaderItem(item_prj);
    item_prj_info = new QTreeWidgetItem(QStringList("1.项目信息"));   //工程信息

    //输出模块
    item_sound_sorce_noise = new QTreeWidgetItem(QStringList("2.声源噪音"));   //1音源噪音
    item_fan_noise = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("风机噪音"));   //1.1风机噪音
    item_fan_coil_noise = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("风机盘管噪音"));   //1.2风机盘管噪音
    item_aircondition_noise_single_fan = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("空调器噪音(单风机)"));   //1.3空调器噪音(单风机)
    item_aircondition_noise_double_fan = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("空调器噪音(双风机)"));   //1.3空调器噪音(双风机)

    item_pipe_and_acce_airflow_noise = new QTreeWidgetItem(QStringList("3.管路及附件气流噪音"));            //2.管路及附件气流噪音
    item_valve_noise = new QTreeWidgetItem(item_pipe_and_acce_airflow_noise,QStringList("阀门噪音"));                                  //2.1 阀门噪音
    item_VAV_terminal = new QTreeWidgetItem(item_valve_noise,QStringList("变风量末端"));                                  //2.1.1 变风量末端
    item_circular_damper = new QTreeWidgetItem(item_valve_noise,QStringList("圆形调风门"));                                  //2.1.2 圆形调风门
    item_rect_damper = new QTreeWidgetItem(item_valve_noise,QStringList("方形调风门"));                                      //2.1.3 方形调风门
    item_terminal_airflow_noise = new QTreeWidgetItem(item_pipe_and_acce_airflow_noise,QStringList("末端气流噪声"));                           //2.2 末端气流噪声
    item_air_diff = new QTreeWidgetItem(item_terminal_airflow_noise,QStringList("布风器+散流器"));                                         //2.2.1 布风器+散流器
    item_pump_send_tuyere = new QTreeWidgetItem(item_terminal_airflow_noise,QStringList("抽/送风头"));                         //2.2.2 抽/送风头
    item_staticBox_grille = new QTreeWidgetItem(item_terminal_airflow_noise,QStringList("静压箱+格栅"));                            //2.2.3 回风箱+格栅
    item_disp_vent_terminal = new QTreeWidgetItem(item_terminal_airflow_noise,QStringList("置换通风末端"));                //2.2.4 置换通风末端
    item_other_send_terminal = new QTreeWidgetItem(item_terminal_airflow_noise,QStringList("其他送风末端"));                //2.2.5 静压箱孔板送风

    item_noise_atten_in_pipe_acce = new QTreeWidgetItem(QStringList("4.管路及附件噪音衰减"));                //3. 管路及附件噪音衰减
    item_branch_atten = new QTreeWidgetItem(item_noise_atten_in_pipe_acce,QStringList("分支衰减"));                             //3.1 分支衰减
    item_static_box = new QTreeWidgetItem(item_branch_atten,QStringList("静压箱"));                               //3.1.1 静压箱
    item_duct_with_multi_ranc = new QTreeWidgetItem(item_branch_atten,QStringList("风道多分支"));                     //3.1.2 风道多分支
    item_tee_atten = new QTreeWidgetItem(item_branch_atten,QStringList("三通衰减"));                                //3.1.3 三通衰减
    item_pipe_atten = new QTreeWidgetItem(item_noise_atten_in_pipe_acce,QStringList("直管衰减"));                                //3.2 直管衰减
    item_elbow_atten = new QTreeWidgetItem(item_noise_atten_in_pipe_acce,QStringList("弯头衰减"));                                //3.3 弯头衰减
    item_reducer_atten = new QTreeWidgetItem(item_noise_atten_in_pipe_acce,QStringList("变径衰减"));                                //3.4 变径衰减
    item_silencer_atten = new QTreeWidgetItem(item_noise_atten_in_pipe_acce,QStringList("消音器衰减"));                                //3.5 消音器衰减

    item_terminal_atten = new QTreeWidgetItem(item_noise_atten_in_pipe_acce,QStringList("末端衰减"));                                //3.6 末端衰减
    item_air_diff_terminal_atten = new QTreeWidgetItem(item_terminal_atten,QStringList("布风器+散流器"));
    item_pump_send_tuyere_terminal_atten = new QTreeWidgetItem(item_terminal_atten,QStringList("抽/送风头"));
    item_staticBox_grille_terminal_atten = new QTreeWidgetItem(item_terminal_atten,QStringList("静压箱+格栅"));
    item_disp_vent_terminal_atten = new QTreeWidgetItem(item_terminal_atten,QStringList("置换通风末端"));
    item_other_send_terminal_atten = new QTreeWidgetItem(item_terminal_atten,QStringList("其他送风末端"));

    item_terminal_refl_atten = new QTreeWidgetItem(item_noise_atten_in_pipe_acce,QStringList("末端反射衰减")); //3.7 末端反射衰减
    item_air_diff_relf_atten = new QTreeWidgetItem(item_terminal_refl_atten,QStringList("布风器+散流器"));
    item_pump_send_tuyere_relf_atten = new QTreeWidgetItem(item_terminal_refl_atten,QStringList("抽/送风头"));
    item_staticBox_grille_relf_atten = new QTreeWidgetItem(item_terminal_refl_atten,QStringList("静压箱+格栅"));
    item_disp_vent_relf_atten = new QTreeWidgetItem(item_terminal_refl_atten,QStringList("置换通风末端"));
    item_other_send_relf_atten = new QTreeWidgetItem(item_terminal_refl_atten,QStringList("其他送风末端"));

    item_room_atten = new QTreeWidgetItem(QStringList("5.声压级计算类型"));                                   //5. 房间衰减
    item_room_less425 = new QTreeWidgetItem(item_room_atten,QStringList("<425m³的房间(点噪声源)"));      //5.1 体积小于425m³的房间（点噪声源）
    item_room_more425 = new QTreeWidgetItem(item_room_atten,QStringList(">425m³的房间(点噪声源)"));       //5.2 体积大于425m³的房间（点噪声源）
    item_room_noFurniture = new QTreeWidgetItem(item_room_atten,QStringList("无家具房间(点噪声源)"));    //5.3 无家具房间（点噪声源）
    item_room_open = new QTreeWidgetItem(item_room_atten,QStringList("室外开敞住所(点噪声源)"));           //5.4 室外开敞住所（点噪声源）
    item_room_gap_tuyere = new QTreeWidgetItem(item_room_atten,QStringList("条缝风口房间(线噪声源)"));     //5.5 条缝风口房间（线噪声源）
    item_room_rain = new QTreeWidgetItem(item_room_atten,QStringList("雨降风口房间(面噪声源)"));           //5.6 雨降风口房间（面噪声源）

    item_system_list = new QTreeWidgetItem(QStringList("6.系统清单"));                                    //6.系统清单
    item_room_define = new QTreeWidgetItem(QStringList("7.计算房间"));                                    //7.计算房间
    item_room_calculate = new QTreeWidgetItem(QStringList("8.噪音计算"));                                 //8.噪音计算
    item_cabin_classic = new QTreeWidgetItem(item_room_calculate,QStringList("典型住舱"));                                     //8.1典型住舱

    item_report = new QTreeWidgetItem(QStringList("9.报表"));        // 9.报表
    item_report_cover = new QTreeWidgetItem(item_report,QStringList("封面"));        // 9.1封面
    item_report_dictionary = new QTreeWidgetItem(item_report,QStringList("目录"));        // 9.2目录
    item_report_overview = new QTreeWidgetItem(item_report,QStringList("项目概述"));        // 9.3项目概述
    item_report_noise_require = new QTreeWidgetItem(item_report,QStringList("噪音要求"));        // 9.4噪音要求
    item_report_noise_require_basis = new QTreeWidgetItem(item_report_noise_require,QStringList("要求来源依据"));        // 9.4噪音要求
    item_report_noise_require_table = new QTreeWidgetItem(item_report_noise_require,QStringList("噪音要求表格"));        // 9.4噪音要求
    item_report_system_list = new QTreeWidgetItem(item_report,QStringList("系统清单"));        // 9.5系统清单
    item_report_cal_room = new QTreeWidgetItem(item_report,QStringList("计算房间"));        // 9.6计算房间
    item_report_room_choose_basis = new QTreeWidgetItem(item_report_cal_room,QStringList("房间选择依据"));        // 9.6房间选择依据
    item_report_cal_room_table = new QTreeWidgetItem(item_report_cal_room,QStringList("计算房间表格"));        // 9.6计算房间表格
    item_report_cal_summarize = new QTreeWidgetItem(item_report,QStringList("计算结果汇总"));        // 9.7计算结果汇总
    item_report_cal_detaile = new QTreeWidgetItem(item_report,QStringList("舱室噪音详细计算"));        // 9.8舱室噪音详细计算

    ui->treeWidget->addTopLevelItem(item_prj_info);     //工程信息
    ui->treeWidget->addTopLevelItem(item_sound_sorce_noise);    //音源噪音
    ui->treeWidget->addTopLevelItem(item_pipe_and_acce_airflow_noise);    //管路及附件气流噪音
    ui->treeWidget->addTopLevelItem(item_noise_atten_in_pipe_acce);    //管路及附件噪音衰减
    ui->treeWidget->addTopLevelItem(item_terminal_refl_atten);    //末端反射衰减
    ui->treeWidget->addTopLevelItem(item_room_atten);    //房间衰减
    ui->treeWidget->addTopLevelItem(item_system_list);    //系统清单
    ui->treeWidget->addTopLevelItem(item_room_define);   //计算房间
    ui->treeWidget->addTopLevelItem(item_room_calculate);   //噪音计算
    //ui->treeWidget->addTopLevelItem(item_report);   //报表

    // 设置子项为展开状态
    item_prj->setExpanded(true); // 这一行将子项设置为展开状态
}

//工程管理按钮
void Widget::on_pushButto_prj_manage_clicked()
{
    Dialog_prj_manager *dialog = new Dialog_prj_manager(this);

    // 连接新的信号和槽
    connect(dialog, SIGNAL(createProjectClicked(QString)), this, SLOT(onCreateProjectClicked(QString)));

    if(ui->stackedWidget->currentWidget() != ui->page_prj_info)
        ui->stackedWidget->setCurrentWidget(ui->page_prj_info);

    if (dialog->exec() == QDialog::Accepted)
    {
        // 如果对话框返回 Accepted，可以在这里执行一些操作
    }
}

void Widget::onCreateProjectClicked(QString projectName)
{
    ui->lineEdit_boat_num->setText(projectName);
    initializeTreeWidget();
}

void Widget::on_pushButton_noi_limit_add_clicked()
{
    int rowCount = ui->tableWidget_noi_limit->rowCount();
    ui->tableWidget_noi_limit->setRowCount(rowCount + 1);
    // 处理复选框
    QCheckBox* checkBox = new QCheckBox();
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(checkBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    ui->tableWidget_noi_limit->setCellWidget(rowCount, 0, widget);

    //设置序号
    QTableWidgetItem *item = new QTableWidgetItem(QString::number(rowCount + 1));
    ui->tableWidget_noi_limit->setItem(rowCount, 1, item);
    item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
    item->setFlags(Qt::ItemIsEditable); // 设置为只读
}

void Widget::on_pushButton_noi_limit_del_clicked()
{
    // 获取选中的行索引
    QList<int> selectedRows;
    QList<QString> selectedNames;
    for (int row = 0; row < ui->tableWidget_noi_limit->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_noi_limit->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox && checkBox->isChecked()) {
            selectedRows.append(row);
            QTableWidgetItem* item = ui->tableWidget_noi_limit->item(row, 2);
            if (item != nullptr)
            { // 确保item不为nullptr
                selectedNames.append(item->text());
            }
        }
    }

    // 弹窗确认
    QString confirmationMessage = "确认删除以下行吗？\n";
    for (int i = 0; i < selectedRows.size(); ++i)
    {
        int row = selectedRows[i];
        confirmationMessage += QString::number(row + 1) + "\n"; // 从1开始计数
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("确认删除");
    msgBox.setText(confirmationMessage);
    msgBox.setIcon(QMessageBox::Warning);
    QPushButton *yesButton = msgBox.addButton("确认", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("取消", QMessageBox::NoRole);
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton)
    {
        for (int i = selectedRows.size() - 1; i >= 0; --i)
        {
            int row = selectedRows[i];
            ui->tableWidget_noi_limit->removeRow(row);
        }

        for (const QString& name : selectedNames)
        {
            auto it = std::remove_if(rooms.begin(), rooms.end(),
                                     [&name](const Room& room) {
                                         return room.name == name;
                                     });
            rooms.erase(it, rooms.end()); // 实际删除 those elements
        }


        // 重新编号
        for (int row = 0; row < ui->tableWidget_noi_limit->rowCount(); ++row) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(row + 1));
            ui->tableWidget_noi_limit->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(Qt::ItemIsEditable);
        }
    }
}

void Widget::on_pushButton_drawing_list_add_clicked()
{
    int rowCount = ui->tableWidget_drawing_list->rowCount();
    ui->tableWidget_drawing_list->setRowCount(rowCount + 1);
    // 处理复选框
    QCheckBox* checkBox = new QCheckBox();
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(checkBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    ui->tableWidget_drawing_list->setCellWidget(rowCount, 0, widget);

    //设置序号
    QTableWidgetItem *item = new QTableWidgetItem(QString::number(rowCount + 1));
    ui->tableWidget_drawing_list->setItem(rowCount, 1, item);
    item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
    item->setFlags(Qt::ItemIsEditable); // 设置为只读
}

void Widget::on_pushButton_drawing_list_del_clicked()
{
    // 获取选中的行索引
    QList<int> selectedRows;
    for (int row = 0; row < ui->tableWidget_drawing_list->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_drawing_list->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox && checkBox->isChecked()) {
            selectedRows.append(row);
        }
    }

    // 弹窗确认
    QString confirmationMessage = "确认删除以下行吗？\n";
    for (int i = 0; i < selectedRows.size(); ++i)
    {
        int row = selectedRows[i];
        confirmationMessage += QString::number(row + 1) + "\n"; // 从1开始计数
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("确认删除");
    msgBox.setText(confirmationMessage);
    msgBox.setIcon(QMessageBox::Warning);
    QPushButton *yesButton = msgBox.addButton("确认", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("取消", QMessageBox::NoRole);
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton)
    {
        for (int i = selectedRows.size() - 1; i >= 0; --i)
        {
            int row = selectedRows[i];
            ui->tableWidget_drawing_list->removeRow(row);
        }


        // 重新编号
        for (int row = 0; row < ui->tableWidget_drawing_list->rowCount(); ++row) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(row + 1));
            ui->tableWidget_drawing_list->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(Qt::ItemIsEditable);
        }
    }
}

void Widget::on_pushButton_prj_revise_clicked()
{
    ui->lineEdit_boat_num->setReadOnly(false);
    ui->lineEdit_shipyard->setReadOnly(false);
    ui->lineEdit_principal->setReadOnly(false);
    ui->lineEdit_prj_num->setReadOnly(false);
    ui->lineEdit_prj_name->setReadOnly(false);

    ui->lineEdit_boat_num->setStyleSheet("#QLineEdit{}");
    ui->lineEdit_shipyard->setStyleSheet("#QLineEdit{}");
    ui->lineEdit_principal->setStyleSheet("#QLineEdit{}");
    ui->lineEdit_prj_num->setStyleSheet("#QLineEdit{}");
    ui->lineEdit_prj_name->setStyleSheet("#QLineEdit{}");
}

void Widget::on_pushButton_prj_save_clicked()
{
    ui->lineEdit_boat_num->setReadOnly(true);
    ui->lineEdit_shipyard->setReadOnly(true);
    ui->lineEdit_principal->setReadOnly(true);
    ui->lineEdit_prj_num->setReadOnly(true);
    ui->lineEdit_prj_name->setReadOnly(true);

    ui->lineEdit_boat_num->setStyleSheet("#lineEdit_boat_num{background-color: rgb(240, 240, 240); border: 1px solid #9C9C9C;}");
    ui->lineEdit_shipyard->setStyleSheet("#lineEdit_shipyard{background-color: rgb(240, 240, 240); border: 1px solid #9C9C9C;}");
    ui->lineEdit_principal->setStyleSheet("#lineEdit_principal{background-color: rgb(240, 240, 240); border: 1px solid #9C9C9C;}");
    ui->lineEdit_prj_num->setStyleSheet("#lineEdit_prj_num{background-color: rgb(240, 240, 240); border: 1px solid #9C9C9C;}");
    ui->lineEdit_prj_name->setStyleSheet("#lineEdit_prj_name{background-color: rgb(240, 240, 240); border: 1px solid #9C9C9C;}");
}

//初始化表格
void Widget::initTableWidget_project_attachment()
{
    int colCount = 3;
    // 设置表头标题
    QStringList headerText;
    headerText << "" << "序号" << "附件名";
    // 设置每列的宽度
    int columnWidths[] = {1, 1, 5};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_project_attachment, headerText, columnWidths, colCount);
}

//项目附件添加
void Widget::on_pushButton_project_attachment_add_clicked()
{
    // 打开文件选择对话框，让用户选择文件
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*)"));
    QString fileName = "";
    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);

        // 保存文件名（包括后缀）
        fileName = fileInfo.fileName();

        projectAttachmentMap[fileName] = filePath;
    }

    QTableWidget *tableWidget = ui->tableWidget_project_attachment;

    QStringList data = {
        QString::number(tableWidget->rowCount() + 1),
        fileName
    };

    int rowCount = tableWidget->rowCount();
    tableWidget->setRowCount(rowCount + 1);

    for (int i = 0; i < data.size() + 1; ++i) {
        if (i == 0) {
            // 处理复选框
            QCheckBox* checkBox = new QCheckBox();
            QWidget* widget = new QWidget();
            QHBoxLayout* layout = new QHBoxLayout(widget);
            layout->addWidget(checkBox);
            layout->setAlignment(Qt::AlignCenter);
            layout->setContentsMargins(0, 0, 0, 0);
            tableWidget->setCellWidget(rowCount, i, widget);
        }
        else if (i == 1)
        {
            QTableWidgetItem *item = new QTableWidgetItem(data[i - 1]);
            tableWidget->setItem(rowCount, i, item);
            item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
            item->setFlags(Qt::ItemIsEditable); // 设置为只读
        }
        else if (i == 2)
        {
            QLabel *label = new QLabel("<a href='#'>" + fileName + "</a>");
            label->setTextFormat(Qt::RichText);
            label->setCursor(Qt::PointingHandCursor); // 设置鼠标悬停时为手形光标
            label->setAlignment(Qt::AlignCenter);
            tableWidget->setCellWidget(rowCount, 2, label);

            connect(label, &QLabel::linkActivated, [=](const QString &link){
                // 处理点击事件，例如打开链接
                QString localFilePath = projectAttachmentMap[fileName]; // 假设这是你的本地文件路径
                QUrl fileUrl = QUrl::fromLocalFile(localFilePath);
                QDesktopServices::openUrl(fileUrl);
            });
        }
    }

}

void Widget::initTableWidget_noi_limit()
{
    int colCount = 5;
    // 设置表头标题
    QStringList headerText;
    headerText << "" << "序号" << "房间类型" << "噪声限值dB(A)" << "处所类型";
    // 设置每列的宽度
    int columnWidths[] = {30, 38, 130, 90, 130};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_noi_limit, headerText, columnWidths, colCount);
    colCount = 4;
    QStringList headerText1;
    headerText1 << "序号" << "房间类型" << "噪声限值dB(A)" << "处所类型";
    int columnWidths1[] = {38, 130, 90, 130};
    //初始化报表部分的表格
    initTableWidget(ui->tableWidget_noise_require, headerText1, columnWidths1, colCount);
}

void Widget::initTableWidget_drawing_list()
{
    int colCount = 4;
    // 设置表头标题
    QStringList headerText;
    headerText << "" << "序号" << "图号" << "图名";
    // 设置每列的宽度
    int columnWidths[] = {1, 2, 9, 9};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_drawing_list, headerText, columnWidths, colCount);
}

//当表格item改变，rooms跟着改变
void Widget::on_tableWidget_noi_limit_itemChanged(QTableWidgetItem *item)
{
    if (!item || item->text().isEmpty())
        return;

    int rowCount = ui->tableWidget_noi_limit->rowCount();
    QVector<Room> tempRooms; // 使用临时列表来收集数据

    for (int i = 0; i < rowCount; i++) {
        if (!ui->tableWidget_noi_limit->item(i, 2) || ui->tableWidget_noi_limit->item(i, 2)->text().isEmpty() ||
            !ui->tableWidget_noi_limit->item(i, 3) || ui->tableWidget_noi_limit->item(i, 3)->text().isEmpty() ||
            !ui->tableWidget_noi_limit->item(i, 4) || ui->tableWidget_noi_limit->item(i, 4)->text().isEmpty()) {
            // 如果任一必需的单元格为空，则跳过这一行
            continue;
        }
        Room room;
        room.name = ui->tableWidget_noi_limit->item(i, 2)->text();
        room.noise = ui->tableWidget_noi_limit->item(i, 3)->text();
        room.type = ui->tableWidget_noi_limit->item(i, 4)->text();
        tempRooms.push_back(room); // 将这个房间添加到临时列表中
    }

    rooms = tempRooms; // 使用收集到的数据更新rooms列表
}

//通过名字获取噪声限制
double Widget::getNoiseLimitByName(const QString& name)
{
    for(int i = 0; i < rooms.size(); i++)
    {
        if(rooms[i].name == name)
        {
            return rooms[i].noise.toDouble();
        }
    }
    return -1;
}

//输入模块按钮
void Widget::on_pushButton_input_clicked()
{
    if(ui->stackedWidget->currentWidget() == ui->page_noise_src)
        return;
    ui->stackedWidget->setCurrentWidget(ui->page_noise_src);
}

//登录按钮
void Widget::on_pushButton_start_clicked()
{
    this->on_pushButto_prj_manage_clicked();
}

#pragma endregion }
/**********主界面**********/





/**********风机噪音**********/
#pragma region "stack_fan_noi" {

//初始化表格
void Widget::initTableWidget_fan_noi()
{
    int colCount = 18;
    // 设置表头标题
    QStringList headerText;
    headerText << "" << "序号" << "编号" << "型号" << "风量\n(m³/h)" << "静压\n(Pa)"  << "品牌" << "噪音\n位置" << "63Hz\n(dB)"
               << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
               << "8kHz\n(dB)" << "总值\ndB(A)" << "来源";
    // 设置每列的宽度
    int columnWidths[] = {30, 38, 120, 120, 55, 55, 60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_fan_noi, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_fanNoi_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_fan_noi;
    int rowCount = tableWidget->rowCount(); //获取当前行数
    std::unique_ptr<Fan_noise> noi;
    Dialog_fan_noise *dialog = new Dialog_fan_noise(this);

    if (dialog->exec() == QDialog::Accepted) {
        // 使用 std::make_unique 创建 std::unique_ptr
        noi = std::make_unique<Fan_noise>(std::move(*static_cast<Fan_noise*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() / 2 + 1);
        if (noi != nullptr) {
            QStringList data_in = {
                noi->table_id,
                noi->number,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
                noi->brand,
                "进口",
                noi->noi_in_63,
                noi->noi_in_125,
                noi->noi_in_250,
                noi->noi_in_500,
                noi->noi_in_1k,
                noi->noi_in_2k,
                noi->noi_in_4k,
                noi->noi_in_8k,
                noi->noi_in_total,
                "厂家信息"
            };
            QStringList data_out = {
                noi->table_id,
                noi->number,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
                noi->brand,
                "出口",
                noi->noi_out_63,
                noi->noi_out_125,
                noi->noi_out_250,
                noi->noi_out_500,
                noi->noi_out_1k,
                noi->noi_out_2k,
                noi->noi_out_4k,
                noi->noi_out_8k,
                noi->noi_out_total,
                "厂家"
            };
            // 使用通用函数添加行
            addRowToTable(tableWidget, data_in);
            addRowToTable(tableWidget, data_out);

            componentManager.addComponent(QSharedPointer<Fan_noise>(noi.release()),"风机");
            for(int i = 0; i < tableWidget->columnCount(); i++)
            {
                if(i < 7 || i > 16)
                {
                    tableWidget->setSpan(rowCount, i, 2, 1);
                }
            }
        }
    }
}

void Widget::on_pushButton_fanNoi_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_fan_noi, 2, "风机");
}

//修改按钮
void Widget::on_pushButton_fanNoi_revise_clicked()
{
    int colCount = 18;
    QString table_id = "";
    for (int row = 0; row < ui->tableWidget_fan_noi->rowCount(); ++row) {

        QWidget* widget = ui->tableWidget_fan_noi->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked()) {
            Fan_noise *noi = new Fan_noise();
            noi->number = ui->tableWidget_fan_noi->item(row - 1,2)->text();
            table_id = noi->table_id;
            noi->model = ui->tableWidget_fan_noi->item(row - 1,3)->text();
            noi->air_volume = ui->tableWidget_fan_noi->item(row - 1,4)->text();
            noi->static_pressure = ui->tableWidget_fan_noi->item(row - 1,5)->text();
            noi->brand = ui->tableWidget_fan_noi->item(row - 1,6)->text();

            noi->noi_in_63 = ui->tableWidget_fan_noi->item(row - 1,8)->text();
            noi->noi_in_125 = ui->tableWidget_fan_noi->item(row - 1,9)->text();
            noi->noi_in_250 = ui->tableWidget_fan_noi->item(row - 1,10)->text();
            noi->noi_in_500 = ui->tableWidget_fan_noi->item(row - 1,11)->text();
            noi->noi_in_1k = ui->tableWidget_fan_noi->item(row - 1,12)->text();
            noi->noi_in_2k = ui->tableWidget_fan_noi->item(row - 1,13)->text();
            noi->noi_in_4k = ui->tableWidget_fan_noi->item(row - 1,14)->text();
            noi->noi_in_8k = ui->tableWidget_fan_noi->item(row - 1,15)->text();
            noi->noi_in_total = ui->tableWidget_fan_noi->item(row - 1,16)->text();

            noi->noi_out_63 = ui->tableWidget_fan_noi->item(row,8)->text();
            noi->noi_out_125 = ui->tableWidget_fan_noi->item(row,9)->text();
            noi->noi_out_250 = ui->tableWidget_fan_noi->item(row,10)->text();
            noi->noi_out_500 = ui->tableWidget_fan_noi->item(row,11)->text();
            noi->noi_out_1k = ui->tableWidget_fan_noi->item(row,12)->text();
            noi->noi_out_2k = ui->tableWidget_fan_noi->item(row,13)->text();
            noi->noi_out_4k = ui->tableWidget_fan_noi->item(row,14)->text();
            noi->noi_out_8k = ui->tableWidget_fan_noi->item(row,15)->text();
            noi->noi_out_total = ui->tableWidget_fan_noi->item(row,16)->text();
            // 创建模态对话框，并设置为模态
            Dialog_fan_noise *fanNoiseDialog = new Dialog_fan_noise(this,row,*noi);
            fanNoiseDialog->setWindowModality(Qt::ApplicationModal);
            fanNoiseDialog->setModal(true);

            // 显示对话框,等待用户操作
            if (fanNoiseDialog->exec() == QDialog::Accepted)
            {
                noi = static_cast<Fan_noise*>(fanNoiseDialog->getNoi());
                noi->table_id = table_id;
                ui->tableWidget_fan_noi->item(row - 1,2)->setText(noi->number);
                ui->tableWidget_fan_noi->item(row - 1,3)->setText(noi->model);
                ui->tableWidget_fan_noi->item(row - 1,4)->setText(noi->air_volume);
                ui->tableWidget_fan_noi->item(row - 1,5)->setText(noi->static_pressure);
                ui->tableWidget_fan_noi->item(row - 1,6)->setText(noi->brand);

                ui->tableWidget_fan_noi->item(row - 1,8)->setText(noi->noi_in_63);
                ui->tableWidget_fan_noi->item(row - 1,9)->setText(noi->noi_in_125);
                ui->tableWidget_fan_noi->item(row - 1,10)->setText(noi->noi_in_250);
                ui->tableWidget_fan_noi->item(row - 1,11)->setText(noi->noi_in_500);
                ui->tableWidget_fan_noi->item(row - 1,12)->setText(noi->noi_in_1k);
                ui->tableWidget_fan_noi->item(row - 1,13)->setText(noi->noi_in_2k);
                ui->tableWidget_fan_noi->item(row - 1,14)->setText(noi->noi_in_4k);
                ui->tableWidget_fan_noi->item(row - 1,15)->setText(noi->noi_in_8k);
                ui->tableWidget_fan_noi->item(row - 1,16)->setText(noi->noi_in_total);

                ui->tableWidget_fan_noi->item(row,2)->setText(noi->number);
                ui->tableWidget_fan_noi->item(row,3)->setText(noi->model);
                ui->tableWidget_fan_noi->item(row,4)->setText(noi->air_volume);
                ui->tableWidget_fan_noi->item(row,5)->setText(noi->static_pressure);
                ui->tableWidget_fan_noi->item(row,6)->setText(noi->brand);

                ui->tableWidget_fan_noi->item(row,8)->setText(noi->noi_out_63);
                ui->tableWidget_fan_noi->item(row,9)->setText(noi->noi_out_125);
                ui->tableWidget_fan_noi->item(row,10)->setText(noi->noi_out_250);
                ui->tableWidget_fan_noi->item(row,11)->setText(noi->noi_out_500);
                ui->tableWidget_fan_noi->item(row,12)->setText(noi->noi_out_1k);
                ui->tableWidget_fan_noi->item(row,13)->setText(noi->noi_out_2k);
                ui->tableWidget_fan_noi->item(row,14)->setText(noi->noi_out_4k);
                ui->tableWidget_fan_noi->item(row,15)->setText(noi->noi_out_8k);
                ui->tableWidget_fan_noi->item(row,16)->setText(noi->noi_out_total);
            }

            componentManager.updateRevisedComponent(noi->table_id, QSharedPointer<Fan_noise>(new Fan_noise(*noi)),"风机");
            delete noi;
        }
    }
}
#pragma endregion }
/**********风机噪音**********/





/**********风机盘管噪音**********/
#pragma region "stack_fanCoil_noi"{

//初始化表格
void Widget::initTableWidget_fanCoil_noi()
{
    int colCount = 18;
    QStringList headerText;
    headerText << "" << "序号" << "类型" << "型号" << "风量\n(m³/h)" << "静压\n(Pa)" << "品牌" << "噪音\n位置" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
               << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "总值\ndB(A)" << "来源";
    int columnWidths[] = {30, 40, 90, 150, 55, 55, 95, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 55};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_fanCoil_noi, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_fanCoil_noi_add_clicked()
{
    //修改
    QTableWidget *tableWidget = ui->tableWidget_fanCoil_noi;
    Dialog_fanCoil_noise *dialog = new Dialog_fanCoil_noise(this);
    std::unique_ptr<FanCoil_noise> noi;

    int rowCount = tableWidget->rowCount(); //获取当前行数
    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<FanCoil_noise>(std::move(*static_cast<FanCoil_noise*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() / 2 + 1);
        if (noi != nullptr) {
            QStringList data_in = {
                noi->table_id,
                noi->type,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
                noi->brand,
                "进口",
                noi->noi_in_63,
                noi->noi_in_125,
                noi->noi_in_250,
                noi->noi_in_500,
                noi->noi_in_1k,
                noi->noi_in_2k,
                noi->noi_in_4k,
                noi->noi_in_8k,
                noi->noi_in_total,
                "厂家"
            };
            QStringList data_out = {
                noi->table_id,
                noi->type,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
                noi->brand,
                "出口",
                noi->noi_out_63,
                noi->noi_out_125,
                noi->noi_out_250,
                noi->noi_out_500,
                noi->noi_out_1k,
                noi->noi_out_2k,
                noi->noi_out_4k,
                noi->noi_out_8k,
                noi->noi_out_total,
                "厂家"
            };
            // 使用通用函数添加行
            addRowToTable(tableWidget, data_in);
            addRowToTable(tableWidget, data_out);

            componentManager.addComponent(QSharedPointer<FanCoil_noise>(noi.release()),"风机盘管");
            for(int i = 0; i < tableWidget->columnCount(); i++)
            {
                if(i < 7 || i > 16)
                {
                    tableWidget->setSpan(rowCount, i, 2, 1);
                }
            }
        }
    }
}


void Widget::on_pushButton_fanCoil_noi_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_fanCoil_noi, 2, "风机盘管");
}

//修改按钮
void Widget::on_pushButton_fanCoil_noi_revise_clicked()
{
    QString table_id = "";
    for (int row = 0; row < ui->tableWidget_fanCoil_noi->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_fanCoil_noi->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked()) {
            FanCoil_noise *noi = new FanCoil_noise();
            table_id = ui->tableWidget_fanCoil_noi->item(row - 1,1)->text();
            noi->type = ui->tableWidget_fanCoil_noi->item(row - 1,2)->text();
            noi->model = ui->tableWidget_fanCoil_noi->item(row - 1,3)->text();
            noi->air_volume = ui->tableWidget_fanCoil_noi->item(row - 1,4)->text();
            noi->static_pressure = ui->tableWidget_fanCoil_noi->item(row - 1,5)->text();
            noi->brand = ui->tableWidget_fanCoil_noi->item(row - 1,6)->text();

            noi->noi_in_63 = ui->tableWidget_fanCoil_noi->item(row - 1,8)->text();
            noi->noi_in_125 = ui->tableWidget_fanCoil_noi->item(row - 1,9)->text();
            noi->noi_in_250 = ui->tableWidget_fanCoil_noi->item(row - 1,10)->text();
            noi->noi_in_500 = ui->tableWidget_fanCoil_noi->item(row - 1,11)->text();
            noi->noi_in_1k = ui->tableWidget_fanCoil_noi->item(row - 1,12)->text();
            noi->noi_in_2k = ui->tableWidget_fanCoil_noi->item(row - 1,13)->text();
            noi->noi_in_4k = ui->tableWidget_fanCoil_noi->item(row - 1,14)->text();
            noi->noi_in_8k = ui->tableWidget_fanCoil_noi->item(row - 1,15)->text();
            noi->noi_in_total = ui->tableWidget_fanCoil_noi->item(row - 1,16)->text();

            noi->noi_out_63 = ui->tableWidget_fanCoil_noi->item(row,8)->text();
            noi->noi_out_125 = ui->tableWidget_fanCoil_noi->item(row,9)->text();
            noi->noi_out_250 = ui->tableWidget_fanCoil_noi->item(row,10)->text();
            noi->noi_out_500 = ui->tableWidget_fanCoil_noi->item(row,11)->text();
            noi->noi_out_1k = ui->tableWidget_fanCoil_noi->item(row,12)->text();
            noi->noi_out_2k = ui->tableWidget_fanCoil_noi->item(row,13)->text();
            noi->noi_out_4k = ui->tableWidget_fanCoil_noi->item(row,14)->text();
            noi->noi_out_8k = ui->tableWidget_fanCoil_noi->item(row,15)->text();
            noi->noi_out_total = ui->tableWidget_fanCoil_noi->item(row,16)->text();
            // 创建模态对话框，并设置为模态
            Dialog_fanCoil_noise *fanCoilNoiseDialog = new Dialog_fanCoil_noise(this,row,*noi);
            fanCoilNoiseDialog->setWindowModality(Qt::ApplicationModal);
            fanCoilNoiseDialog->setModal(true);

            // 显示对话框,等待用户操作
            if (fanCoilNoiseDialog->exec() == QDialog::Accepted)
            {
                noi = static_cast<FanCoil_noise*>(fanCoilNoiseDialog->getNoi());
                noi->table_id = table_id;   //设置table_id
                ui->tableWidget_fanCoil_noi->item(row - 1,2)->setText(noi->type);
                ui->tableWidget_fanCoil_noi->item(row - 1,3)->setText(noi->model);
                ui->tableWidget_fanCoil_noi->item(row - 1,4)->setText(noi->air_volume);
                ui->tableWidget_fanCoil_noi->item(row - 1,5)->setText(noi->static_pressure);
                ui->tableWidget_fanCoil_noi->item(row - 1,6)->setText(noi->brand);

                ui->tableWidget_fanCoil_noi->item(row - 1,8)->setText(noi->noi_in_63);
                ui->tableWidget_fanCoil_noi->item(row - 1,9)->setText(noi->noi_in_125);
                ui->tableWidget_fanCoil_noi->item(row - 1,10)->setText(noi->noi_in_250);
                ui->tableWidget_fanCoil_noi->item(row - 1,11)->setText(noi->noi_in_500);
                ui->tableWidget_fanCoil_noi->item(row - 1,12)->setText(noi->noi_in_1k);
                ui->tableWidget_fanCoil_noi->item(row - 1,13)->setText(noi->noi_in_2k);
                ui->tableWidget_fanCoil_noi->item(row - 1,14)->setText(noi->noi_in_4k);
                ui->tableWidget_fanCoil_noi->item(row - 1,15)->setText(noi->noi_in_8k);
                ui->tableWidget_fanCoil_noi->item(row - 1,16)->setText(noi->noi_in_total);

                ui->tableWidget_fanCoil_noi->item(row,2)->setText(noi->brand);
                ui->tableWidget_fanCoil_noi->item(row,3)->setText(noi->type);
                ui->tableWidget_fanCoil_noi->item(row,4)->setText(noi->model);
                ui->tableWidget_fanCoil_noi->item(row,5)->setText(noi->air_volume);
                ui->tableWidget_fanCoil_noi->item(row,6)->setText(noi->static_pressure);

                ui->tableWidget_fanCoil_noi->item(row,8)->setText(noi->noi_out_63);
                ui->tableWidget_fanCoil_noi->item(row,9)->setText(noi->noi_out_125);
                ui->tableWidget_fanCoil_noi->item(row,10)->setText(noi->noi_out_250);
                ui->tableWidget_fanCoil_noi->item(row,11)->setText(noi->noi_out_500);
                ui->tableWidget_fanCoil_noi->item(row,12)->setText(noi->noi_out_1k);
                ui->tableWidget_fanCoil_noi->item(row,13)->setText(noi->noi_out_2k);
                ui->tableWidget_fanCoil_noi->item(row,14)->setText(noi->noi_out_4k);
                ui->tableWidget_fanCoil_noi->item(row,15)->setText(noi->noi_out_8k);
                ui->tableWidget_fanCoil_noi->item(row,16)->setText(noi->noi_out_total);
            }
            componentManager.updateRevisedComponent(noi->table_id, QSharedPointer<FanCoil_noise>(new FanCoil_noise(*noi)),"风机盘管");
            delete noi;
        }
    }
}

#pragma endregion}
/**********风机盘管噪音**********/





/**********空调器噪音(单风机)**********/
#pragma region "stack_air_noi_single"{

//初始化表格
void Widget::initTableWidget_air_noi_single()
{
    int colCount = 19;

    QStringList headerText;
    headerText << "" << "序号" << "编号" << "型号" << "风量\n(m³/h)" << "静压\n(Pa)" << "品牌" << "类型"
               << "噪音\n位置" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
               << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "总值\ndB(A)" << "来源";

    int columnWidths[] = {30, 40, 120, 120, 55, 55, 55, 80, 60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_air_noi_single_fan, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_air_noi_single_fan_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_air_noi_single_fan;
    Dialog_aircondition_noise *dialog =  new Dialog_aircondition_noise(this,-1,0);
    std::unique_ptr<Aircondition_noise> noi;

    int rowCount = tableWidget->rowCount(); //获取当前行数
    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Aircondition_noise>(std::move(*static_cast<Aircondition_noise*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() / 2 + 1);
        if (noi != nullptr) {
            QStringList data_send_in = {
                noi->table_id,
                noi->number,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
                noi->brand,
                "送风机",
                "进口",
                noi->noi_send_in_63,
                noi->noi_send_in_125,
                noi->noi_send_in_250,
                noi->noi_send_in_500,
                noi->noi_send_in_1k,
                noi->noi_send_in_2k,
                noi->noi_send_in_4k,
                noi->noi_send_in_8k,
                noi->noi_send_in_total,
                "厂家"
            };
            QStringList data_send_out = {
                noi->table_id,
                noi->number,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
                noi->brand,
                "送风机",
                "出口",
                noi->noi_send_out_63,
                noi->noi_send_out_125,
                noi->noi_send_out_250,
                noi->noi_send_out_500,
                noi->noi_send_out_1k,
                noi->noi_send_out_2k,
                noi->noi_send_out_4k,
                noi->noi_send_out_8k,
                noi->noi_send_out_total,
                "厂家"
            };
            // 使用通用函数添加行
            addRowToTable(tableWidget, data_send_in);
            addRowToTable(tableWidget, data_send_out);


            componentManager.addComponent(QSharedPointer<Aircondition_noise>(noi.release()),"空调器");
            for(int i = 0; i < tableWidget->columnCount(); i++)
            {
                if(i < 8 || i > 17)
                {
                    tableWidget->setSpan(rowCount, i, 2, 1);
                }
            }
        }
    }
}

void Widget::on_pushButton_air_noi_single_fan_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_air_noi_single_fan, 2, "空调器");
}

//修改按钮
void Widget::on_pushButton_air_noi_single_fan_revise_clicked()
{
    int colCount = 19;
    QString table_id = "";
    for (int row = 0; row < ui->tableWidget_air_noi_single_fan->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_air_noi_single_fan->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked()) {
            Aircondition_noise *noi = new Aircondition_noise();
            noi->type = "单风机";
            table_id = ui->tableWidget_air_noi_single_fan->item(row - 1,1)->text();
            noi->number = ui->tableWidget_air_noi_single_fan->item(row - 1,2)->text();
            noi->model = ui->tableWidget_air_noi_single_fan->item(row - 1,3)->text();
            noi->air_volume = ui->tableWidget_air_noi_single_fan->item(row - 1,4)->text();
            noi->static_pressure = ui->tableWidget_air_noi_single_fan->item(row - 1,5)->text();
            noi->brand = ui->tableWidget_air_noi_single_fan->item(row - 1,6)->text();

            noi->noi_send_in_63 = ui->tableWidget_air_noi_single_fan->item(row - 1,9)->text();
            noi->noi_send_in_125 = ui->tableWidget_air_noi_single_fan->item(row - 1,10)->text();
            noi->noi_send_in_250 = ui->tableWidget_air_noi_single_fan->item(row - 1,11)->text();
            noi->noi_send_in_500 = ui->tableWidget_air_noi_single_fan->item(row - 1,12)->text();
            noi->noi_send_in_1k = ui->tableWidget_air_noi_single_fan->item(row - 1,13)->text();
            noi->noi_send_in_2k = ui->tableWidget_air_noi_single_fan->item(row - 1,14)->text();
            noi->noi_send_in_4k = ui->tableWidget_air_noi_single_fan->item(row - 1,15)->text();
            noi->noi_send_in_8k = ui->tableWidget_air_noi_single_fan->item(row - 1,16)->text();
            noi->noi_send_in_total = ui->tableWidget_air_noi_single_fan->item(row - 1,17)->text();

            noi->noi_send_out_63 = ui->tableWidget_air_noi_single_fan->item(row,9)->text();
            noi->noi_send_out_125 = ui->tableWidget_air_noi_single_fan->item(row,10)->text();
            noi->noi_send_out_250 = ui->tableWidget_air_noi_single_fan->item(row,11)->text();
            noi->noi_send_out_500 = ui->tableWidget_air_noi_single_fan->item(row,12)->text();
            noi->noi_send_out_1k = ui->tableWidget_air_noi_single_fan->item(row,13)->text();
            noi->noi_send_out_2k = ui->tableWidget_air_noi_single_fan->item(row,14)->text();
            noi->noi_send_out_4k = ui->tableWidget_air_noi_single_fan->item(row,15)->text();
            noi->noi_send_out_8k = ui->tableWidget_air_noi_single_fan->item(row,16)->text();
            noi->noi_send_out_total = ui->tableWidget_air_noi_single_fan->item(row,17)->text();
            // 创建模态对话框，并设置为模态
            Dialog_aircondition_noise *airNoiseDialog = new Dialog_aircondition_noise(this,row, 0,*noi);
            airNoiseDialog->setWindowModality(Qt::ApplicationModal);
            airNoiseDialog->setModal(true);

            // 显示对话框,等待用户操作
            if (airNoiseDialog->exec() == QDialog::Accepted)
            {
                noi = static_cast<Aircondition_noise*>(airNoiseDialog->getNoi());
                ui->tableWidget_air_noi_single_fan->item(row - 1,2)->setText(noi->number);
                ui->tableWidget_air_noi_single_fan->item(row - 1,3)->setText(noi->brand);
                ui->tableWidget_air_noi_single_fan->item(row - 1,4)->setText(noi->air_volume);
                ui->tableWidget_air_noi_single_fan->item(row - 1,5)->setText(noi->static_pressure);
                ui->tableWidget_air_noi_single_fan->item(row - 1,6)->setText(noi->model);

                ui->tableWidget_air_noi_single_fan->item(row - 1,9)->setText(noi->noi_send_in_63);
                ui->tableWidget_air_noi_single_fan->item(row - 1,10)->setText(noi->noi_send_in_125);
                ui->tableWidget_air_noi_single_fan->item(row - 1,11)->setText(noi->noi_send_in_250);
                ui->tableWidget_air_noi_single_fan->item(row - 1,12)->setText(noi->noi_send_in_500);
                ui->tableWidget_air_noi_single_fan->item(row - 1,13)->setText(noi->noi_send_in_1k);
                ui->tableWidget_air_noi_single_fan->item(row - 1,14)->setText(noi->noi_send_in_2k);
                ui->tableWidget_air_noi_single_fan->item(row - 1,15)->setText(noi->noi_send_in_4k);
                ui->tableWidget_air_noi_single_fan->item(row - 1,16)->setText(noi->noi_send_in_8k);
                ui->tableWidget_air_noi_single_fan->item(row - 1,17)->setText(noi->noi_send_in_total);

                ui->tableWidget_air_noi_single_fan->item(row,2)->setText(noi->number);
                ui->tableWidget_air_noi_single_fan->item(row,3)->setText(noi->brand);
                ui->tableWidget_air_noi_single_fan->item(row,4)->setText(noi->air_volume);
                ui->tableWidget_air_noi_single_fan->item(row,5)->setText(noi->static_pressure);
                ui->tableWidget_air_noi_single_fan->item(row,6)->setText(noi->model);

                ui->tableWidget_air_noi_single_fan->item(row,9)->setText(noi->noi_send_out_63);
                ui->tableWidget_air_noi_single_fan->item(row,10)->setText(noi->noi_send_out_125);
                ui->tableWidget_air_noi_single_fan->item(row,11)->setText(noi->noi_send_out_250);
                ui->tableWidget_air_noi_single_fan->item(row,12)->setText(noi->noi_send_out_500);
                ui->tableWidget_air_noi_single_fan->item(row,13)->setText(noi->noi_send_out_1k);
                ui->tableWidget_air_noi_single_fan->item(row,14)->setText(noi->noi_send_out_2k);
                ui->tableWidget_air_noi_single_fan->item(row,15)->setText(noi->noi_send_out_4k);
                ui->tableWidget_air_noi_single_fan->item(row,16)->setText(noi->noi_send_out_8k);
                ui->tableWidget_air_noi_single_fan->item(row,17)->setText(noi->noi_send_out_total);
            }
            noi->table_id = table_id;
            componentManager.updateRevisedComponent(noi->table_id, QSharedPointer<Aircondition_noise>(new Aircondition_noise(*noi)),"空调器");

            delete noi;
        }
    }
}

#pragma endregion}
/**********空调器噪音(单风机)**********/

/**********空调器噪音(双风机)**********/
#pragma region "stack_air_noi_double"{
//初始化表格
void Widget::initTableWidget_air_noi_double()
{
    int colCount = 19;

    QStringList headerText;
    headerText << "" << "序号" << "编号" << "型号" << "风量\n(m³/h)" << "静压\n(Pa)" << "品牌" << "类型"
               << "噪音\n位置" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
               << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "总值\ndB(A)" << "来源";

    int columnWidths[] = {30, 40, 120, 120, 55, 55, 55, 80, 60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    initTableWidget(ui->tableWidget_air_noi_double_fan, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_air_noi_double_fan_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_air_noi_double_fan;
    Dialog_aircondition_noise *dialog = new Dialog_aircondition_noise(this,-1,1);
    std::unique_ptr<Aircondition_noise> noi;

    int rowCount = tableWidget->rowCount(); //获取当前行数
    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Aircondition_noise>(std::move(*static_cast<Aircondition_noise*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() / 4 + 1);
        if (noi != nullptr) {
            QStringList data_send_in = {
                noi->table_id,
                noi->number,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
                noi->brand,
                "送风机",
                "进口",
                noi->noi_send_in_63,
                noi->noi_send_in_125,
                noi->noi_send_in_250,
                noi->noi_send_in_500,
                noi->noi_send_in_1k,
                noi->noi_send_in_2k,
                noi->noi_send_in_4k,
                noi->noi_send_in_8k,
                noi->noi_send_in_total,
                "厂家"
            };
            QStringList data_send_out = {
                noi->table_id,
                noi->number,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
                noi->brand,
                "送风机",
                "出口",
                noi->noi_send_out_63,
                noi->noi_send_out_125,
                noi->noi_send_out_250,
                noi->noi_send_out_500,
                noi->noi_send_out_1k,
                noi->noi_send_out_2k,
                noi->noi_send_out_4k,
                noi->noi_send_out_8k,
                noi->noi_send_out_total,
                "厂家"
            };

            QStringList data_exhaust_in = {
                noi->table_id,
                noi->number,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
                noi->brand,
                "排风机",
                "进口",
                noi->noi_exhaust_in_63,
                noi->noi_exhaust_in_125,
                noi->noi_exhaust_in_250,
                noi->noi_exhaust_in_500,
                noi->noi_exhaust_in_1k,
                noi->noi_exhaust_in_2k,
                noi->noi_exhaust_in_4k,
                noi->noi_exhaust_in_8k,
                noi->noi_exhaust_in_total,
                "厂家"
            };

            QStringList data_exhaust_out = {
                noi->table_id,
                noi->number,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
                noi->brand,
                "排风机",
                "出口",
                noi->noi_exhaust_out_63,
                noi->noi_exhaust_out_125,
                noi->noi_exhaust_out_250,
                noi->noi_exhaust_out_500,
                noi->noi_exhaust_out_1k,
                noi->noi_exhaust_out_2k,
                noi->noi_exhaust_out_4k,
                noi->noi_exhaust_out_8k,
                noi->noi_exhaust_out_total,
                "厂家"
            };
            // 使用通用函数添加行
            addRowToTable(tableWidget, data_send_in);
            addRowToTable(tableWidget, data_send_out);
            addRowToTable(tableWidget, data_exhaust_in);
            addRowToTable(tableWidget, data_exhaust_out);

            componentManager.addComponent(QSharedPointer<Aircondition_noise>(noi.release()),"空调器");
            for(int i = 0; i < tableWidget->columnCount(); i++)
            {
                if(i < 7)
                {
                    tableWidget->setSpan(rowCount, i, 4, 1);
                }
                if(i == 7 || i > 17)
                {
                    tableWidget->setSpan(rowCount, i, 2, 1);
                    tableWidget->setSpan(rowCount + 2, i, 2, 1);
                }
            }
        }
    }
}

void Widget::on_pushButton_air_noi_double_fan_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_air_noi_double_fan, 4, "空调器");
}

//修改按钮
void Widget::on_pushButton_air_noi_double_fan_revise_clicked()
{
    int colCount = 19;
    QString table_id = "";
    for (int row = 0; row < ui->tableWidget_air_noi_double_fan->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_air_noi_double_fan->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked()) {
            Aircondition_noise *noi = new Aircondition_noise();
            noi->type = "双风机";
            table_id = ui->tableWidget_air_noi_double_fan->item(row - 3,1)->text();
            noi->number = ui->tableWidget_air_noi_double_fan->item(row - 3,2)->text();
            noi->model = ui->tableWidget_air_noi_double_fan->item(row - 3,2)->text();
            noi->air_volume = ui->tableWidget_air_noi_double_fan->item(row - 3,4)->text();
            noi->static_pressure = ui->tableWidget_air_noi_double_fan->item(row - 3,5)->text();
            noi->brand = ui->tableWidget_air_noi_double_fan->item(row - 3,6)->text();

            noi->noi_send_in_63 = ui->tableWidget_air_noi_double_fan->item(row - 3,9)->text();
            noi->noi_send_in_125 = ui->tableWidget_air_noi_double_fan->item(row - 3,10)->text();
            noi->noi_send_in_250 = ui->tableWidget_air_noi_double_fan->item(row - 3,11)->text();
            noi->noi_send_in_500 = ui->tableWidget_air_noi_double_fan->item(row - 3,12)->text();
            noi->noi_send_in_1k = ui->tableWidget_air_noi_double_fan->item(row - 3,13)->text();
            noi->noi_send_in_2k = ui->tableWidget_air_noi_double_fan->item(row - 3,14)->text();
            noi->noi_send_in_4k = ui->tableWidget_air_noi_double_fan->item(row - 3,15)->text();
            noi->noi_send_in_8k = ui->tableWidget_air_noi_double_fan->item(row - 3,16)->text();
            noi->noi_send_in_total = ui->tableWidget_air_noi_double_fan->item(row - 3,17)->text();

            noi->noi_send_out_63 = ui->tableWidget_air_noi_double_fan->item(row - 2,9)->text();
            noi->noi_send_out_125 = ui->tableWidget_air_noi_double_fan->item(row - 2,10)->text();
            noi->noi_send_out_250 = ui->tableWidget_air_noi_double_fan->item(row - 2,11)->text();
            noi->noi_send_out_500 = ui->tableWidget_air_noi_double_fan->item(row - 2,12)->text();
            noi->noi_send_out_1k = ui->tableWidget_air_noi_double_fan->item(row - 2,13)->text();
            noi->noi_send_out_2k = ui->tableWidget_air_noi_double_fan->item(row - 2,14)->text();
            noi->noi_send_out_4k = ui->tableWidget_air_noi_double_fan->item(row - 2,15)->text();
            noi->noi_send_out_8k = ui->tableWidget_air_noi_double_fan->item(row - 2,16)->text();
            noi->noi_send_out_total = ui->tableWidget_air_noi_double_fan->item(row - 2,17)->text();

            noi->noi_exhaust_in_63 = ui->tableWidget_air_noi_double_fan->item(row - 1,9)->text();
            noi->noi_exhaust_in_125 = ui->tableWidget_air_noi_double_fan->item(row - 1,10)->text();
            noi->noi_exhaust_in_250 = ui->tableWidget_air_noi_double_fan->item(row - 1,11)->text();
            noi->noi_exhaust_in_500 = ui->tableWidget_air_noi_double_fan->item(row - 1,12)->text();
            noi->noi_exhaust_in_1k = ui->tableWidget_air_noi_double_fan->item(row - 1,13)->text();
            noi->noi_exhaust_in_2k = ui->tableWidget_air_noi_double_fan->item(row - 1,14)->text();
            noi->noi_exhaust_in_4k = ui->tableWidget_air_noi_double_fan->item(row - 1,15)->text();
            noi->noi_exhaust_in_8k = ui->tableWidget_air_noi_double_fan->item(row - 1,16)->text();
            noi->noi_exhaust_in_total = ui->tableWidget_air_noi_double_fan->item(row - 1,17)->text();

            noi->noi_exhaust_out_63 = ui->tableWidget_air_noi_double_fan->item(row,9)->text();
            noi->noi_exhaust_out_125 = ui->tableWidget_air_noi_double_fan->item(row,10)->text();
            noi->noi_exhaust_out_250 = ui->tableWidget_air_noi_double_fan->item(row,11)->text();
            noi->noi_exhaust_out_500 = ui->tableWidget_air_noi_double_fan->item(row,12)->text();
            noi->noi_exhaust_out_1k = ui->tableWidget_air_noi_double_fan->item(row,13)->text();
            noi->noi_exhaust_out_2k = ui->tableWidget_air_noi_double_fan->item(row,14)->text();
            noi->noi_exhaust_out_4k = ui->tableWidget_air_noi_double_fan->item(row,15)->text();
            noi->noi_exhaust_out_8k = ui->tableWidget_air_noi_double_fan->item(row,16)->text();
            noi->noi_exhaust_out_total = ui->tableWidget_air_noi_double_fan->item(row,17)->text();



            // 创建模态对话框，并设置为模态
            Dialog_aircondition_noise *airNoiseDialog = new Dialog_aircondition_noise(this,row, 1,*noi);
            airNoiseDialog->setWindowModality(Qt::ApplicationModal);
            airNoiseDialog->setModal(true);

            // 显示对话框,等待用户操作
            if (airNoiseDialog->exec() == QDialog::Accepted)
            {
                noi = static_cast<Aircondition_noise*>(airNoiseDialog->getNoi());
                ui->tableWidget_air_noi_double_fan->item(row - 3,2)->setText(noi->number);
                ui->tableWidget_air_noi_double_fan->item(row - 3,3)->setText(noi->brand);
                ui->tableWidget_air_noi_double_fan->item(row - 3,4)->setText(noi->air_volume);
                ui->tableWidget_air_noi_double_fan->item(row - 3,5)->setText(noi->static_pressure);
                ui->tableWidget_air_noi_double_fan->item(row - 3,6)->setText(noi->model);

                ui->tableWidget_air_noi_double_fan->item(row - 3,9)->setText(noi->noi_send_in_63);
                ui->tableWidget_air_noi_double_fan->item(row - 3,10)->setText(noi->noi_send_in_125);
                ui->tableWidget_air_noi_double_fan->item(row - 3,11)->setText(noi->noi_send_in_250);
                ui->tableWidget_air_noi_double_fan->item(row - 3,12)->setText(noi->noi_send_in_500);
                ui->tableWidget_air_noi_double_fan->item(row - 3,13)->setText(noi->noi_send_in_1k);
                ui->tableWidget_air_noi_double_fan->item(row - 3,14)->setText(noi->noi_send_in_2k);
                ui->tableWidget_air_noi_double_fan->item(row - 3,15)->setText(noi->noi_send_in_4k);
                ui->tableWidget_air_noi_double_fan->item(row - 3,16)->setText(noi->noi_send_in_8k);
                ui->tableWidget_air_noi_double_fan->item(row - 3,17)->setText(noi->noi_send_in_total);

                ui->tableWidget_air_noi_double_fan->item(row - 2,2)->setText(noi->number);
                ui->tableWidget_air_noi_double_fan->item(row - 2,3)->setText(noi->brand);
                ui->tableWidget_air_noi_double_fan->item(row - 2,4)->setText(noi->air_volume);
                ui->tableWidget_air_noi_double_fan->item(row - 2,5)->setText(noi->static_pressure);
                ui->tableWidget_air_noi_double_fan->item(row - 2,6)->setText(noi->model);

                ui->tableWidget_air_noi_double_fan->item(row - 2,9)->setText(noi->noi_send_out_63);
                ui->tableWidget_air_noi_double_fan->item(row - 2,10)->setText(noi->noi_send_out_125);
                ui->tableWidget_air_noi_double_fan->item(row - 2,11)->setText(noi->noi_send_out_250);
                ui->tableWidget_air_noi_double_fan->item(row - 2,12)->setText(noi->noi_send_out_500);
                ui->tableWidget_air_noi_double_fan->item(row - 2,13)->setText(noi->noi_send_out_1k);
                ui->tableWidget_air_noi_double_fan->item(row - 2,14)->setText(noi->noi_send_out_2k);
                ui->tableWidget_air_noi_double_fan->item(row - 2,15)->setText(noi->noi_send_out_4k);
                ui->tableWidget_air_noi_double_fan->item(row - 2,16)->setText(noi->noi_send_out_8k);
                ui->tableWidget_air_noi_double_fan->item(row - 2,17)->setText(noi->noi_send_out_total);

                ui->tableWidget_air_noi_double_fan->item(row - 1,2)->setText(noi->number);
                ui->tableWidget_air_noi_double_fan->item(row - 1,3)->setText(noi->brand);
                ui->tableWidget_air_noi_double_fan->item(row - 1,4)->setText(noi->air_volume);
                ui->tableWidget_air_noi_double_fan->item(row - 1,5)->setText(noi->static_pressure);
                ui->tableWidget_air_noi_double_fan->item(row - 1,6)->setText(noi->model);

                ui->tableWidget_air_noi_double_fan->item(row - 1,9)->setText(noi->noi_exhaust_in_63);
                ui->tableWidget_air_noi_double_fan->item(row - 1,10)->setText(noi->noi_exhaust_in_125);
                ui->tableWidget_air_noi_double_fan->item(row - 1,11)->setText(noi->noi_exhaust_in_250);
                ui->tableWidget_air_noi_double_fan->item(row - 1,12)->setText(noi->noi_exhaust_in_500);
                ui->tableWidget_air_noi_double_fan->item(row - 1,13)->setText(noi->noi_exhaust_in_1k);
                ui->tableWidget_air_noi_double_fan->item(row - 1,14)->setText(noi->noi_exhaust_in_2k);
                ui->tableWidget_air_noi_double_fan->item(row - 1,15)->setText(noi->noi_exhaust_in_4k);
                ui->tableWidget_air_noi_double_fan->item(row - 1,16)->setText(noi->noi_exhaust_in_8k);
                ui->tableWidget_air_noi_double_fan->item(row - 1,17)->setText(noi->noi_exhaust_in_total);

                ui->tableWidget_air_noi_double_fan->item(row,2)->setText(noi->number);
                ui->tableWidget_air_noi_double_fan->item(row,3)->setText(noi->model);
                ui->tableWidget_air_noi_double_fan->item(row,4)->setText(noi->static_pressure);
                ui->tableWidget_air_noi_double_fan->item(row,5)->setText(noi->air_volume);
                ui->tableWidget_air_noi_double_fan->item(row,6)->setText(noi->brand);

                ui->tableWidget_air_noi_double_fan->item(row,9)->setText(noi->noi_exhaust_out_63);
                ui->tableWidget_air_noi_double_fan->item(row,10)->setText(noi->noi_exhaust_out_125);
                ui->tableWidget_air_noi_double_fan->item(row,11)->setText(noi->noi_exhaust_out_250);
                ui->tableWidget_air_noi_double_fan->item(row,12)->setText(noi->noi_exhaust_out_500);
                ui->tableWidget_air_noi_double_fan->item(row,13)->setText(noi->noi_exhaust_out_1k);
                ui->tableWidget_air_noi_double_fan->item(row,14)->setText(noi->noi_exhaust_out_2k);
                ui->tableWidget_air_noi_double_fan->item(row,15)->setText(noi->noi_exhaust_out_4k);
                ui->tableWidget_air_noi_double_fan->item(row,16)->setText(noi->noi_exhaust_out_8k);
                ui->tableWidget_air_noi_double_fan->item(row,17)->setText(noi->noi_exhaust_out_total);
            }
            noi->table_id = table_id;
            componentManager.updateRevisedComponent(noi->table_id, QSharedPointer<Aircondition_noise>(new Aircondition_noise(*noi)),"空调器");

            delete noi;
        }
    }
}

#pragma endregion}
/**********空调器噪音(双风机)**********/



/**********变风量末端**********/
#pragma region "stack_VAV_terminal"{
//初始化表格
void Widget::initTableWidget_VAV_terminal()
{
    int colCount = 18;
    QStringList headerText;
    headerText<< "" << "序号" << "编号" << "型号" << "阀门\n开度" << "风量\n(m³/h)"  << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)"
              << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源" << "UUID";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 120, 60, 60, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55 ,60, 0};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_VAV_terminal, headerText, columnWidths, colCount);

    // 隐藏最后一列（假设隐藏列是最后一列）
    ui->tableWidget_VAV_terminal->setColumnHidden(colCount - 1, true); // 让隐藏列不可见
}

void Widget::on_pushButton_VAV_terminal_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_VAV_terminal;
    Dialog_VAV_terminal *dialog = new Dialog_VAV_terminal(this);
    std::unique_ptr<VAV_terminal_noise> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<VAV_terminal_noise>(std::move(*static_cast<VAV_terminal_noise*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data = {
                noi->table_id,
                noi->number,
                noi->model,
                noi->valve_open_degree,
                noi->air_volume,
                noi->brand,
                noi->noi_63,
                noi->noi_125,
                noi->noi_250,
                noi->noi_500,
                noi->noi_1k,
                noi->noi_2k,
                noi->noi_4k,
                noi->noi_8k,
                noi->noi_total,
                "厂家信息"
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data);

            componentManager.addComponent(QSharedPointer<VAV_terminal_noise>(noi.release()),"变风量末端");
        }
    }

}

void Widget::on_pushButton_VAV_terminal_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_VAV_terminal, 1,"变风量末端");
}

//修改按钮
void Widget::on_pushButton_VAV_terminal_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_VAV_terminal;
    VAV_terminal_noise *noi = new VAV_terminal_noise();
    QString table_id = "";
    int table_id_col = 1;
    QVector<QString*> items = {
        &noi->number,        
        &noi->model,
        &noi->valve_open_degree,
        &noi->air_volume,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total
    };
    int cols[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        noiseRevision<VAV_terminal_noise, Dialog_VAV_terminal>(tableWidget, row, noi, items, cols, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<VAV_terminal_noise>(new VAV_terminal_noise(*noi)),"变风量末端");
    }
}

#pragma endregion}
/**********变风量末端**********/




/**********圆形调风门**********/
#pragma region "stack_circular_damper"{
//初始化表格
void Widget::initTableWidget_circular_damper()
{
    int colCount = 16;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "阀门\n开度" << "风量\n(m³/h)" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 55, 55, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_circular_damper, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_circular_damper_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_circular_damper;
    Dialog_circular_damper *dialog = new Dialog_circular_damper(this);
    std::unique_ptr<Circular_damper_noi> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Circular_damper_noi>(std::move(*static_cast<Circular_damper_noi*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data = {
                noi->table_id,
                noi->model,
                noi->angle,
                noi->air_volume,
                noi->brand,
                noi->noi_63,
                noi->noi_125,
                noi->noi_250,
                noi->noi_500,
                noi->noi_1k,
                noi->noi_2k,
                noi->noi_4k,
                noi->noi_8k,
                noi->noi_total,
                noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data);

            componentManager.addComponent(QSharedPointer<Circular_damper_noi>(noi.release()),"圆形调风门");
        }
    }
}

void Widget::on_pushButton_circular_damper_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_circular_damper, 1, "圆形调风门");
}

//修改按钮
void Widget::on_pushButton_circular_damper_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_circular_damper;
    Circular_damper_noi *noi = new Circular_damper_noi();
    QString table_id = "";
    int table_id_col = 1;
    QVector<QString*> items = {
        &noi->model,
        &noi->angle,
        &noi->air_volume,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
        &noi->getMode
    };
    int cols[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        noiseRevision<Circular_damper_noi, Dialog_circular_damper>(tableWidget, row, noi, items, cols, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Circular_damper_noi>(new Circular_damper_noi(*noi)),"圆形调风门");
    }
}

#pragma endregion}
/**********圆形调风门**********/




/**********方形调风门**********/
#pragma region "stack_rect_damper"{
//初始化表格
void Widget::initTableWidget_rect_damper()
{
    int colCount = 16;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "阀门\n开度" << "风量\n(m³/h)" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 55, 55, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_rect_damper, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_rect_damper_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_rect_damper;
    Dialog_rect_damper *dialog = new Dialog_rect_damper(this);
    std::unique_ptr<Rect_damper_noi> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Rect_damper_noi>(std::move(*static_cast<Rect_damper_noi*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data = {
                noi->table_id,
                noi->model,
                noi->angle,
                noi->air_volume,
                noi->brand,
                noi->noi_63,
                noi->noi_125,
                noi->noi_250,
                noi->noi_500,
                noi->noi_1k,
                noi->noi_2k,
                noi->noi_4k,
                noi->noi_8k,
                noi->noi_total,
                noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data);

            componentManager.addComponent(QSharedPointer<Rect_damper_noi>(noi.release()),"方形调风门");
        }
    }

}


void Widget::on_pushButton_rect_damper_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_rect_damper, 1, "方形调风门");
}

//修改按钮
void Widget::on_pushButton_rect_damper_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_rect_damper;
    Rect_damper_noi *noi = new Rect_damper_noi();
    QString table_id = "";
    int table_id_col = 1;
    QVector<QString*> items = {
        &noi->model,
        &noi->angle,
        &noi->air_volume,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
        &noi->getMode
    };
    int cols[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        noiseRevision<Rect_damper_noi, Dialog_rect_damper>(tableWidget, row, noi, items, cols, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Rect_damper_noi>(new Rect_damper_noi(*noi)),"方形调风门");
    }
}

#pragma endregion}
/**********方形调风门**********/





/**********布风器+散流器**********/
#pragma region "stack_air_diff"{
//初始化表格
void Widget::initTableWidget_air_diff()
{
    int colCount = 17;
    QStringList headerText;
    headerText<< "" << "序号" << "布风器型号" << "散流器型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)"
              << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
              << "8kHz\n(dB)" << "总值\ndB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_air_diff, headerText, columnWidths, colCount);

    colCount = 16;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "布风器型号" << "散流器型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)"
                    << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
                    << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {30, 40, 120, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_air_diff_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    colCount = 16;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "布风器型号" << "散流器型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)"
                   << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
                   << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {30, 40, 120, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_air_diff_terminal_refl, headerText_refl, refl_columnWidths, colCount);
}

void Widget::on_pushButton_air_diff_add_clicked()
{
    QTableWidget *tableWidget_noise = ui->tableWidget_air_diff;
    QTableWidget *tableWidget_atten = ui->tableWidget_air_diff_terminal_atten;
    QTableWidget *tableWidget_refl = ui->tableWidget_air_diff_terminal_refl;
    Dialog_air_diff *dialog = new Dialog_air_diff("布风器+散流器",this);
    AirDiff_noise* noi = nullptr;

    if (dialog->exec() == QDialog::Accepted) {
        noi = static_cast<AirDiff_noise*>(dialog->getNoi());
        noi->table_id = QString::number(tableWidget_noise->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data_noise = {
                noi->table_id,
                noi->air_distributor_model,
                noi->diffuser_model,
                noi->type,
                noi->size,
                noi->brand,
                noi->noi_63,
                noi->noi_125,
                noi->noi_250,
                noi->noi_500,
                noi->noi_1k,
                noi->noi_2k,
                noi->noi_4k,
                noi->noi_8k,
                noi->noi_total,
                "厂家"
            };

            QStringList data_atten = {
                noi->table_id,
                noi->air_distributor_model,
                noi->diffuser_model,
                noi->type,
                noi->size,
                noi->brand,
                noi->atten_63,
                noi->atten_125,
                noi->atten_250,
                noi->atten_500,
                noi->atten_1k,
                noi->atten_2k,
                noi->atten_4k,
                noi->atten_8k,
                "厂家"
            };

            QStringList data_refl = {
                noi->table_id,
                noi->air_distributor_model,
                noi->diffuser_model,
                noi->type,
                noi->size,
                noi->brand,
                noi->refl_63,
                noi->refl_125,
                noi->refl_250,
                noi->refl_500,
                noi->refl_1k,
                noi->refl_2k,
                noi->refl_4k,
                noi->refl_8k,
                noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget_noise, data_noise);
            addRowToTable(tableWidget_atten, data_atten);
            addRowToTable(tableWidget_refl, data_refl);

            componentManager.addComponent(QSharedPointer<AirDiff_noise>(noi),"布风器+散流器");
        }
    }
}

void Widget::on_pushButton_air_diff_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_air_diff, ui->tableWidget_air_diff_terminal_atten, ui->tableWidget_air_diff_terminal_refl, "布风器+散流器");
}

void Widget::on_pushButton_air_diff_terminal_atten_add_clicked()
{
    on_pushButton_air_diff_add_clicked();
}


void Widget::on_pushButton_air_diff_terminal_atten_del_clicked()
{
    on_pushButton_air_diff_del_clicked();
}

void Widget::on_pushButton_air_diff_terminal_refl_add_clicked()
{
    on_pushButton_air_diff_add_clicked();
}


void Widget::on_pushButton_air_diff_terminal_refl_del_clicked()
{
    on_pushButton_air_diff_del_clicked();
}

//修改按钮
void Widget::on_pushButton_air_diff_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_air_diff;
    QTableWidget* tableWidget_noise = ui->tableWidget_air_diff;
    QTableWidget* tableWidget_atten = ui->tableWidget_air_diff_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_air_diff_terminal_refl;
    AirDiff_noise *noi = new AirDiff_noise();
    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->air_distributor_model,
        &noi->diffuser_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    QVector<QString*> items_atten = {        
        &noi->air_distributor_model,
        &noi->diffuser_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_refl = {        
        &noi->air_distributor_model,
        &noi->diffuser_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<AirDiff_noise, Dialog_air_diff>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id,"布风器+散流器");
        componentManager.updateRevisedComponent(table_id, QSharedPointer<AirDiff_noise>(new AirDiff_noise(*noi)),"布风器+散流器");
    }
}

void Widget::on_pushButton_air_diff_terminal_atten_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_air_diff_terminal_atten;
    QTableWidget* tableWidget_noise = ui->tableWidget_air_diff;
    QTableWidget* tableWidget_atten = ui->tableWidget_air_diff_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_air_diff_terminal_refl;
    AirDiff_noise *noi = new AirDiff_noise();
    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->air_distributor_model,
        &noi->diffuser_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    QVector<QString*> items_atten = {        
        &noi->air_distributor_model,
        &noi->diffuser_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_refl = {        
        &noi->air_distributor_model,
        &noi->diffuser_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<AirDiff_noise, Dialog_air_diff>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id,"布风器+散流器");
        componentManager.updateRevisedComponent(table_id, QSharedPointer<AirDiff_noise>(new AirDiff_noise(*noi)),"布风器+散流器");
    }
}

void Widget::on_pushButton_air_diff_terminal_refl_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_air_diff_terminal_refl;
    QTableWidget* tableWidget_noise = ui->tableWidget_air_diff;
    QTableWidget* tableWidget_atten = ui->tableWidget_air_diff_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_air_diff_terminal_refl;
    AirDiff_noise *noi = new AirDiff_noise();

    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->air_distributor_model,
        &noi->diffuser_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    QVector<QString*> items_atten = {        
        &noi->air_distributor_model,
        &noi->diffuser_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_refl = {        
        &noi->air_distributor_model,
        &noi->diffuser_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<AirDiff_noise, Dialog_air_diff>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id,"布风器+散流器");
        componentManager.updateRevisedComponent(table_id, QSharedPointer<AirDiff_noise>(new AirDiff_noise(*noi)),"布风器+散流器");
    }
}
#pragma endregion}
/**********布风器+散流器**********/





/**********抽/送风头**********/
#pragma region "stack_pump_send_tuyere"{
void Widget::initTableWidget_pump_send_tuyere()
{
    this->initTableWidget_pump_tuyere();
    this->initTableWidget_send_tuyere();
    ui->stackedWidget_pump_send_table->setCurrentWidget(ui->page_pump);
    ui->stackedWidget_pump_send_terminal_atten_table->setCurrentWidget(ui->page_pump_atten);
    ui->stackedWidget_pump_send_terminal_refl_table->setCurrentWidget(ui->page_pump_refl);
}

void Widget::initTableWidget_pump_tuyere()
{
    int colCount = 16;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    initTableWidget(ui->tableWidget_pump_tuyere, headerText, columnWidths, colCount);

    colCount = 15;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                    << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                    << "来源";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_pump_tuyere_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    colCount = 15;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                   << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                   << "来源";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_pump_tuyere_terminal_refl, headerText_refl, refl_columnWidths, colCount);
}

void Widget::initTableWidget_send_tuyere()
{
    int colCount = 16;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    initTableWidget(ui->tableWidget_send_tuyere, headerText, columnWidths, colCount);

    colCount = 15;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                    << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                    << "来源";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_send_tuyere_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    colCount = 15;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                   << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                   << "来源";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_send_tuyere_terminal_refl, headerText_refl, refl_columnWidths, colCount);
}

void Widget::on_pushButton_pump_send_add_clicked()
{
    QTableWidget *tableWidget_noise = nullptr;
    QTableWidget *tableWidget_atten = nullptr;
    QTableWidget *tableWidget_refl = nullptr;
    Dialog_pump_send *dialog = nullptr;
    std::unique_ptr<PumpSend_noise> noi;

    if(ui->stackedWidget_pump_send_table->currentWidget() == ui->page_pump)
    {
        tableWidget_noise = ui->tableWidget_pump_tuyere;
        tableWidget_atten = ui->tableWidget_pump_tuyere_terminal_atten;
        tableWidget_refl = ui->tableWidget_pump_tuyere_terminal_refl;
        dialog = new Dialog_pump_send("抽风头",this);
    }
    else if(ui->stackedWidget_pump_send_table->currentWidget() == ui->page_send)
    {
        tableWidget_noise = ui->tableWidget_send_tuyere;
        tableWidget_atten = ui->tableWidget_send_tuyere_terminal_atten;
        tableWidget_refl = ui->tableWidget_send_tuyere_terminal_refl;
        dialog = new Dialog_pump_send("送风头",this);
    }

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<PumpSend_noise>(std::move(*static_cast<PumpSend_noise*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget_noise->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data_noise = {
                noi->table_id,                
                noi->model,
                noi->type,
                noi->size,
                noi->brand,
                noi->noi_63,
                noi->noi_125,
                noi->noi_250,
                noi->noi_500,
                noi->noi_1k,
                noi->noi_2k,
                noi->noi_4k,
                noi->noi_8k,
                noi->noi_total,
                "厂家"
            };

            QStringList data_atten = {
                noi->table_id,                
                noi->model,
                noi->type,
                noi->size,
                noi->brand,
                noi->atten_63,
                noi->atten_125,
                noi->atten_250,
                noi->atten_500,
                noi->atten_1k,
                noi->atten_2k,
                noi->atten_4k,
                noi->atten_8k,
                "厂家"
            };

            QStringList data_refl = {
                noi->table_id,                
                noi->model,
                noi->type,
                noi->size,
                noi->brand,
                noi->refl_63,
                noi->refl_125,
                noi->refl_250,
                noi->refl_500,
                noi->refl_1k,
                noi->refl_2k,
                noi->refl_4k,
                noi->refl_8k,
                noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget_noise, data_noise);
            addRowToTable(tableWidget_atten, data_atten);
            addRowToTable(tableWidget_refl, data_refl);

            componentManager.addComponent(QSharedPointer<PumpSend_noise>(noi.release()),"抽/送风头");
        }
    }
}

void Widget::on_pushButton_pump_send_del_clicked()
{
    if(ui->stackedWidget_pump_send_table->currentWidget() == ui->page_pump)
        deleteRowFromTable(ui->tableWidget_pump_tuyere, ui->tableWidget_pump_tuyere_terminal_atten, ui->tableWidget_pump_tuyere_terminal_refl,"抽/送风头");
    else if(ui->stackedWidget_pump_send_table->currentWidget() == ui->page_send)
        deleteRowFromTable(ui->tableWidget_send_tuyere, ui->tableWidget_send_tuyere_terminal_atten, ui->tableWidget_send_tuyere_terminal_refl,"抽/送风头");
}

void Widget::on_pushButton_pump_send_terminal_atten_add_clicked()
{
    on_pushButton_pump_send_add_clicked();
}

void Widget::on_pushButton_pump_send_terminal_atten_del_clicked()
{
    on_pushButton_pump_send_del_clicked();
}

void Widget::on_pushButton_pump_send_terminal_refl_add_clicked()
{
    on_pushButton_pump_send_add_clicked();
}

void Widget::on_pushButton_pump_send_terminal_refl_del_clicked()
{
    on_pushButton_pump_send_del_clicked();
}

void Widget::on_pushButton_pump_send_revise_clicked()
{
    QTableWidget* currentTableWidget = nullptr;
    QTableWidget *tableWidget_noise = nullptr;
    QTableWidget *tableWidget_atten = nullptr;
    QTableWidget *tableWidget_refl = nullptr;
    Dialog_pump_send *dialog = nullptr;
    PumpSend_noise* noi = new PumpSend_noise();
    QString typeName = "";
    if((ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere && ui->stackedWidget_pump_send_table->currentWidget() == ui->page_pump)
            || (ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere_terminal_atten && ui->stackedWidget_pump_send_terminal_atten_table->currentWidget() == ui->page_pump_atten)
            || (ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere_terminal_refl && ui->stackedWidget_pump_send_terminal_refl_table->currentWidget() == ui->page_pump_refl))
    {
        if(ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere && ui->stackedWidget_pump_send_table->currentWidget() == ui->page_pump)
            currentTableWidget = ui->tableWidget_pump_tuyere;
        else if(ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere_terminal_atten && ui->stackedWidget_pump_send_terminal_atten_table->currentWidget() == ui->page_pump_atten)
            currentTableWidget = ui->tableWidget_pump_tuyere_terminal_atten;
        else if(ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere_terminal_refl && ui->stackedWidget_pump_send_terminal_refl_table->currentWidget() == ui->page_pump_refl)
            currentTableWidget = ui->tableWidget_pump_tuyere_terminal_refl;

        tableWidget_noise = ui->tableWidget_pump_tuyere;
        tableWidget_atten = ui->tableWidget_pump_tuyere_terminal_atten;
        tableWidget_refl = ui->tableWidget_pump_tuyere_terminal_refl;
        typeName = "抽风头";
    }
    else if((ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere && ui->stackedWidget_pump_send_table->currentWidget() == ui->page_send)
            || (ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere_terminal_atten && ui->stackedWidget_pump_send_terminal_atten_table->currentWidget() == ui->page_send_atten)
            || (ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere_terminal_refl && ui->stackedWidget_pump_send_terminal_refl_table->currentWidget() == ui->page_send_refl))
    {
        if(ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere && ui->stackedWidget_pump_send_table->currentWidget() == ui->page_send)
            currentTableWidget = ui->tableWidget_send_tuyere;
        else if(ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere_terminal_atten && ui->stackedWidget_pump_send_terminal_atten_table->currentWidget() == ui->page_send_atten)
            currentTableWidget = ui->tableWidget_send_tuyere_terminal_atten;
        else if(ui->stackedWidget->currentWidget() == ui->page_pump_send_tuyere_terminal_refl && ui->stackedWidget_pump_send_terminal_refl_table->currentWidget() == ui->page_send_refl)
            currentTableWidget = ui->tableWidget_send_tuyere_terminal_refl;

        tableWidget_noise = ui->tableWidget_send_tuyere;
        tableWidget_atten = ui->tableWidget_send_tuyere_terminal_atten;
        tableWidget_refl = ui->tableWidget_send_tuyere_terminal_refl;
        typeName = "送风头";
    }

    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_atten = {        
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13};

    QVector<QString*> items_refl = {        
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<PumpSend_noise, Dialog_pump_send>(currentTableWidget, tableWidget_noise, tableWidget_atten,
                                                        tableWidget_refl, row, noi, items_noise, items_atten, items_refl,
                                                        cols_noise, cols_atten, cols_refl, table_id_col, table_id,typeName);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<PumpSend_noise>(new PumpSend_noise(*noi)),"抽/送风头");
    }

    delete noi;  // 在这里删除 noi 对象
}

void Widget::on_pushButton_pump_send_terminal_refl_revise_clicked()
{
    on_pushButton_pump_send_revise_clicked();
}

void Widget::on_pushButton_pump_send_terminal_atten_revise_clicked()
{
    on_pushButton_pump_send_revise_clicked();
}


/**切换表格**/
void Widget::on_pushButton_pump_table_clicked()
{
    ui->stackedWidget_pump_send_table->setCurrentWidget(ui->page_pump);

    ui->pushButton_pump_table->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_send_table->setStyleSheet("QPushButton { background-color: white; }");
}

void Widget::on_pushButton_send_table_clicked()
{
    ui->stackedWidget_pump_send_table->setCurrentWidget(ui->page_send);
    ui->pushButton_send_table->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_pump_table->setStyleSheet("QPushButton { background-color: white; }");
}

void Widget::on_pushButton_pump_terminal_atten_table_clicked()
{
    ui->stackedWidget_pump_send_terminal_atten_table->setCurrentWidget(ui->page_pump_atten);

    ui->pushButton_pump_terminal_atten_table->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_send_terminal_atten_table->setStyleSheet("QPushButton { background-color: white; }");
}

void Widget::on_pushButton_send_terminal_atten_table_clicked()
{
    ui->stackedWidget_pump_send_terminal_atten_table->setCurrentWidget(ui->page_send_atten);
    ui->pushButton_send_terminal_atten_table->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_pump_terminal_atten_table->setStyleSheet("QPushButton { background-color: white; }");
}

void Widget::on_pushButton_pump_terminal_refl_table_clicked()
{
    ui->stackedWidget_pump_send_terminal_refl_table->setCurrentWidget(ui->page_pump_refl);

    ui->pushButton_pump_terminal_refl_table->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_send_terminal_refl_table->setStyleSheet("QPushButton { background-color: white; }");
}

void Widget::on_pushButton_send_terminal_refl_table_clicked()
{
    ui->stackedWidget_pump_send_terminal_refl_table->setCurrentWidget(ui->page_send_refl);
    ui->pushButton_send_terminal_refl_table->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_pump_terminal_refl_table->setStyleSheet("QPushButton { background-color: white; }");
}
/**切换表格**/

void Widget::on_tableWidget_pump_tuyere_itemDoubleClicked(QTableWidgetItem *item)
{
    item->setFlags(Qt::ItemIsEditable); // 设置为只读

    QWidget* widget = ui->tableWidget_pump_tuyere->cellWidget(item->row(), 0); // Assuming the checkbox is in the first column (index 0)
    QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
    checkBox->setChecked(true);
    //on_pushButton_pump_tuyere_revise_clicked();
    checkBox->setChecked(false);
    item->setFlags(Qt::ItemIsEnabled); // 设置为只读
}

void Widget::on_tableWidget_send_tuyere_itemDoubleClicked(QTableWidgetItem *item)
{
    item->setFlags(Qt::ItemIsEditable); // 设置为只读

    QWidget* widget = ui->tableWidget_send_tuyere->cellWidget(item->row(), 0); // Assuming the checkbox is in the first column (index 0)
    QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
    checkBox->setChecked(true);
    //on_pushButton_send_tuyere_revise_clicked();
    checkBox->setChecked(false);
    item->setFlags(Qt::ItemIsEnabled); // 设置为只读
}
#pragma endregion}
/**********抽/送风头**********/




/**********静压箱+格栅**********/
#pragma region "stack_staticBox_grille"{
void Widget::initTableWidget_staticBox_grille()
{
    int colCount = 17;
    QStringList headerText;
    headerText<< "" << "序号" << "静压箱型号" << "格栅型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)"
              << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
              << "8kHz\n(dB)" << "总值\ndB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    initTableWidget(ui->tableWidget_staticBox_grille, headerText, columnWidths, colCount);


    colCount = 16;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "静压箱型号" << "格栅型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)"
                    << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
                    << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {30, 40, 120, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_staticBox_grille_terminal_atten, headerText_atten, atten_columnWidths, colCount);


    colCount = 16;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "静压箱型号" << "格栅型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)"
                   << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)"
                   << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {30, 40, 120, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_staticBox_grille_terminal_refl, headerText_refl, refl_columnWidths, colCount);
}

void Widget::on_pushButton_staticBox_grille_add_clicked()
{
    QTableWidget *tableWidget_noise = ui->tableWidget_staticBox_grille;
    QTableWidget *tableWidget_atten = ui->tableWidget_staticBox_grille_terminal_atten;
    QTableWidget *tableWidget_refl = ui->tableWidget_staticBox_grille_terminal_refl;
    Dialog_staticBox_grille *dialog = new Dialog_staticBox_grille(this);
    std::unique_ptr<StaticBox_grille_noise> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<StaticBox_grille_noise>(std::move(*static_cast<StaticBox_grille_noise*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget_noise->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data_noise = {
                noi->table_id,                
                noi->staticBox_model,
                noi->grille_model,
                noi->type,
                noi->size,
                noi->brand,
                noi->noi_63,
                noi->noi_125,
                noi->noi_250,
                noi->noi_500,
                noi->noi_1k,
                noi->noi_2k,
                noi->noi_4k,
                noi->noi_8k,
                noi->noi_total,
                "厂家"
            };

            QStringList data_atten = {
                noi->table_id,                
                noi->staticBox_model,
                noi->grille_model,
                noi->type,
                noi->size,
                noi->brand,
                noi->atten_63,
                noi->atten_125,
                noi->atten_250,
                noi->atten_500,
                noi->atten_1k,
                noi->atten_2k,
                noi->atten_4k,
                noi->atten_8k,
                "厂家"
            };

            QStringList data_refl = {
                QString::number(tableWidget_noise->rowCount() + 1),                
                noi->staticBox_model,
                noi->grille_model,
                noi->type,
                noi->size,
                noi->brand,
                noi->refl_63,
                noi->refl_125,
                noi->refl_250,
                noi->refl_500,
                noi->refl_1k,
                noi->refl_2k,
                noi->refl_4k,
                noi->refl_8k,
                noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget_noise, data_noise);
            addRowToTable(tableWidget_atten, data_atten);
            addRowToTable(tableWidget_refl, data_refl);

            componentManager.addComponent(QSharedPointer<StaticBox_grille_noise>(noi.release()),"静压箱+格栅");
        }
    }
}

void Widget::on_pushButton_staticBox_grille_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_staticBox_grille, ui->tableWidget_staticBox_grille_terminal_atten, ui->tableWidget_staticBox_grille_terminal_refl, "静压箱+格栅");
}

void Widget::on_pushButton_staticBox_grille_terminal_atten_add_clicked()
{
    on_pushButton_staticBox_grille_add_clicked();
}


void Widget::on_pushButton_staticBox_grille_terminal_atten_del_clicked()
{
    on_pushButton_staticBox_grille_del_clicked();
}

void Widget::on_pushButton_staticBox_grille_terminal_refl_add_clicked()
{
    on_pushButton_staticBox_grille_add_clicked();
}


void Widget::on_pushButton_staticBox_grille_terminal_refl_del_clicked()
{
    on_pushButton_staticBox_grille_del_clicked();
}

//修改按钮
void Widget::on_pushButton_staticBox_grille_terminal_atten_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_staticBox_grille_terminal_atten;
    QTableWidget* tableWidget_noise = ui->tableWidget_staticBox_grille;
    QTableWidget* tableWidget_atten = ui->tableWidget_staticBox_grille_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_staticBox_grille_terminal_refl;
    StaticBox_grille_noise *noi = new StaticBox_grille_noise();

    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->staticBox_model,
        &noi->grille_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_atten = {        
        &noi->staticBox_model,
        &noi->grille_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13};

    QVector<QString*> items_refl = {        
        &noi->staticBox_model,
        &noi->grille_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<StaticBox_grille_noise, Dialog_staticBox_grille>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<StaticBox_grille_noise>(new StaticBox_grille_noise(*noi)),"静压箱+格栅");
    }
}

void Widget::on_pushButton_staticBox_grille_terminal_refl_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_staticBox_grille_terminal_refl;
    QTableWidget* tableWidget_noise = ui->tableWidget_staticBox_grille;
    QTableWidget* tableWidget_atten = ui->tableWidget_staticBox_grille_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_staticBox_grille_terminal_refl;
    StaticBox_grille_noise *noi = new StaticBox_grille_noise();

    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->staticBox_model,
        &noi->grille_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_atten = {        
        &noi->staticBox_model,
        &noi->grille_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13};

    QVector<QString*> items_refl = {        
        &noi->staticBox_model,
        &noi->grille_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<StaticBox_grille_noise, Dialog_staticBox_grille>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<StaticBox_grille_noise>(new StaticBox_grille_noise(*noi)),"静压箱+格栅");
    }
}

void Widget::on_pushButton_staticBox_grille_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_staticBox_grille;
    QTableWidget* tableWidget_noise = ui->tableWidget_staticBox_grille;
    QTableWidget* tableWidget_atten = ui->tableWidget_staticBox_grille_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_staticBox_grille_terminal_refl;
    StaticBox_grille_noise *noi = new StaticBox_grille_noise();

    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->staticBox_model,
        &noi->grille_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_atten = {        
        &noi->staticBox_model,
        &noi->grille_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13};

    QVector<QString*> items_refl = {        
        &noi->staticBox_model,
        &noi->grille_model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<StaticBox_grille_noise, Dialog_staticBox_grille>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<StaticBox_grille_noise>(new StaticBox_grille_noise(*noi)),"静压箱+格栅");
    }
}

#pragma endregion}
/**********静压箱+格栅**********/




/**********置换通风末端**********/
#pragma region "stack_disp_vent_terminal"{
//初始化表格
void Widget::initTableWidget_disp_vent_terminal()
{
    int colCount = 16;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    initTableWidget(ui->tableWidget_disp_vent_terminal, headerText, columnWidths, colCount);

    colCount = 15;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                    << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                    << "来源";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_disp_vent_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    colCount = 15;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                   << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                   << "来源";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_disp_vent_terminal_refl, headerText_refl, refl_columnWidths, colCount);
}

void Widget::on_pushButton_disp_vent_terminal_add_clicked()
{
    QTableWidget *tableWidget_noise = ui->tableWidget_disp_vent_terminal;
    QTableWidget *tableWidget_atten = ui->tableWidget_disp_vent_terminal_atten;
    QTableWidget *tableWidget_refl = ui->tableWidget_disp_vent_terminal_refl;
    Dialog_disp_vent_terminal *dialog = new Dialog_disp_vent_terminal(this);
    std::unique_ptr<Disp_vent_terminal_noise> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Disp_vent_terminal_noise>(std::move(*static_cast<Disp_vent_terminal_noise*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget_noise->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data_noise = {
                QString::number(tableWidget_noise->rowCount() + 1),                
                noi->model,
                noi->type,
                noi->size,
                noi->brand,
                noi->noi_63,
                noi->noi_125,
                noi->noi_250,
                noi->noi_500,
                noi->noi_1k,
                noi->noi_2k,
                noi->noi_4k,
                noi->noi_8k,
                noi->noi_total,
                "厂家"
            };

            QStringList data_atten = {
                QString::number(tableWidget_noise->rowCount() + 1),                
                noi->model,
                noi->type,
                noi->size,
                noi->brand,
                noi->atten_63,
                noi->atten_125,
                noi->atten_250,
                noi->atten_500,
                noi->atten_1k,
                noi->atten_2k,
                noi->atten_4k,
                noi->atten_8k,
                "厂家"
            };

            QStringList data_refl = {
                QString::number(tableWidget_noise->rowCount() + 1),                
                noi->model,
                noi->type,
                noi->size,
                noi->brand,
                noi->refl_63,
                noi->refl_125,
                noi->refl_250,
                noi->refl_500,
                noi->refl_1k,
                noi->refl_2k,
                noi->refl_4k,
                noi->refl_8k,
                noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget_noise, data_noise);
            addRowToTable(tableWidget_atten, data_atten);
            addRowToTable(tableWidget_refl, data_refl);

            componentManager.addComponent(QSharedPointer<Disp_vent_terminal_noise>(noi.release()),"置换通风末端");
        }
    }
}


void Widget::on_pushButton_disp_vent_terminal_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_disp_vent_terminal, ui->tableWidget_disp_vent_terminal_atten, ui->tableWidget_disp_vent_terminal_refl,"置换通风末端");
}


void Widget::on_pushButton_disp_vent_terminal_atten_add_clicked()
{
    on_pushButton_disp_vent_terminal_add_clicked();
}


void Widget::on_pushButton_disp_vent_terminal_atten_del_clicked()
{
    on_pushButton_disp_vent_terminal_del_clicked();
}


void Widget::on_pushButton_disp_vent_terminal_refl_add_clicked()
{
    on_pushButton_disp_vent_terminal_add_clicked();
}


void Widget::on_pushButton_disp_vent_terminal_refl_del_clicked()
{
    on_pushButton_disp_vent_terminal_del_clicked();
}

//修改按钮
void Widget::on_pushButton_disp_vent_terminal_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_disp_vent_terminal;
    QTableWidget* tableWidget_noise = ui->tableWidget_disp_vent_terminal;
    QTableWidget* tableWidget_atten = ui->tableWidget_disp_vent_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_disp_vent_terminal_refl;
    Disp_vent_terminal_noise *noi = new Disp_vent_terminal_noise();
    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_atten = {        
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13};

    QVector<QString*> items_refl = {        
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<Disp_vent_terminal_noise, Dialog_disp_vent_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Disp_vent_terminal_noise>(new Disp_vent_terminal_noise(*noi)),"置换通风末端");
    }
}

void Widget::on_pushButton_disp_vent_terminal_atten_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_disp_vent_terminal_refl;
    QTableWidget* tableWidget_noise = ui->tableWidget_disp_vent_terminal;
    QTableWidget* tableWidget_atten = ui->tableWidget_disp_vent_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_disp_vent_terminal_refl;
    Disp_vent_terminal_noise *noi = new Disp_vent_terminal_noise();

    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_atten = {        
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13};

    QVector<QString*> items_refl = {        
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<Disp_vent_terminal_noise, Dialog_disp_vent_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Disp_vent_terminal_noise>(new Disp_vent_terminal_noise(*noi)),"置换通风末端");
    }
}

void Widget::on_pushButton_disp_vent_terminal_refl_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_disp_vent_terminal_atten;
    QTableWidget* tableWidget_noise = ui->tableWidget_disp_vent_terminal;
    QTableWidget* tableWidget_atten = ui->tableWidget_disp_vent_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_disp_vent_terminal_refl;
    Disp_vent_terminal_noise *noi = new Disp_vent_terminal_noise();

    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_atten = {        
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13};

    QVector<QString*> items_refl = {        
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<Disp_vent_terminal_noise, Dialog_disp_vent_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Disp_vent_terminal_noise>(new Disp_vent_terminal_noise(*noi)),"置换通风末端");
    }
}

#pragma endregion}
/**********置换通风末端**********/




/**********其他送风末端**********/
#pragma region "stack_other_send_terminal"{
//初始化表格
void Widget::initTableWidget_other_send_terminal()
{
    int colCount = 17;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
              << "总值\ndB(A)" << "来源" << "备注";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60, 120};

    initTableWidget(ui->tableWidget_other_send_terminal, headerText, columnWidths, colCount);

    colCount = 16;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                    << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                    << "来源" << "备注";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60, 120};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_other_send_terminal_atten, headerText_atten, atten_columnWidths, colCount);


    colCount = 16;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "型号" << "末端\n类型" << "末端\n尺寸" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                   << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                   << "来源" << "备注";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {30, 40, 120, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60, 120};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_other_send_terminal_refl, headerText_refl, refl_columnWidths, colCount);
}

void Widget::on_pushButton_other_send_terminal_add_clicked()
{
    QTableWidget *tableWidget_noise = ui->tableWidget_other_send_terminal;
    QTableWidget *tableWidget_atten = ui->tableWidget_other_send_terminal_atten;
    QTableWidget *tableWidget_refl = ui->tableWidget_other_send_terminal_refl;
    Dialog_other_send_terminal *dialog = new Dialog_other_send_terminal(this);
    std::unique_ptr<Other_send_terminal_noise> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Other_send_terminal_noise>(std::move(*static_cast<Other_send_terminal_noise*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget_noise->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data_noise = {
                QString::number(tableWidget_noise->rowCount() + 1),                
                noi->model,
                noi->type,
                noi->size,
                noi->brand,
                noi->noi_63,
                noi->noi_125,
                noi->noi_250,
                noi->noi_500,
                noi->noi_1k,
                noi->noi_2k,
                noi->noi_4k,
                noi->noi_8k,
                noi->noi_total,
                noi->remark,
                "厂家"
            };

            QStringList data_atten = {
                QString::number(tableWidget_noise->rowCount() + 1),                
                noi->model,
                noi->type,
                noi->size,
                noi->brand,
                noi->atten_63,
                noi->atten_125,
                noi->atten_250,
                noi->atten_500,
                noi->atten_1k,
                noi->atten_2k,
                noi->atten_4k,
                noi->atten_8k,
                noi->remark,
                "厂家"
            };

            QStringList data_refl = {
                QString::number(tableWidget_noise->rowCount() + 1),                
                noi->model,
                noi->type,
                noi->size,
                noi->brand,
                noi->refl_63,
                noi->refl_125,
                noi->refl_250,
                noi->refl_500,
                noi->refl_1k,
                noi->refl_2k,
                noi->refl_4k,
                noi->refl_8k,
                noi->remark,
                noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget_noise, data_noise);
            addRowToTable(tableWidget_atten, data_atten);
            addRowToTable(tableWidget_refl, data_refl);

            componentManager.addComponent(QSharedPointer<Other_send_terminal_noise>(noi.release()),"其他送风末端");
        }
    }

}

void Widget::on_pushButton_other_send_terminal_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_other_send_terminal, ui->tableWidget_other_send_terminal_atten, ui->tableWidget_other_send_terminal_refl,"其他送风末端");
}


void Widget::on_pushButton_other_send_terminal_atten_add_clicked()
{
    on_pushButton_other_send_terminal_add_clicked();
}


void Widget::on_pushButton_other_send_terminal_atten_del_clicked()
{
    on_pushButton_other_send_terminal_del_clicked();
}


void Widget::on_pushButton_other_send_terminal_refl_add_clicked()
{
    on_pushButton_other_send_terminal_add_clicked();
}


void Widget::on_pushButton_other_send_terminal_refl_del_clicked()
{
    on_pushButton_other_send_terminal_del_clicked();
}

//修改按钮
void Widget::on_pushButton_other_send_terminal_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_other_send_terminal;
    QTableWidget* tableWidget_noise = ui->tableWidget_other_send_terminal;
    QTableWidget* tableWidget_atten = ui->tableWidget_other_send_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_other_send_terminal_refl;
    Other_send_terminal_noise *noi = new Other_send_terminal_noise();

    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
        &noi->remark,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    QVector<QString*> items_atten = {        
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
        &noi->remark,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_refl = {        
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->remark,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<Other_send_terminal_noise, Dialog_other_send_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Other_send_terminal_noise>(new Other_send_terminal_noise(*noi)),"置换通风末端");
    }
}

void Widget::on_pushButton_other_send_terminal_atten_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_other_send_terminal_atten;
    QTableWidget* tableWidget_noise = ui->tableWidget_other_send_terminal;
    QTableWidget* tableWidget_atten = ui->tableWidget_other_send_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_other_send_terminal_refl;
    Other_send_terminal_noise *noi = new Other_send_terminal_noise();

    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
        &noi->remark,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    QVector<QString*> items_atten = {
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
        &noi->remark,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_refl = {
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->remark,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<Other_send_terminal_noise, Dialog_other_send_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Other_send_terminal_noise>(new Other_send_terminal_noise(*noi)),"置换通风末端");
    }
}

void Widget::on_pushButton_other_send_terminal_refl_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_other_send_terminal_refl;
    QTableWidget* tableWidget_noise = ui->tableWidget_other_send_terminal;
    QTableWidget* tableWidget_atten = ui->tableWidget_other_send_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_other_send_terminal_refl;
    Other_send_terminal_noise *noi = new Other_send_terminal_noise();

    int table_id_col = 1;
    QString table_id = "";
    QVector<QString*> items_noise = {
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->noi_total,
        &noi->remark,
    };
    int cols_noise[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    QVector<QString*> items_atten = {
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->atten_63,
        &noi->atten_125,
        &noi->atten_250,
        &noi->atten_500,
        &noi->atten_1k,
        &noi->atten_2k,
        &noi->atten_4k,
        &noi->atten_8k,
        &noi->remark,
    };
    int cols_atten[] = {2,3,4,5,6,7,8,9,10,11,12,13,14};

    QVector<QString*> items_refl = {
        &noi->model,
        &noi->type,
        &noi->size,
        &noi->brand,
        &noi->refl_63,
        &noi->refl_125,
        &noi->refl_250,
        &noi->refl_500,
        &noi->refl_1k,
        &noi->refl_2k,
        &noi->refl_4k,
        &noi->refl_8k,
        &noi->remark,
        &noi->getMode
    };
    int cols_refl[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};


    for (int row = 0; row < currentTableWidget->rowCount(); ++row)
    {
        noiseRevision<Other_send_terminal_noise, Dialog_other_send_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Other_send_terminal_noise>(new Other_send_terminal_noise(*noi)),"置换通风末端");
    }
}
#pragma endregion}
/**********其他送风末端**********/





/**********静压箱**********/
#pragma region "stack_static_box"{
//初始化表格
void Widget::initTableWidget_static_box()
{
    int colCount = 13;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    initTableWidget(ui->tableWidget_static_box, headerText, columnWidths, colCount);

}

void Widget::on_pushButton_static_box_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_static_box;
    Dialog_static_box *dialog = new Dialog_static_box(this);
    std::unique_ptr<Static_box> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Static_box>(std::move(*static_cast<Static_box*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data = {
                noi->table_id,                
                noi->model,
                noi->brand,
                noi->noi_63,
                noi->noi_125,
                noi->noi_250,
                noi->noi_500,
                noi->noi_1k,
                noi->noi_2k,
                noi->noi_4k,
                noi->noi_8k,
                noi->getMode,
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data);

            componentManager.addComponent(QSharedPointer<Static_box>(noi.release()),"静压箱");
        }
    }
}


void Widget::on_pushButton_static_box_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_static_box, 1, "静压箱");
}

//修改按钮
void Widget::on_pushButton_static_box_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_static_box;
    Static_box *noi = new Static_box();
    QString table_id = "";
    int table_id_col = 1;
    QVector<QString*> items = {
        &noi->model,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->getMode
    };
    int cols[] = {2,3,4,5,6,7,8,9,10,11,12};

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        noiseRevision<Static_box, Dialog_static_box>(tableWidget, row, noi, items, cols, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Static_box>(new Static_box(*noi)),"静压箱");
    }
}
#pragma endregion}
/**********静压箱**********/



/**********风道多分支**********/
#pragma region "stack_duct_with_multi_ranc"{
void Widget::initTableWidegt_duct_with_multi_ranc()
{
    int colCount = 13;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    initTableWidget(ui->tableWidget_duct_with_multi_ranc, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_duct_with_multi_ranc_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_duct_with_multi_ranc;
    Dialog_duct_with_multi_ranc *dialog = new Dialog_duct_with_multi_ranc(this);
    std::unique_ptr<Multi_ranc_atten> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Multi_ranc_atten>(std::move(*static_cast<Multi_ranc_atten*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data = {
               noi->table_id,               
               noi->model,
               noi->brand,
               noi->noi_63,
               noi->noi_125,
               noi->noi_250,
               noi->noi_500,
               noi->noi_1k,
               noi->noi_2k,
               noi->noi_4k,
               noi->noi_8k,
               noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data);

            componentManager.addComponent(QSharedPointer<Multi_ranc_atten>(noi.release()),"风道多分支");
        }
    }

}


void Widget::on_pushButton_duct_with_multi_ranc_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_duct_with_multi_ranc, 1,"风道多分支");
}

//修改按钮
void Widget::on_pushButton_duct_with_multi_ranc_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_duct_with_multi_ranc;
    Multi_ranc_atten *noi = new Multi_ranc_atten();
    QString table_id = "";
    int table_id_col = 1;
    QVector<QString*> items = {
        &noi->model,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->getMode
    };
    int cols[] = {2,3,4,5,6,7,8,9,10,11,12};

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        noiseRevision<Multi_ranc_atten, Dialog_duct_with_multi_ranc>(tableWidget, row, noi, items, cols, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Multi_ranc_atten>(new Multi_ranc_atten(*noi)),"风道多分支");
    }
}

#pragma endregion}
/**********风道多分支**********/




/**********三通衰减**********/
#pragma region "stack_tee_atten"{
void Widget::initTableWidegt_tee()
{
    int colCount = 13;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    initTableWidget(ui->tableWidget_tee, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_tee_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_tee;
    Dialog_tee *dialog = new Dialog_tee(this);
    std::unique_ptr<Tee_atten> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Tee_atten>(std::move(*static_cast<Tee_atten*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data = {
               QString::number(tableWidget->rowCount() + 1),               
               noi->model,
               noi->brand,
               noi->noi_63,
               noi->noi_125,
               noi->noi_250,
               noi->noi_500,
               noi->noi_1k,
               noi->noi_2k,
               noi->noi_4k,
               noi->noi_8k,
               noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data);
            componentManager.addComponent(QSharedPointer<Tee_atten>(noi.release()),"三通");
        }
    }

}


void Widget::on_pushButton_tee_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_tee, 1,"三通");
}

//修改按钮
void Widget::on_pushButton_tee_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_tee;
    Tee_atten *noi = new Tee_atten();
    QString table_id = "";
    int table_id_col = 1;
    QVector<QString*> items = {
        &noi->model,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->getMode
    };
    int cols[] = {2,3,4,5,6,7,8,9,10,11,12};

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        noiseRevision<Tee_atten, Dialog_tee>(tableWidget, row, noi, items, cols, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Tee_atten>(new Tee_atten(*noi)),"三通衰减");
    }
}


#pragma endregion}
/**********三通衰减**********/







/**********直管衰减**********/
#pragma region "stack_pipe_atten"{
void Widget::initTableWidget_pipe()
{
    int colCount = 13;

    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "类型" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 70, 55, 55, 55, 55, 55, 55, 55, 55, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_pipe, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_pipe_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_pipe;
    Dialog_pipe *dialog = new Dialog_pipe(this);
    std::unique_ptr<Pipe_atten> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Pipe_atten>(std::move(*static_cast<Pipe_atten*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data = {
               noi->table_id,
               noi->model,
               noi->type,
               noi->noi_63,
               noi->noi_125,
               noi->noi_250,
               noi->noi_500,
               noi->noi_1k,
               noi->noi_2k,
               noi->noi_4k,
               noi->noi_8k,
               noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data);
            componentManager.addComponent(QSharedPointer<Pipe_atten>(noi.release()),"直管");
        }
    }

}


void Widget::on_pushButton_pipe_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_pipe, 1, "直管");
}

//修改按钮
void Widget::on_pushButton_pipe_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_duct_with_multi_ranc;
    Pipe_atten *noi = new Pipe_atten();
    QString table_id = "";
    int table_id_col = 1;
    QVector<QString*> items = {
        &noi->model,
        &noi->type,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->getMode
    };
    int cols[] = {2,3,4,5,6,7,8,9,10,11,12};

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        noiseRevision<Pipe_atten, Dialog_pipe>(tableWidget, row, noi, items, cols, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Pipe_atten>(new Pipe_atten(*noi)),"直管衰减");
    }
}


#pragma endregion}
/**********直管衰减**********/





/**********弯头衰减**********/
#pragma region "stack_elbow"{
void Widget::initTableWidget_elbow()
{
    int colCount = 13;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "类型" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 70, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_elbow, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_elbow_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_elbow;
    Dialog_elbow *dialog = new Dialog_elbow(this);
    std::unique_ptr<Elbow_atten> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Elbow_atten>(std::move(*static_cast<Elbow_atten*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data = {
               noi->table_id,
               noi->model,
               noi->type,
               noi->noi_63,
               noi->noi_125,
               noi->noi_250,
               noi->noi_500,
               noi->noi_1k,
               noi->noi_2k,
               noi->noi_4k,
               noi->noi_8k,
               noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data);
            componentManager.addComponent(QSharedPointer<Elbow_atten>(noi.release()),"弯头");
        }
    }
}


void Widget::on_pushButton_elbow_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_elbow, 1,"弯头");
}

//修改按钮
void Widget::on_pushButton_elbow_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_elbow;
    Elbow_atten *noi = new Elbow_atten();
    QString table_id = "";
    int table_id_col = 1;
    QVector<QString*> items = {
        &noi->model,
        &noi->type,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->getMode
    };
    int cols[] = {2,3,4,5,6,7,8,9,10,11,12};

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        noiseRevision<Elbow_atten, Dialog_elbow>(tableWidget, row, noi, items, cols, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Elbow_atten>(new Elbow_atten(*noi)),"弯头衰减");
    }
}

#pragma endregion}
/**********弯头衰减**********/



/**********变径衰减**********/
#pragma region "stack_reducer_atten"{
void Widget::initTableWidget_reducer()
{
    int colCount = 13;

    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "类型" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 70, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_reducer, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_reducer_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_reducer;
    Dialog_reducer *dialog = new Dialog_reducer(this);
    std::unique_ptr<Reducer_atten> noi;

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Reducer_atten>(std::move(*static_cast<Reducer_atten*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data = {
               noi->table_id,
               noi->model,
               noi->type,
               noi->noi_63,
               noi->noi_125,
               noi->noi_250,
               noi->noi_500,
               noi->noi_1k,
               noi->noi_2k,
               noi->noi_4k,
               noi->noi_8k,
               noi->getMode
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data);
            componentManager.addComponent(QSharedPointer<Reducer_atten>(noi.release()),"变径");
        }
    }

}


void Widget::on_pushButton_reducer_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_reducer, 1,"变径");
}

//修改按钮
void Widget::on_pushButton_reducer_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_reducer;
    Reducer_atten *noi = new Reducer_atten();
    QString table_id = "";
    int table_id_col = 1;
    QVector<QString*> items = {
        &noi->model,
        &noi->type,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
        &noi->getMode
    };
    int cols[] = {2,3,4,5,6,7,8,9,10,11,12};

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        noiseRevision<Reducer_atten, Dialog_reducer>(tableWidget, row, noi, items, cols, table_id_col, table_id);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Reducer_atten>(new Reducer_atten(*noi)),"变径衰减");
    }
}

#pragma endregion}
/**********变径衰减**********/





/**********消声器**********/
#pragma region "silencer"{
void Widget::initTableWidget_silencer()
{
    this->initTableWidget_circular_silencer();
    this->initTableWidget_circular_silencerEb();
    this->initTableWidget_rect_silencer();
    this->initTableWidget_rect_silencerEb();
    ui->stackedWidget_silencer_table->setCurrentWidget(ui->page_circular_silencer);
}

void Widget::initTableWidget_circular_silencer()
{
    int colCount = 13;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_circular_silencer, headerText, columnWidths, colCount);
}

void Widget::initTableWidget_rect_silencer()
{
    int colCount = 13;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_rect_silencer, headerText, columnWidths, colCount);
}

void Widget::initTableWidget_circular_silencerEb()
{
    int colCount = 13;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_circular_silencerEb, headerText, columnWidths, colCount);
}

void Widget::initTableWidget_rect_silencerEb()
{
    int colCount = 13;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
              << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 120, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_rect_silencerEb, headerText, columnWidths, colCount);
}


void Widget::on_pushButton_silencer_add_clicked()
{
    QTableWidget *tableWidget = nullptr;
    Dialog_silencer *dialog = nullptr;
    std::unique_ptr<Silencer_atten> noi;

    if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_circular_silencer)
    {
        tableWidget = ui->tableWidget_circular_silencer;
        dialog = new Dialog_silencer("圆形消音器",this);
    }
    else if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_circular_silencerEb)
    {
        tableWidget = ui->tableWidget_circular_silencerEb;
        dialog = new Dialog_silencer("圆形消音弯头",this);
    }
    else if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_rect_silencer)
    {
        tableWidget = ui->tableWidget_rect_silencer;
        dialog = new Dialog_silencer("矩形消音器",this);
    }
    else if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_rect_silencerEb)
    {
        tableWidget = ui->tableWidget_rect_silencerEb;
        dialog = new Dialog_silencer("矩形消音弯头",this);
    }

    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Silencer_atten>(std::move(*static_cast<Silencer_atten*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() + 1);
        if (noi != nullptr) {
            QStringList data = {
               noi->table_id,               
               noi->model,
               noi->brand,
               noi->noi_63,
               noi->noi_125,
               noi->noi_250,
               noi->noi_500,
               noi->noi_1k,
               noi->noi_2k,
               noi->noi_4k,
               noi->noi_8k,
               "厂家"
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data);

            componentManager.addComponent(QSharedPointer<Silencer_atten>(noi.release()),"消声器");
        }
    }
}

void Widget::on_pushButton_silencer_del_clicked()
{
    QTableWidget *tableWidget = nullptr;
    if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_circular_silencer)
    {
        tableWidget = ui->tableWidget_circular_silencer;
    }
    else if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_circular_silencerEb)
    {
        tableWidget = ui->tableWidget_circular_silencerEb;
    }
    else if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_rect_silencer)
    {
        tableWidget = ui->tableWidget_rect_silencer;
    }
    else if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_rect_silencerEb)
    {
        tableWidget = ui->tableWidget_rect_silencerEb;
    }

    deleteRowFromTable(tableWidget, 1,"消声器");
}

void Widget::on_pushButton_silencer_revise_clicked()
{
    QTableWidget* tableWidget = nullptr;
    QString name = "";
    QString table_id = "";
    int table_id_col = 1;
    if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_circular_silencer)
    {
        tableWidget = ui->tableWidget_circular_silencer;
        name = "圆形消音器";
    }
    else if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_circular_silencerEb)
    {
        tableWidget = ui->tableWidget_circular_silencerEb;
        name = "圆形消音弯头";
    }
    else if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_rect_silencer)
    {
        tableWidget = ui->tableWidget_rect_silencer;
        name = "矩形消音器";
    }
    else if(ui->stackedWidget_silencer_table->currentWidget() == ui->page_rect_silencerEb)
    {
        tableWidget = ui->tableWidget_rect_silencerEb;
        name = "矩形消音弯头";
    }

    Silencer_atten *noi = new Silencer_atten();
    QVector<QString*> items = {        
        &noi->model,
        &noi->brand,
        &noi->noi_63,
        &noi->noi_125,
        &noi->noi_250,
        &noi->noi_500,
        &noi->noi_1k,
        &noi->noi_2k,
        &noi->noi_4k,
        &noi->noi_8k,
    };
    int cols[] = {2,3,4,5,6,7,8,9,10,11};

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        noiseRevision<Silencer_atten, Dialog_silencer>(tableWidget, row, noi, items, cols, table_id_col, table_id, name);
        componentManager.updateRevisedComponent(table_id, QSharedPointer<Silencer_atten>(new Silencer_atten(*noi)),"消声器");
    }

    delete noi;  // 在这里删除 noi 对象
}

void Widget::on_pushButton_circular_silencer_table_clicked()
{
    ui->stackedWidget_silencer_table->setCurrentWidget(ui->page_circular_silencer);
    ui->pushButton_circular_silencer_table->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_circular_silencerEb_table->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect_silencer_table->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect_silencerEb_table->setStyleSheet("QPushButton { background-color: white; }");
}


void Widget::on_pushButton_rect_silencer_table_clicked()
{
    ui->stackedWidget_silencer_table->setCurrentWidget(ui->page_rect_silencer);
    ui->pushButton_circular_silencer_table->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_circular_silencerEb_table->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect_silencer_table->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_rect_silencerEb_table->setStyleSheet("QPushButton { background-color: white; }");
}


void Widget::on_pushButton_circular_silencerEb_table_clicked()
{
    ui->stackedWidget_silencer_table->setCurrentWidget(ui->page_circular_silencerEb);
    ui->pushButton_circular_silencer_table->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_circular_silencerEb_table->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_rect_silencer_table->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect_silencerEb_table->setStyleSheet("QPushButton { background-color: white; }");
}


void Widget::on_pushButton_rect_silencerEb_table_clicked()
{
    ui->stackedWidget_silencer_table->setCurrentWidget(ui->page_rect_silencerEb);
    ui->pushButton_circular_silencer_table->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_circular_silencerEb_table->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect_silencer_table->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect_silencerEb_table->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
}

#pragma endregion}
/**********消声器**********/

// 右键点击响应
void Widget::TreeWidgetItemPressed_Slot(QTreeWidgetItem *item, int n)
{
    if (qApp->mouseButtons() == Qt::RightButton) // 1、首先判断是否为右键点击
    {
        if(item==item_system_list) //  点击了6.系统清单
        {
            if(actAddzsq==menusystemlist->exec(QCursor::pos()))   // 弹出添加主竖区菜单
            {     
                Dialog_add_zhushuqu *box=new Dialog_add_zhushuqu;
                box->setlabeltext("主竖区");
                if(box->exec()==QDialog::Accepted)
                {
                    QTreeWidgetItem *treeitem=new QTreeWidgetItem(item_system_list,QStringList(box->getname()));
                    QTreeWidgetItem *treeitemcp1=new QTreeWidgetItem(item_room_define,QStringList(box->getname()));
                    QTreeWidgetItem *treeitemcp2=new QTreeWidgetItem(QStringList(box->getname()));

                    int n=item_room_calculate->childCount();    //  保证典型房间一直在最后一行
                    int insertIndex = qMax(0, n - 1);
                    item_room_calculate->insertChild(insertIndex, treeitemcp2);

                    map_zsq67.insert(treeitem,treeitemcp1);  // 保存第6项和第7项主竖区对应关系
                    map_zsq68.insert(treeitem,treeitemcp2);  // 保存第6项和第8项主竖区对应关系
                    vec_zsq.append(treeitem);

                    connect(ui->treeWidget,&QTreeWidget::itemClicked,this,[=](QTreeWidgetItem *item1,int n){
                        if(item1 == treeitemcp1 || item1 == treeitem || item1 == treeitemcp1)     //     点击甲板会显示空白
                            ui->stackedWidget->setCurrentWidget(ui->page_white);
                    });

                    return;
                }
            }
        }
        // 右击主竖区
        if(vec_zsq.contains(item))
        {
            QAction *act=menuzsq->exec(QCursor::pos());
            Dialog_add_zhushuqu *box=new Dialog_add_zhushuqu;
            if(actAddsystem==act)
            {
                box->setlabeltext("系统编号");
                if(box->exec()==QDialog::Accepted)
                {
                    QTreeWidgetItem *treeitemsystem = new QTreeWidgetItem(item,QStringList(box->getname()));
                    vec_system.append(treeitemsystem);
                    QTreeWidgetItem *treeitemcp1=new QTreeWidgetItem(map_zsq67.value(item),QStringList(box->getname()));
                    QTreeWidgetItem *treeitemcp2=new QTreeWidgetItem(map_zsq68.value(item),QStringList(box->getname()));
                    map_system67.insert(treeitemsystem,treeitemcp1);
                    map_system68.insert(treeitemsystem,treeitemcp2);
                    //  添加系统同时在6、7项的系统编号下添加界面
                    //6
                    Form_system_list *form_sl = new Form_system_list(box->getname());
                    ui->stackedWidget->addWidget(form_sl);
                    connect(ui->treeWidget,&QTreeWidget::itemClicked,this,[=](QTreeWidgetItem *item1,int n){
                        if(item1 == treeitemsystem)     //     点击系统会显示系统清单界面
                            ui->stackedWidget->setCurrentWidget(form_sl);
                    });
                    vec_system.append(treeitemsystem);

                    //7
                    Form_room_define *form=new Form_room_define;
                    form->setjiabanItem(treeitemcp2);   // 房间位于这一treeitem下
                    ui->stackedWidget->addWidget(form);
                    // 关联form发出的添加房间、删除房间信号
                    connect(form,SIGNAL(roomadd(QTreeWidgetItem*,QString,int, QString, QString)),
                            this,SLOT(upDateTreeItem8(QTreeWidgetItem*,QString,int, QString, QString)));
                    connect(form,SIGNAL(roomdel(QTreeWidgetItem*,QString)),
                            this,SLOT(delroom(QTreeWidgetItem*,QString)));

                    connect(ui->treeWidget,&QTreeWidget::itemClicked,this,[=](QTreeWidgetItem *item1,int n){
                        if(item1==treeitemcp1)     //     点击甲板会显示添加的页面
                            ui->stackedWidget->setCurrentWidget(form);
                    });
                }
            }
            if(actModzsqname==act)
            {
                box->setlabeltext("修改名称");
                if(box->exec()==QDialog::Accepted)
                {
                    item->setText(0,box->getname());
                    map_zsq67.value(item)->setText(0,box->getname());
                    map_zsq68.value(item)->setText(0,box->getname());
                }
            }
            if(actDelzsq==act)
            {
                delete(item);
                delete(map_zsq67.value(item));
                delete(map_zsq68.value(item));
                map_zsq67.remove(item);
                map_zsq68.remove(item);
                vec_zsq.removeOne(item);
            }
        }
        // 右击系统
        if(vec_system.contains(item))
        {
            QAction *act=menusystem->exec(QCursor::pos());
            if(actModsystemname==act)
            {
                Dialog_add_zhushuqu *box=new Dialog_add_zhushuqu;
                box->setlabeltext("修改系统名称");
                if(box->exec()==QDialog::Accepted)
                {
                    item->setText(0,box->getname());
                    map_system67.value(item)->setText(0,box->getname());
                    map_system68.value(item)->setText(0,box->getname());

                }
            }
            if(actDelsystem==act)
            {
                delete(item);
                delete(map_system67.value(item));
                delete(map_system68.value(item));
                map_system67.remove(item);
                map_system68.remove(item);
                vec_system.removeOne(item);
            }
        }
        // 右击典型舱室
        if(item==item_cabin_classic)
        {
            if(actAddclassicroom==menuclassiccabin->exec(QCursor::pos()))
            {
                Dialog_add_zhushuqu *box=new Dialog_add_zhushuqu;
                box->setlabeltext("典型房间类型");
                if(box->exec()==QDialog::Accepted)
                {
                    QTreeWidgetItem *treeitem=new QTreeWidgetItem(item_cabin_classic,QStringList(box->getname()));
                    vec_classicroom.append(treeitem);
                    room_cal_baseWidget *page = new room_cal_baseWidget(nullptr,box->getname());
                    ui->stackedWidget->addWidget(page);

                    page->addMenuAction(box->getname());

                    connect(ui->treeWidget, &QTreeWidget::itemClicked,this, [=](QTreeWidgetItem *itemClicked, int column) {
                        if (itemClicked == treeitem)
                        {
                            ui->stackedWidget->setCurrentWidget(page);
                        }
                    });
                }
            }
        }
        // 右击典型房间
        if(vec_classicroom.contains(item))
        {
            //典型房间
            QAction *act = menuclassicroom->exec(QCursor::pos());
            if(actModclassicroomname==act)
            {
                Dialog_add_zhushuqu *box=new Dialog_add_zhushuqu;
                box->setlabeltext("修改房间名称");
                if(box->exec()==QDialog::Accepted)
                {
                    item->setText(0,box->getname());
                }
            }
            if(actDelclassicroom==act)
            {
                delete(item);
                vec_classicroom.removeOne(item);

            }
        }
        // 右击主风管
        if(vec_zfg.contains(item))
        {
            QAction *act=menuzfg->exec(QCursor::pos());
            if(actModzfgname==act)
            {
                Dialog_add_zhushuqu *box=new Dialog_add_zhushuqu;
                box->setlabeltext("修改主风管名称");
                if(box->exec()==QDialog::Accepted)
                {
                    item->setText(0,box->getname());
                }
            }
            if(actDelzfg==act)
            {
                delete(item);
                vec_zfg.removeOne(item);
            }
        }

    }
}

//  更新第八项房间底下的主风管信息 房间编号，主风管数量
void Widget::upDateTreeItem8(QTreeWidgetItem *item,QString roomid,int num, QString jiaban, QString limit) //
{
    QTreeWidgetItem *treeitemfj=new QTreeWidgetItem(item,QStringList(roomid));
    QString systemName = item->text(0);     //获取系统编号
    QString zhushuqu = item->parent()->text(0);     //获取主竖区
    for(int i=0;i<num;i++)
    {
        // 创建 房间主风管 页面对象
        room_cal_baseWidget *page = new room_cal_baseWidget;
        page->setInfo(zhushuqu,jiaban,roomid,limit,QString::number(num));   //设置信息
        page->setSystemName(systemName);   //设置系统名
        // 保存 房间编号下的主风管page
        map_roomid_zfgpage[roomid].push_back(page);

        // 将页面添加到堆栈窗口部件
        ui->stackedWidget->addWidget(page);

        // 这里的主风管还要插入对应page页面
        QTreeWidgetItem *treeitemfg = new QTreeWidgetItem(treeitemfj,QStringList("主风管"+QString::number(i+1)));
        vec_zfg.append(treeitemfg);     // 保存主风管ID
        map_zfg_pag.insert(treeitemfg,page);

        // 关联页面和子项
        connect(ui->treeWidget, &QTreeWidget::itemClicked,this, [=](QTreeWidgetItem *itemClicked, int column) {
            if (itemClicked == treeitemfg)
            {
                // 设置当前页面为对应的页面
                ui->stackedWidget->setCurrentWidget(page);
                if(page->flag_firstopen)
                {
                    Dialog_add_zhushuqu *box=new Dialog_add_zhushuqu;
                    box->setlabeltext("主风管命名");
                    if(box->exec()==QDialog::Accepted)
                    {
                        treeitemfg->setText(0,box->getname());
                        page->setMainDuctNumber(box->getname());    //room_cal_table设置名称
                    }
                    page->flag_firstopen=0;
                }
            }
        });
    }
    //主风管汇总
    QTreeWidgetItem *treeitemtotals=new QTreeWidgetItem(treeitemfj,QStringList("房间噪音"));

    //在这里写关联界面
    room_cal_total *page = new room_cal_total;

    ui->stackedWidget->addWidget(page);
    connect(ui->treeWidget, &QTreeWidget::itemClicked,this, [=](QTreeWidgetItem *itemClicked, int column) {
        if (itemClicked == treeitemtotals)
        {
            // 设置当前页面为对应的页面
            ui->stackedWidget->setCurrentWidget(page);
        }
    });

}

void Widget::delroom(QTreeWidgetItem* item_system,QString roomid)
{
    map_roomid_zfgpage.remove(roomid);      //删除 房间编号下对应的主风管page页面 容器记录

    for(int i=0;i<item_system->childCount();i++)         // 遍历房间
    {
        if(item_system->child(i)->text(0)==roomid) // 删除房间
        {
            QTreeWidgetItem* itemfj=item_system->child(i);
            for(int j=0;j<itemfj->childCount();j++) // 首先删除房间下的主风管以及页面
            {
                delete map_zfg_pag.value(itemfj->child(j)); //释放主风管关联的page
                map_zfg_pag.remove(itemfj->child(j));      // 删除记录
                vec_zfg.removeOne(itemfj->child(j));
            }
            delete item_system->child(i);            // 系统下的房间子项全部移走

        }
    }
}

void Widget::initRightButtonMenu()
{
    menusystemlist = new QMenu(this);
    menuzsq = new QMenu(this);
    menusystem = new QMenu(this);
    menuclassiccabin = new QMenu(this);
    menuclassicroom = new QMenu(this);
    menucalroom = new QMenu(this);
    menuzfg = new QMenu(this);

    actAddzsq = new QAction("添加主竖区");
    actAddsystem = new QAction("添加系统");
    actModzsqname = new QAction("修改主竖区名称");
    actDelzsq = new QAction("删除主竖区");
    actModsystemname = new QAction("修改系统名称");
    actDelsystem = new QAction("删除系统");

    actAddzfg = new QAction("添加主风管");
    actModzfgname = new QAction("修改主风管名称");
    actDelzfg = new QAction("删除主风管");

    actAddclassicroom = new QAction("添加典型房间");
    actModclassicroomname = new QAction("修改房间名称");
    actDelclassicroom = new QAction("删除房间");

    menusystemlist->addAction(actAddzsq);
    menuzsq->addAction(actAddsystem);
    menuzsq->addAction(actModzsqname);
    menuzsq->addAction(actDelzsq);
    menusystem->addAction(actModsystemname);
    menusystem->addAction(actDelsystem);
    menuclassiccabin->addAction(actAddclassicroom);
    menuclassicroom->addAction(actModclassicroomname);
    menuclassicroom->addAction(actDelclassicroom);
    menucalroom->addAction(actAddzfg);
    menuzfg->addAction(actModzfgname);
    menuzfg->addAction(actDelzfg);

    // 关联树Item的点击信号，但是在槽中判断是不是右键
    connect(ui->treeWidget, SIGNAL(itemPressed(QTreeWidgetItem*, int)),
            this, SLOT(TreeWidgetItemPressed_Slot(QTreeWidgetItem*, int)));
}




/**********报表**********/
#pragma region "report"{

//封面录入
void Widget::on_pushButton_cover_entry_clicked()
{
    ui->lineEdit_image_boat_name->setText(ui->lineEdit_boat_name->text());
    ui->lineEdit_image_drawing_name->setText(ui->lineEdit_drawing_name->text());
    ui->lineEdit_image_drawing_number->setText(ui->lineEdit_drawing_number->text());

    QStringList marks{
        "project_name",
        "drawing_name",
        "drawing_number"
    };

    QStringList inputData{
        ui->lineEdit_boat_name->text(),
        ui->lineEdit_drawing_name->text(),
        ui->lineEdit_drawing_number->text()
    };

    // 检查是否已经打开 Word 文档，如果没有则打开
    if (!wordEngine->isOpen()) {
        wordEngine->open(reportPath);
    }
    wordEngine->setBatchMarks(marks,inputData);
    wordEngine->save();
}
//封面清空
void Widget::on_pushButton_cover_clear_clicked()
{
    ui->lineEdit_image_boat_name->setText("船名");
    ui->lineEdit_image_drawing_name->setText("图名");
    ui->lineEdit_image_drawing_number->setText("图编");
    ui->lineEdit_boat_name->clear();
    ui->lineEdit_drawing_name->clear();
    ui->lineEdit_drawing_number->clear();
}
//目录(页眉)录入
void Widget::on_pushButton_dictionary_entry_clicked()
{
    ui->lineEdit_image_header_project_name->setText(ui->lineEdit_header_project_name->text());
    ui->lineEdit_image_header_drawing_name->setText(ui->lineEdit_header_drawing_name->text());
    ui->lineEdit_image_header_designer_name->setText(ui->lineEdit_header_designer_name->text());
}
//目录(页眉)清空
void Widget::on_pushButton_dictionary_clear_clicked()
{
    ui->lineEdit_image_header_project_name->setText("项目名称");
    ui->lineEdit_image_header_drawing_name->setText("计算书名");
    ui->lineEdit_image_header_designer_name->setText("设计方名称");
    ui->lineEdit_header_project_name->clear();
    ui->lineEdit_header_drawing_name->clear();
    ui->lineEdit_header_designer_name->clear();
}
//项目概述1.1插入
void Widget::on_pushButton_project_overview_entry_clicked()
{
    QTextCursor cursor = ui->plainTextEdit_project_overview_image->textCursor();
    cursor.movePosition(QTextCursor::Start);

    // 查找第一部分标题
    int firstSectionIndex = ui->plainTextEdit_project_overview_image->toPlainText().indexOf("1.1 项目概述：");
    if (firstSectionIndex != -1) {
        // 将光标移动到第一部分标题的下一行行首
        cursor.setPosition(firstSectionIndex + QString("1.1 项目概述：").length(), QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::Down);
        cursor.movePosition(QTextCursor::StartOfLine);
    }

    // 查找第二部分标题
    int secondSectionIndex = ui->plainTextEdit_project_overview_image->toPlainText().indexOf("1.2 参考图纸清单：");
    if (secondSectionIndex != -1) {
        // 将光标移动到第二部分标题的上一行行末
        cursor.setPosition(secondSectionIndex - 1, QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::EndOfLine);
    }

    // 删除两个标题之间的内容
    cursor.setPosition(firstSectionIndex + QString("1.1 项目概述：").length(), QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    // 插入一行空行
    cursor.insertText("\n");

    /****上面是删除****/

    cursor = ui->plainTextEdit_project_overview_image->textCursor();
    cursor.movePosition(QTextCursor::End);

    // 查找小节标题
    int subsectionIndex = ui->plainTextEdit_project_overview_image->toPlainText().indexOf("1.1 项目概述：");
    if (subsectionIndex != -1) {
        // 将光标移动到小节标题的下一行行尾
        cursor.setPosition(subsectionIndex + QString("1.1 项目概述：").length());
        cursor.movePosition(QTextCursor::EndOfLine);
        cursor.movePosition(QTextCursor::Down);
    }

    // 获取要插入的文本
    QString originalText = ui->plainTextEdit_project_overview->toPlainText();

    // 在每一段的开头插入两个空格
    QString indentedText = originalText.replace("\n", "\n    ");

    // 在首行额外缩进两个字
    indentedText.prepend("    ");

    // 插入内容
    cursor.insertText(indentedText);
}

//项目概述1.1清空
void Widget::on_pushButton_project_overview_clear_clicked()
{
    ui->plainTextEdit_project_overview->clear();

    QTextCursor cursor = ui->plainTextEdit_project_overview_image->textCursor();
    cursor.movePosition(QTextCursor::Start);

    // 查找第一部分标题
    int firstSectionIndex = ui->plainTextEdit_project_overview_image->toPlainText().indexOf("1.1 项目概述：");
    if (firstSectionIndex != -1) {
        // 将光标移动到第一部分标题的下一行行首
        cursor.setPosition(firstSectionIndex + QString("1.1 项目概述：").length(), QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::Down);
        cursor.movePosition(QTextCursor::StartOfLine);
    }

    // 查找第二部分标题
    int secondSectionIndex = ui->plainTextEdit_project_overview_image->toPlainText().indexOf("1.2 参考图纸清单：");
    if (secondSectionIndex != -1) {
        // 将光标移动到第二部分标题的上一行行末
        cursor.setPosition(secondSectionIndex - 1, QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::EndOfLine);
    }

    // 删除两个标题之间的内容
    cursor.setPosition(firstSectionIndex + QString("1.1 项目概述：").length(), QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    // 插入一行空行
    cursor.insertText("\n");
}

//项目概述1.2插入
void Widget::on_pushButton_reference_drawing_entry_clicked()
{
    QTextCursor cursor = ui->plainTextEdit_project_overview_image->textCursor();
    cursor.movePosition(QTextCursor::Start);

    // 查找第二部分标题
    int secondSectionIndex = ui->plainTextEdit_project_overview_image->toPlainText().indexOf("1.2 参考图纸清单：");
    if (secondSectionIndex != -1) {
        // 将光标移动到第二部分标题的下一行行尾
        cursor.setPosition(secondSectionIndex + QString("1.2 参考图纸清单：").length(), QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::Down);
        cursor.movePosition(QTextCursor::StartOfLine);
    }

    // 删除第二部分标题下一行行尾到文本末尾的内容
    cursor.movePosition(QTextCursor::End);

    // 删除两个标题之间的内容
    cursor.setPosition(secondSectionIndex + QString("1.2 参考图纸清单：").length(), QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    // 插入一行空行
    cursor.insertText("\n");

    cursor = ui->plainTextEdit_project_overview_image->textCursor();
    cursor.movePosition(QTextCursor::Start);

    // 查找小节标题
    int subsectionIndex = ui->plainTextEdit_project_overview_image->toPlainText().indexOf("1.2 参考图纸清单：");
    if (subsectionIndex != -1) {
        // 将光标移动到小节标题的下一行行尾
        cursor.setPosition(subsectionIndex + QString("1.2 参考图纸清单：").length());
        cursor.movePosition(QTextCursor::EndOfLine);
        cursor.movePosition(QTextCursor::Down);
    }

    // 获取要插入的文本
    //QString originalText = ui->plainTextEdit_reference_drawing->toPlainText();

    // 在每一段的开头插入两个空格
    //QString indentedText = originalText.replace("\n", "\n    ");

    // 在首行额外缩进两个字
    //indentedText.prepend("    ");

    // 插入内容
    //cursor.insertText(indentedText);
}

//项目概述1.2清空
void Widget::on_pushButton_reference_drawing_clear_clicked()
{
    //ui->plainTextEdit_reference_drawing->clear();

    QTextCursor cursor = ui->plainTextEdit_project_overview_image->textCursor();
    cursor.movePosition(QTextCursor::Start);

    // 查找第二部分标题
    int secondSectionIndex = ui->plainTextEdit_project_overview_image->toPlainText().indexOf("1.2 参考图纸清单：");
    if (secondSectionIndex != -1) {
        // 将光标移动到第二部分标题的下一行行尾
        cursor.setPosition(secondSectionIndex + QString("1.2 参考图纸清单：").length(), QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::Down);
        cursor.movePosition(QTextCursor::StartOfLine);
    }

    // 删除第二部分标题下一行行尾到文本末尾的内容
    cursor.movePosition(QTextCursor::End);

    // 删除两个标题之间的内容
    cursor.setPosition(secondSectionIndex + QString("1.2 参考图纸清单：").length(), QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    // 插入一行空行
    cursor.insertText("\n");
}

//噪音要求录入
void Widget::on_pushButton_noise_require_entry_clicked()
{
    ui->plainTextEdit_noise_require_image->clear();

    QTextCursor cursor = ui->plainTextEdit_noise_require_image->textCursor();
    cursor.movePosition(QTextCursor::End);

    // 获取要插入的文本
    QString originalText = ui->plainTextEdit_noise_require->toPlainText();

    // 在每一段的开头插入两个空格
    QString indentedText = originalText.replace("\n", "\n    ");

    // 在首行额外缩进两个字
    indentedText.prepend("    ");

    // 插入内容
    cursor.insertText(indentedText);
}

//噪音要求清空
void Widget::on_pushButton_noise_require_clear_clicked()
{
    ui->plainTextEdit_noise_require_image->clear();
    ui->plainTextEdit_noise_require->clear();
    ui->plainTextEdit_noise_require_image->insertPlainText("要求来源依据：规格书，规范等");
}

//噪音要求表格录入
void Widget::on_pushButton_noise_require_table_entry_clicked()
{
    ui->tableWidget_noise_require->setRowCount(0);
    ui->tableWidget_noise_require->setRowCount(ui->tableWidget_noi_limit->rowCount());

    for (int row = 0; row < ui->tableWidget_noi_limit->rowCount(); ++row) {
        for (int column = 1; column < ui->tableWidget_noi_limit->columnCount(); ++column) {
            QTableWidgetItem *sourceItem = ui->tableWidget_noi_limit->item(row, column);

            QTableWidgetItem* newItem;
            if (sourceItem) {
                // 创建一个新项目并将其文本设置为源项目的文本
                newItem = new QTableWidgetItem(sourceItem->text());
            } else {
                // 创建一个没有源项目文本的新项目
                newItem = new QTableWidgetItem();
            }

            ui->tableWidget_noise_require->setItem(row, column - 1, newItem);
        }
    }
}

//房间选择依据
void Widget::on_pushButton_choose_basis_entry_clicked()
{
    ui->plainTextEdit_report_choose_basis_image->clear();

    QTextCursor cursor = ui->plainTextEdit_report_choose_basis_image->textCursor();
    cursor.movePosition(QTextCursor::End);

    // 获取要插入的文本
    QString originalText = ui->plainTextEdit_report_choose_basis->toPlainText();

    // 在每一段的开头插入两个空格
    QString indentedText = originalText.replace("\n", "\n    ");

    // 在首行额外缩进两个字
    indentedText.prepend("    ");

    // 插入内容
    cursor.insertText(indentedText);
}

//房间选择依据
void Widget::on_pushButton_choose_basis_clear_clicked()
{
    ui->plainTextEdit_report_choose_basis_image->clear();
    ui->plainTextEdit_report_choose_basis->clear();
    ui->plainTextEdit_report_choose_basis_image->insertPlainText("房间选择依据说明");
}

void Widget::initTableWidget_system_list()
{
    int colCount = 4;
    // 设置表头标题
    QStringList headerText;
    headerText << "系统编号" << "空调器" << "独立排风机" << "公共区域风机盘管";
    // 设置每列的宽度
    int columnWidths[] = {1, 1, 1, 1};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_system_list, headerText, columnWidths, colCount);
}

void Widget::initTableWidget_report_cal_room()
{
    int colCount = 8;
    // 设置表头标题
    QStringList headerText;
    headerText << "主竖区" << "甲板" << "系统编号" << "房间编号" << "房间类型" << "主风管数量" << "噪音限值dB(A)" << "房间计算类型";
    // 设置每列的宽度
    int columnWidths[] = {180, 180, 180, 180, 180, 180, 180, 179};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_report_cal_room, headerText, columnWidths, colCount);
}
#pragma endregion}
/**********报表**********/




/**************事件处理************/
#pragma region "event" {
//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对鼠标事件的重写
//表格中鼠标逻辑

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        canmove = 1; // 设置可以移动窗口的标志
        last = event->globalPos(); // 记录鼠标的当前位置
    }
    // 调用父类的事件处理函数
    QWidget::mousePressEvent(event);
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    if(canmove==1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        last = e->globalPos();
        move(x()+dx, y()+dy);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *e)
{
    if(canmove==1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        move(x()+dx, y()+dy);
        canmove=0;
    }
}

void Widget::keyPressEvent(QKeyEvent* e)
{
    // 清除QtableWidget_fan_noi的焦点
    ui->tableWidget_fan_noi->clearFocus();

    int currentRow = ui->tableWidget_fan_noi->currentRow();
    int currentColumn = ui->tableWidget_fan_noi->currentColumn();
    QTableWidgetItem* currentItem = ui->tableWidget_fan_noi->item(currentRow, currentColumn);

    switch (e->key()) {
        case Qt::Key_Up:
            if (currentRow > 0) {
                ui->tableWidget_fan_noi->setCurrentCell(currentRow - 1, currentColumn);
            }
            break;
        case Qt::Key_Down:
            if (currentRow < ui->tableWidget_fan_noi->rowCount() - 1) {
                ui->tableWidget_fan_noi->setCurrentCell(currentRow + 1, currentColumn);
            }
            break;
        case Qt::Key_Left:
            if (currentColumn > 0) {
                ui->tableWidget_fan_noi->setCurrentCell(currentRow, currentColumn - 1);
            }
            break;
        case Qt::Key_Right:
            if (currentColumn < ui->tableWidget_fan_noi->columnCount() - 1) {
                ui->tableWidget_fan_noi->setCurrentCell(currentRow, currentColumn + 1);
            }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            // 进入当前单元格的编辑状态并允许输入
            ui->tableWidget_fan_noi->editItem(currentItem);
            break;
        default:
            QWidget::keyPressEvent(e);
            break;
    }
}

bool Widget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == ui->tableWidget_fan_noi && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        int currentColumn = ui->tableWidget_fan_noi->currentColumn();

        if (keyEvent->key() == Qt::Key_Left)
        {
            // 处理左箭头键，切换到前一个单元格
            if (currentColumn > 0) {
                ui->tableWidget_fan_noi->setCurrentCell(ui->tableWidget_fan_noi->currentRow(), currentColumn - 1);
            }
            return true;  // 事件已处理
        }
        else if (keyEvent->key() == Qt::Key_Right) {
            // 处理右箭头键，切换到后一个单元格
            if (currentColumn < ui->tableWidget_fan_noi->columnCount() - 1) {
                ui->tableWidget_fan_noi->setCurrentCell(ui->tableWidget_fan_noi->currentRow(), currentColumn + 1);
            }
            return true;  // 事件已处理
        }

    }

    return QWidget::eventFilter(obj, event);
}

void Widget::on_min_clicked()//点击最小化按钮
{
    this->showMinimized();
}

void Widget::on_max_clicked()//点击最大化按钮
{
    static int n=0;
    if(n==0)
    {
        this->showMaximized();
        n=1;
    }
    else if(n==1)
    {
        this->showNormal();
        n=0;
    }
}

void Widget::on_close_clicked()//点击关闭按钮
{
    this->close();
}

#pragma endregion }
/**************事件处理************/





/**************树列表************/
void Widget::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(current == item_prj_info)     //工程信息
    {
        ui->stackedWidget->setCurrentWidget(ui->page_prj_info);
    }
    else if(current == item_fan_noise)      //风机噪音
    {
        ui->stackedWidget->setCurrentWidget(ui->page_noise_src);
    }
    else if(current == item_fan_coil_noise) //风机盘管噪音
    {
        ui->stackedWidget->setCurrentWidget(ui->page_fanCoil_noi);
    }
    else if(current == item_aircondition_noise_single_fan) //风机盘管噪音
    {
        ui->stackedWidget->setCurrentWidget(ui->page_air_noi_single_fan);
    }
    else if(current == item_aircondition_noise_double_fan) //风机盘管噪音
    {
        ui->stackedWidget->setCurrentWidget(ui->page_air_noi_double_fan);
    }
    else if(current == item_VAV_terminal)   //变风量末端噪音
    {
        ui->stackedWidget->setCurrentWidget(ui->page_VAV_terminal);
    }
    else if(current == item_circular_damper)   //圆形调风门
    {
        ui->stackedWidget->setCurrentWidget(ui->page_circular_damper);
    }
    else if(current == item_rect_damper)   //方形调风门
    {
        ui->stackedWidget->setCurrentWidget(ui->page_rect_damper);
    }
    else if(current == item_air_diff)       //布风器+散流器
    {
        ui->stackedWidget->setCurrentWidget(ui->page_air_diff);
    }
    else if(current == item_pump_send_tuyere)   //抽/送风头
    {
        ui->stackedWidget->setCurrentWidget(ui->page_pump_send_tuyere);
    }
    else if(current == item_staticBox_grille)  //回风箱+格栅
    {
        ui->stackedWidget->setCurrentWidget(ui->page_staticBox_grille);
    }
    else if(current == item_disp_vent_terminal)     //置换通风末端
    {
        ui->stackedWidget->setCurrentWidget(ui->page_disp_vent_terminal);
    }
    else if (current == item_other_send_terminal)       //静压箱孔板送风
    {
        ui->stackedWidget->setCurrentWidget(ui->page_other_send_terminal);
    }
    else if(current == item_static_box)         //静压箱
    {
        ui->stackedWidget->setCurrentWidget(ui->page_static_box);
    }
    else if(current == item_duct_with_multi_ranc)         //风道多分支
    {
        ui->stackedWidget->setCurrentWidget(ui->page_duct_with_multi_ranc);
    }
    else if(current == item_tee_atten)         //三通
    {
        ui->stackedWidget->setCurrentWidget(ui->page_tee);
    }
    else if(current == item_pipe_atten)         //直管
    {
        ui->stackedWidget->setCurrentWidget(ui->page_pipe);
    }
    else if(current == item_elbow_atten)         //直管
    {
        ui->stackedWidget->setCurrentWidget(ui->page_elbow);
    }
    else if(current == item_reducer_atten)         //变径
    {
        ui->stackedWidget->setCurrentWidget(ui->page_reducer);
    }
    else if(current == item_silencer_atten)     //消音器
    {
        ui->stackedWidget->setCurrentWidget(ui->page_silencer);
    }
    else if(current == item_air_diff_terminal_atten)     //布风器散流器末端衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_air_diff_terminal_atten);
    }
    else if(current == item_pump_send_tuyere_terminal_atten)     //抽送风头末端衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_pump_send_tuyere_terminal_atten);
    }
    else if(current == item_staticBox_grille_terminal_atten)     //抽送风头末端衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_staticBox_grille_terminal_atten);
    }
    else if(current == item_disp_vent_terminal_atten)     //置换通风末端衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_disp_vent_terminal_atten);
    }
    else if(current == item_other_send_terminal_atten)     //其他送风末端衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_other_send_terminal_atten);
    }
    else if(current == item_air_diff_relf_atten)     //末端布风器散流器反射衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_air_diff_terminal_refl);
    }
    else if(current == item_pump_send_tuyere_relf_atten)     //抽送风头末端反射衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_pump_send_tuyere_terminal_refl);
    }
    else if(current == item_staticBox_grille_relf_atten)     //回风箱＋格栅末端反射衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_staticBox_grille_terminal_refl);
    }
    else if(current == item_disp_vent_relf_atten)     //置换通风末端反射衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_disp_vent_terminal_refl);
    }
    else if(current == item_other_send_relf_atten)     //其他送风末端反射衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_other_send_terminal_refl);
    }
    else if(current == item_room_less425)     //5.1 体积小于425m³的房间（点噪声源）
    {
        ui->stackedWidget->setCurrentWidget(ui->page_room_less425);
    }
    else if(current == item_room_more425)     //5.2 体积大于425m³的房间（点噪声源）
    {
        ui->stackedWidget->setCurrentWidget(ui->page_room_more425);
    }
    else if(current == item_room_noFurniture)     //5.3 无家具房间（点噪声源）
    {
        ui->stackedWidget->setCurrentWidget(ui->page_room_noFurniture);
    }
    else if(current == item_room_open)     //5.4 室外开敞住所（点噪声源）
    {
        ui->stackedWidget->setCurrentWidget(ui->page_room_open);
    }
    else if(current == item_room_gap_tuyere)     //5.5 条缝风口房间（线噪声源）
    {
        ui->stackedWidget->setCurrentWidget(ui->page_room_gap_tuyere);
    }
    else if(current == item_room_rain)     //抽送风头末端反射衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_room_rain);
    }
    else if(current == item_report_cover)     //报表封面
    {
        ui->stackedWidget->setCurrentWidget(ui->page_report_cover);
    }
    else if(current == item_report_dictionary)     //报表目录
    {
        ui->stackedWidget->setCurrentWidget(ui->page_report_dictionary);
    }
    else if(current == item_report_overview)     //报表项目概述
    {
        ui->stackedWidget->setCurrentWidget(ui->page_report_overview);
    }
    else if(current == item_report_noise_require_basis)     //报表要求来源依据
    {
        ui->stackedWidget->setCurrentWidget(ui->page_report_noise_require_basis);
    }
    else if(current == item_report_noise_require_table)     //报表噪音要求表格
    {
        ui->stackedWidget->setCurrentWidget(ui->page_report_noise_require_table);
    }
    else if(current == item_report_system_list)     //系统清单
    {
        ui->stackedWidget->setCurrentWidget(ui->page_system_list);
    }
    else if(current == item_report_room_choose_basis)     //房间选择依据
    {
        ui->stackedWidget->setCurrentWidget(ui->page_report_cal_room);
    }
    else if(current == item_report_cal_room_table)     //房间选择依据
    {
        ui->stackedWidget->setCurrentWidget(ui->page_report_cal_room_table);
    }
    else if(current == item_room_define || current == item_room_calculate || current == item_system_list)     //设置成空白
    {
        ui->stackedWidget->setCurrentWidget(ui->page_white);
    }
}
/**************树列表************/



/*
日期: 2024-3-23
作者: JJH
环境: win10 QT5.14.2 MinGW32
参数：要提取的表格，输出器件名+时间戳
功能: 提取tablewidget的数据（除去第一二列）导出数据到execl表格
*/
void Widget::SaveExeclData(QTableWidget *table,QString component_name)
{
    // 获取当前时间
       QDateTime currentDateTime = QDateTime::currentDateTime();
       QString defaultFileName = component_name+"_" + currentDateTime.toString("yyyyMMdd_hhmmss") + ".xlsx";
    //获取保存路径
       QString filepath=QFileDialog::getSaveFileName(this,tr("Save"),defaultFileName,tr(" (*.xlsx)"));
       if(!filepath.isEmpty()){
           QAxObject *excel = new QAxObject(this);
           //连接Excel控件
           excel->setControl("Excel.Application");
           if (excel->isNull()) {
               QMessageBox::critical(this, tr("错误"), tr("未能创建 Excel 对象，请安装 Microsoft Excel。"));
               delete excel;
               return;
           }
           //不显示窗体
           excel->dynamicCall("SetVisible (bool Visible)","false");
           //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
           excel->setProperty("DisplayAlerts", false);
           //获取工作簿集合
           QAxObject *workbooks = excel->querySubObject("WorkBooks");
           //新建一个工作簿
           workbooks->dynamicCall("Add");
           //获取当前工作簿
           QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
           //获取工作表集合
           QAxObject *worksheets = workbook->querySubObject("Sheets");
           //获取工作表集合的工作表1，即sheet1
           QAxObject *worksheet = worksheets->querySubObject("Item(int)",1);

           /*注意事项：
            1.提取值 不要第一二列
            2.设置某行某列,在 Qt 中，数组的索引是从 0 开始的，
              而在 Excel 中，列的索引是从 1 开始的，所以需要进行调整。
            */

           //设置表头值
           for(int i=1;i<table->columnCount()-1;i++)
           {
               if(table->horizontalHeaderItem(i+1) != nullptr) {//空值空指针会使程序崩溃
                   QAxObject *Range = worksheet->querySubObject("Cells(int,int)", 1, i);
                   //调用Excel的函数设置设置表头
                   Range->dynamicCall("SetValue(const QString &)",table->horizontalHeaderItem(i+1)->text());
               }
           }
           //设置表格数据
           for(int i=1;i<table->rowCount()+1;i++)
           {
               for(int j = 1;j<table->columnCount()-1;j++)
               {
                   if(table->item(i-1,j+1) != nullptr) {
                       QAxObject *Range = worksheet->querySubObject("Cells(int,int)", i+1, j);
                       Range->dynamicCall("SetValue(const QString &)",table->item(i-1,j+1)->data(Qt::DisplayRole).toString());
                   }
               }
           }
           workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(filepath));//保存至filepath
           workbook->dynamicCall("Close()");//关闭工作簿
           excel->dynamicCall("Quit()");//关闭excel
           delete excel;
           excel=NULL;

           QMessageBox::information(this,"提示","导出成功",QMessageBox::Yes);
       }
}


/*
日期: 2024-3-23
作者: JJH
环境: win10 QT5.14.2 MinGW32
参数：要导入到的表格
功能: 从Excel中提取数据到tableWidget（第一列自动添加复选框，第二列加递增序号）
*/
void Widget::LoadExeclData(QTableWidget *table)
{
    // 提示用户清空表格
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("注意"), tr("确定清除当前表格再导入新表格"), QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 清空表格
        table->clearContents();
    } else {
        // 用户选择不清空表格，直接返回
        return;
    }

    // 选择要导入的 Excel 文件
    QString filePath = QFileDialog::getOpenFileName(this, tr("打开Excel文件"), ".", tr("Excel Files (*.xls *.xlsx)"));
    if (filePath.isEmpty()) {
        QMessageBox::warning(this,"警告","路径错误！");
        return;
    }

    QAxObject *excel = new QAxObject(this);
    //连接Excel控件
    excel->setControl("Excel.Application");
    if (excel->isNull()) {
        QMessageBox::critical(this, tr("错误"), tr("未能创建 Excel 对象，请安装 Microsoft Excel。"));
        delete excel;
        return;
    }
    excel->dynamicCall("SetVisible(bool)", false); // 不显示 Excel 界面

    QAxObject *workbooks = excel->querySubObject("Workbooks");
    QAxObject *workbook = workbooks->querySubObject("Open(const QString&)", filePath);
    QAxObject *worksheets = workbook->querySubObject("Worksheets");

    // 假设只导入第一个工作表
    QAxObject *worksheet = worksheets->querySubObject("Item(int)", 1);
    QAxObject *usedRange = worksheet->querySubObject("UsedRange");
    QAxObject *rows = usedRange->querySubObject("Rows");
    QAxObject *columns = usedRange->querySubObject("Columns");

    //计算Excel表格行列数
    int rowCount = rows->property("Count").toInt();
    int columnCount = columns->property("Count").toInt();

    // 设置表格的行列数
    table->setRowCount(rowCount-1);// 不算表头的行数
    table->setColumnCount(columnCount+2); //加上一二列

    // 读取数据并填充表格
    for (int row = 0; row < rowCount; ++row) {

        // 添加复选框
        QCheckBox* checkBox = new QCheckBox();
        QWidget* widget = new QWidget();
        widget->setStyleSheet("background-color: #C0C0C0;");
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        table->setCellWidget(row, 0, widget);
        //添加序号
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(row+1));
        table->setItem(row, 1, item);
        item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
        item->setFlags(Qt::ItemIsEditable); // 设置为只读
        item->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色


        for (int col = 0; col < columnCount; ++col) {
            //提取数据时，从Excel的第二行、第一列开始
            QAxObject *cell = worksheet->querySubObject("Cells(int,int)", row+2, col+1);
            QString value = cell->dynamicCall("Value()").toString();

            QTableWidgetItem *item = new QTableWidgetItem(value);
            table->setItem(row, col+2, item);
            item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
            item->setFlags(Qt::ItemIsEditable); // 设置为只读
            item->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
            item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色
            delete cell;
        }
    }

    workbook->dynamicCall("Close()");//关闭工作簿
    excel->dynamicCall("Quit()");//关闭excel
    delete excel;
    excel=NULL;

}


/*
日期: 2024-3-23
作者: JJH
环境: win10 QT5.14.2 MinGW32
参数：要提取的表格，输出器件名+时间戳
功能: 提取噪声源tablewidget的数据（除去第一二列）导出数据到execl表格，并合并某些单元格
*/
void Widget::SaveExeclData_noisourse(QTableWidget *table,QString component_name,int mergeCellfirst,int mergeCelllast)
{
    // 获取当前时间
       QDateTime currentDateTime = QDateTime::currentDateTime();
       QString defaultFileName = component_name+"_" + currentDateTime.toString("yyyyMMdd_hhmmss") + ".xlsx";
    //获取保存路径
       QString filepath=QFileDialog::getSaveFileName(this,tr("Save"),defaultFileName,tr(" (*.xlsx)"));
       if(!filepath.isEmpty()){
           QAxObject *excel = new QAxObject(this);
           //连接Excel控件
           excel->setControl("Excel.Application");
           if (excel->isNull()) {
               QMessageBox::critical(this, tr("错误"), tr("未能创建 Excel 对象，请安装 Microsoft Excel。"));
               delete excel;
               return;
           }
           //不显示窗体
           excel->dynamicCall("SetVisible (bool Visible)","false");
           //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
           excel->setProperty("DisplayAlerts", false);
           //获取工作簿集合
           QAxObject *workbooks = excel->querySubObject("WorkBooks");
           //新建一个工作簿
           workbooks->dynamicCall("Add");
           //获取当前工作簿
           QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
           //获取工作表集合
           QAxObject *worksheets = workbook->querySubObject("Sheets");
           //获取工作表集合的工作表1，即sheet1
           QAxObject *worksheet = worksheets->querySubObject("Item(int)",1);

           /*注意事项：
            1.提取值 不要第一二列
            2.设置某行某列,在 Qt 中，数组的索引是从 0 开始的，
              而在 Excel 中，列的索引是从 1 开始的，所以需要进行调整。
            */

           //设置表头值
           for(int i=1;i<table->columnCount()-1;i++)
           {
               if(table->horizontalHeaderItem(i+1) != nullptr) {//空值空指针会使程序崩溃
                   QAxObject *Range = worksheet->querySubObject("Cells(int,int)", 1, i);
                   //调用Excel的函数设置设置表头
                   Range->dynamicCall("SetValue(const QString &)",table->horizontalHeaderItem(i+1)->text());
               }
           }
           //设置表格数据
           for(int i=1;i<table->rowCount()+1;i++)
           {
               for(int j = 1;j<table->columnCount()-1;j++)
               {
                   if(table->item(i-1,j+1) != nullptr) {
                       QAxObject *Range = worksheet->querySubObject("Cells(int,int)", i+1, j);
                       Range->dynamicCall("SetValue(const QString &)",table->item(i-1,j+1)->data(Qt::DisplayRole).toString());
                   }
               }
           }

           //合并单元格
           for(int i=2;i<table->rowCount()+2;i+=2)
           {
               //Excel 0到mergeCellfirst合并
               for(int j=0;j<mergeCellfirst;j++)
               {
                   QString letter = QChar('A' + j);
                   QString mergeCell = letter + QString::number(i) + ":" + letter + QString::number(i + 1);
                   QAxObject *rangeObject = worksheet->querySubObject("Range(const QString&)", mergeCell);
                   rangeObject->setProperty("MergeCells", true);
                   rangeObject->setProperty("HorizontalAlignment", -4108); // xlCenter
               }
               //mergeCelllast合并
               QString letter = QChar('A' + mergeCelllast-1);
               QString mergeCell = letter + QString::number(i) + ":" + letter + QString::number(i + 1);
               QAxObject *rangeObject = worksheet->querySubObject("Range(const QString&)", mergeCell);
               rangeObject->setProperty("MergeCells", true);
               rangeObject->setProperty("HorizontalAlignment", -4108); // xlCenter
           }

           workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(filepath));//保存至filepath
           workbook->dynamicCall("Close()");//关闭工作簿
           excel->dynamicCall("Quit()");//关闭excel
           delete excel;
           excel=NULL;

           QMessageBox::information(this,"提示","导出成功",QMessageBox::Yes);
       }
}

/*
日期: 2024-3-23
作者: JJH
环境: win10 QT5.14.2 MinGW32
参数：要导入到的表格
功能: 从Excel中提取数据到tableWidget（第一列自动添加复选框，第二列加递增序号）并合并某些单元格
*/
void Widget::LoadExeclData_noisourse(QTableWidget *table,int spancolfirst,int spancollast)
{
    // 提示用户清空表格
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("注意"), tr("确定清除当前表格再导入新表格"), QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 清空表格
        table->clearContents();
    } else {
        // 用户选择不清空表格，直接返回
        return;
    }

    // 选择要导入的 Excel 文件
    QString filePath = QFileDialog::getOpenFileName(this, tr("打开Excel文件"), ".", tr("Excel Files (*.xls *.xlsx)"));
    if (filePath.isEmpty()) {
        QMessageBox::warning(this,"警告","路径错误！");
        return;
    }

    QAxObject *excel = new QAxObject(this);
    //连接Excel控件
    excel->setControl("Excel.Application");
    if (excel->isNull()) {
        QMessageBox::critical(this, tr("错误"), tr("未能创建 Excel 对象，请安装 Microsoft Excel。"));
        delete excel;
        return;
    }
    excel->dynamicCall("SetVisible(bool)", false); // 不显示 Excel 界面

    QAxObject *workbooks = excel->querySubObject("Workbooks");
    QAxObject *workbook = workbooks->querySubObject("Open(const QString&)", filePath);
    QAxObject *worksheets = workbook->querySubObject("Worksheets");

    // 假设只导入第一个工作表
    QAxObject *worksheet = worksheets->querySubObject("Item(int)", 1);
    QAxObject *usedRange = worksheet->querySubObject("UsedRange");
    QAxObject *rows = usedRange->querySubObject("Rows");
    QAxObject *columns = usedRange->querySubObject("Columns");

    //计算Excel表格行列数
    int excel_rowCount = rows->property("Count").toInt();
    int excel_columnCount = columns->property("Count").toInt();

    // 设置表格的行列数
    table->setRowCount(excel_rowCount-1);// 不算表头的行数
    table->setColumnCount(excel_columnCount+2); //加上一二列

    // 读取数据并填充表格
    //以Excel的行列为基准
    for (int i=2;i<=excel_rowCount;i++)
    {
        for(int j=1;j<=excel_columnCount;j++){
            QAxObject *cell = worksheet->querySubObject("Cells(int,int)", i, j);
            QString value = cell->dynamicCall("Value()").toString();

            QTableWidgetItem *item = new QTableWidgetItem(value);
            table->setItem(i-2, j+1, item);
            item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
            item->setFlags(Qt::ItemIsEditable); // 设置为只读
            item->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
            item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色
        }

        // 处理复选框
        QCheckBox* checkBox = new QCheckBox();
        QWidget* widget = new QWidget();
        widget->setStyleSheet("background-color: #C0C0C0;");
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        table->setCellWidget(i-2, 0, widget);
    }

    //合并
    int spancolf=spancolfirst;
    int spancoll=spancollast;
    int row=1;
    for(int i = 0; i < table->rowCount(); i+=2)
    {
        for (int j=0;j<table->columnCount();j++) {
            if(j <= spancolf || j >= spancoll)
            {
                table->setSpan(i, j, 2, 1);
            }
        }
        //添加序号
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(row++));
        table->setItem(i, 1, item);
        item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
        item->setFlags(Qt::ItemIsEditable); // 设置为只读
        item->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色
    }

    workbook->dynamicCall("Close()");//关闭工作簿
    excel->dynamicCall("Quit()");//关闭excel
    delete excel;
    excel=NULL;

}

/******************导入导出表格********************/
//噪声源
void Widget::on_pushButton_fanNoi_input_clicked(){
    LoadExeclData_noisourse(ui->tableWidget_fan_noi,6,17);
}

void Widget::on_pushButton_fanNoi_output_clicked(){
    SaveExeclData_noisourse(ui->tableWidget_fan_noi,"风机",5,16);
}

void Widget::on_pushButton_fanCoil_noi_input_clicked(){
    LoadExeclData_noisourse(ui->tableWidget_fanCoil_noi,6,17);
}

void Widget::on_pushButton_fanCoil_noi_output_clicked(){
    SaveExeclData_noisourse(ui->tableWidget_fanCoil_noi,"风机盘管",5,16);
}

void Widget::on_pushButton_air_noi_single_fan_input_clicked(){
    //空调器单风机
    LoadExeclData_noisourse(ui->tableWidget_air_noi_single_fan,7,18);
}

void Widget::on_pushButton_air_noi_single_fan_output_clicked(){
    SaveExeclData_noisourse(ui->tableWidget_air_noi_single_fan,"空调器单风机",6,17);
}

void Widget::on_pushButton_air_noi_double_fan_input_clicked(){
    //空调器多风机
}

void Widget::on_pushButton_air_noi_double_fan_output_clicked(){

}

//其他器件
void Widget::on_pushButton_VAV_terminal_input_clicked(){
    LoadExeclData(ui->tableWidget_VAV_terminal);
}

void Widget::on_pushButton_VAV_terminal_output_clicked(){
    SaveExeclData(ui->tableWidget_VAV_terminal,"变风量末端");
}

void Widget::on_pushButton_circular_damper_input_clicked(){
    LoadExeclData(ui->tableWidget_circular_damper);
}

void Widget::on_pushButton_circular_damper_output_clicked(){
    SaveExeclData(ui->tableWidget_circular_damper,"圆形调风门");
}

void Widget::on_pushButton_rect_damper_input_clicked(){
    LoadExeclData(ui->tableWidget_rect_damper);
}

void Widget::on_pushButton_rect_damper_output_clicked(){
    SaveExeclData(ui->tableWidget_rect_damper,"方形调风门");
}

void Widget::on_pushButton_air_diff_input_clicked(){
    LoadExeclData(ui->tableWidget_air_diff);
}

void Widget::on_pushButton_air_diff_output_clicked(){
    SaveExeclData(ui->tableWidget_air_diff,"布风器+散流器");
}

void Widget::on_pushButton_pump_send_input_clicked(){
    //询问是送风头还是抽风头
    QMessageBox messageBox;
    messageBox.setWindowTitle("提示");
    messageBox.setText("导入抽风头还是送风头？");

    // 添加自定义按钮，并设置按钮文本
    QPushButton *b1 = messageBox.addButton("抽风头", QMessageBox::ActionRole);
    QPushButton *b2 = messageBox.addButton("送风头", QMessageBox::ActionRole);
    QPushButton *b5 = messageBox.addButton(QMessageBox::Abort);
    b5->setText("取消");

    // 显示消息框并等待用户响应
    messageBox.exec();

    // 根据用户响应执行相应的操作
    if (messageBox.clickedButton() == b1) {
        LoadExeclData(ui->tableWidget_pump_tuyere);
    } else if (messageBox.clickedButton() == b2) {
        LoadExeclData(ui->tableWidget_send_tuyere);
    }
}

void Widget::on_pushButton_pump_send_output_clicked(){
    //询问是送风头还是抽风头
    QMessageBox messageBox;
    messageBox.setWindowTitle("提示");
    messageBox.setText("导出抽风头还是送风头？");

    // 添加自定义按钮，并设置按钮文本
    QPushButton *b1 = messageBox.addButton("抽风头", QMessageBox::ActionRole);
    QPushButton *b2 = messageBox.addButton("送风头", QMessageBox::ActionRole);
    QPushButton *b5 = messageBox.addButton(QMessageBox::Abort);
    b5->setText("取消");

    // 显示消息框并等待用户响应
    messageBox.exec();

    // 根据用户响应执行相应的操作
    if (messageBox.clickedButton() == b1) {
        SaveExeclData(ui->tableWidget_pump_tuyere,"抽风头");
    } else if (messageBox.clickedButton() == b2) {
        SaveExeclData(ui->tableWidget_send_tuyere,"送风头");
    }
}

void Widget::on_pushButton_staticBox_grille_input_clicked(){
    LoadExeclData(ui->tableWidget_staticBox_grille);
}

void Widget::on_pushButton_staticBox_grille_output_clicked(){
    SaveExeclData(ui->tableWidget_staticBox_grille,"静压箱+格栅");
}


void Widget::on_pushButton_disp_vent_terminal_input_clicked(){
    LoadExeclData(ui->tableWidget_disp_vent_terminal);
}

void Widget::on_pushButton_disp_vent_terminal_output_clicked()
{
    SaveExeclData(ui->tableWidget_disp_vent_terminal,"置换通风末端");
}

void Widget::on_pushButton_other_send_terminal_input_clicked(){
    LoadExeclData(ui->tableWidget_other_send_terminal);
}

void Widget::on_pushButton_other_send_terminal_output_clicked(){
    SaveExeclData(ui->tableWidget_other_send_terminal,"其他送风末端");
}

void Widget::on_pushButton_static_box_input_clicked(){
    LoadExeclData(ui->tableWidget_static_box);
}

void Widget::on_pushButton_static_box_output_clicked(){
    SaveExeclData(ui->tableWidget_static_box,"静压箱");
}

void Widget::on_pushButton_duct_with_multi_ranc_input_clicked(){
    LoadExeclData(ui->tableWidget_duct_with_multi_ranc);
}

void Widget::on_pushButton_duct_with_multi_ranc_output_clicked(){
    SaveExeclData(ui->tableWidget_duct_with_multi_ranc,"风道多分支");
}

void Widget::on_pushButton_tee_input_clicked(){
    LoadExeclData(ui->tableWidget_tee);
}

void Widget::on_pushButton_tee_output_clicked(){
    SaveExeclData(ui->tableWidget_tee,"三通");
}

void Widget::on_pushButton_pipe_input_clicked(){
    LoadExeclData(ui->tableWidget_pipe);
}

void Widget::on_pushButton_pipe_output_clicked(){
    SaveExeclData(ui->tableWidget_pipe,"直管");
}

void Widget::on_pushButton_elbow_input_clicked(){
    LoadExeclData(ui->tableWidget_elbow);
}

void Widget::on_pushButton_elbow_output_clicked(){
    SaveExeclData(ui->tableWidget_elbow,"弯头");
}

void Widget::on_pushButton_reducer_input_clicked(){
    LoadExeclData(ui->tableWidget_reducer);
}

void Widget::on_pushButton_reducer_output_clicked(){
    SaveExeclData(ui->tableWidget_reducer,"变径");
}

void Widget::on_pushButton_silencer_input_clicked(){
    //询问哪个消音器
    QMessageBox msgBox;
    msgBox.setWindowTitle("提示");
    msgBox.setText("导入哪种消音器？");

    QPushButton *b1 = msgBox.addButton("圆形消音器", QMessageBox::ActionRole);
    QPushButton *b2 = msgBox.addButton("矩形消音器", QMessageBox::ActionRole);
    QPushButton *b3 = msgBox.addButton("圆形消音弯头", QMessageBox::ActionRole);
    QPushButton *b4 = msgBox.addButton("矩形消音弯头", QMessageBox::ActionRole);
    QPushButton *b5 = msgBox.addButton(QMessageBox::Abort);
    b5->setText("取消");

    msgBox.exec();

    if (msgBox.clickedButton() == b1) {
        LoadExeclData(ui->tableWidget_circular_silencer);
    } else if (msgBox.clickedButton() == b2) {
        LoadExeclData(ui->tableWidget_rect_silencer);
    } else if (msgBox.clickedButton() == b3) {
        LoadExeclData(ui->tableWidget_circular_silencerEb);
    } else if (msgBox.clickedButton() == b4) {
        LoadExeclData(ui->tableWidget_rect_silencerEb);
    }
}

void Widget::on_pushButton_silencer_output_clicked(){
    //询问哪个消音器
    QMessageBox msgBox;
    msgBox.setWindowTitle("提示");
    msgBox.setText("导出哪种消音器？");

    QPushButton *b1 = msgBox.addButton("圆形消音器", QMessageBox::ActionRole);
    QPushButton *b2 = msgBox.addButton("矩形消音器", QMessageBox::ActionRole);
    QPushButton *b3 = msgBox.addButton("圆形消音弯头", QMessageBox::ActionRole);
    QPushButton *b4 = msgBox.addButton("矩形消音弯头", QMessageBox::ActionRole);
    QPushButton *b5 = msgBox.addButton(QMessageBox::Abort);
    b5->setText("取消");
    msgBox.exec();

    if (msgBox.clickedButton() == b1) {
        SaveExeclData(ui->tableWidget_circular_silencer,"圆形消音器");
    } else if (msgBox.clickedButton() == b2) {
        SaveExeclData(ui->tableWidget_rect_silencer,"矩形消音器");
    } else if (msgBox.clickedButton() == b3) {
        SaveExeclData(ui->tableWidget_circular_silencerEb,"圆形消音弯头");
    } else if (msgBox.clickedButton() == b4) {
        SaveExeclData(ui->tableWidget_rect_silencerEb,"矩形消音弯头");
    }


}


