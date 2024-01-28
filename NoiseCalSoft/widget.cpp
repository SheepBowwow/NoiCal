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
#include "global_constant.h"

#include <iostream>
#include <string>
#include <regex>

ComponentManager& componentManager = ComponentManager::getInstance();
WordEngine* wordEngine = new WordEngine();

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->page_login);
    this->initTableWidget_noi_limit();
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
    this->initTableWidget_air_noi();
    this->initTableWidget_reducer();
    this->initTableWidget_elbow();
    this->initRightButtonMenu();
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
        confirmationMessage += QString::number(deleteRowNum == 1 ? (row + 1) : ((row + 1) / 2)) + "\n"; // 从1开始计数
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
            if(deleteRowNum == 2)
            {
                tableWidget->removeRow(row - 1);
            }
            if(deleteRowNum == 2)
                componentManager.del_and_updateTableID((row + 1) / 2, componentName);
            else
                componentManager.del_and_updateTableID(row + 1, componentName);
        }


        // 重新编号
        for (int row = 0; row < tableWidget->rowCount(); ++row) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(deleteRowNum == 1 ? (row + 1) : (row / 2 + 1)));
            tableWidget->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(Qt::ItemIsEditable);
            item->setBackground(QBrush(Qt::lightGray));
            item->setData(Qt::ForegroundRole, QColor(70, 70, 70));
        }
        if(deleteRowNum == 2)
        {
            for(int row = 0; row < tableWidget->rowCount(); row += 2)
            {
                for(int i = 0; i < tableWidget->columnCount(); i++)
                {
                    if(tableWidget == ui->tableWidget_fanCoil_noi)
                    {
                        if(i < 8 || i > 17)
                        {
                            tableWidget->setSpan(row, i, 2, 1);
                        }
                    }
                    else
                    {
                        if(i < 7 || i > 16)
                        {
                            tableWidget->setSpan(row, i, 2, 1);
                        }
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
    QSet<int> selectedRows;
    QVector<QTableWidget *> tableWidgets= {tableWidget_noise,tableWidget_atten,tableWidget_refl};
    for(int i = 0; i < tableWidgets.size(); i++)
    {
        for (int row = 0; row < tableWidgets[i]->rowCount(); ++row) {
            QWidget* widget = tableWidgets[i]->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
            QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

            if (checkBox && checkBox->isChecked()) {
                selectedRows.insert(row);
            }
        }
    }

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

template <typename NoiType, typename DialogType>
void Widget::noiseRevision(QTableWidget *tableWidget, int row,NoiType *noi, QVector<QString*>& items, int* cols, QString name)
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
                   int row,NoiType *noi, QVector<QString*>& items_noise, QVector<QString*>& items_atten, QVector<QString*>& items_refl,
                   int* cols_noise, int* cols_atten, int* cols_refl, QString name)
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
    item_aircondition_noise = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("空调器噪音"));   //1.3空调器噪音

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
    ui->treeWidget->addTopLevelItem(item_report);   //报表

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
    for (int row = 0; row < ui->tableWidget_noi_limit->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_noi_limit->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
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
            ui->tableWidget_noi_limit->removeRow(row);
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
void Widget::initTableWidget_noi_limit()
{
    int colCount = 5;
    // 设置表头标题
    QStringList headerText;
    headerText << "" << "序号" << "房间类型" << "噪声限值dB(A)" << "处所类型";
    // 设置每列的宽度
    int columnWidths[] = {30, 38, 130, 190, 125};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_noi_limit, headerText, columnWidths, colCount);
    colCount = 4;
    QStringList headerText1;
    headerText1 << "序号" << "房间类型" << "噪声限值dB(A)" << "处所类型";
    int columnWidths1[] = {38, 130, 190, 125};
    initTableWidget(ui->tableWidget_noise_require, headerText1, columnWidths1, colCount);
}

//当表格item改变，rooms跟着改变
void Widget::on_tableWidget_noi_limit_itemChanged(QTableWidgetItem *item)
{
    if(!item || item->text().isEmpty())
        return;
    int rowCount = ui->tableWidget_noi_limit->rowCount();

    rooms.clear();

    for(int i = 0; i < rowCount; i++)
    {
        Room room;
        if(!ui->tableWidget_noi_limit->item(i,2) || !ui->tableWidget_noi_limit->item(i,3) ||!ui->tableWidget_noi_limit->item(i,4) || !ui->tableWidget_noi_limit->item(i,5))
            return;
        room.name = ui->tableWidget_noi_limit->item(i,2)->text();
        room.noise = ui->tableWidget_noi_limit->item(i,3)->text();
        room.type = ui->tableWidget_noi_limit->item(i,4)->text();
        rooms.push_back(room);
    }
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
    headerText << "" << "序号" << "编号" << "品牌" << "型号" << "风量" << "静压" << "噪音位置" << "63Hz" << "125Hz" << "250Hz"
               << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源";
    // 设置每列的宽度
    int columnWidths[] = {30, 38, 120, 100, 100, 90, 90, 80, 55, 55, 55, 55, 55, 55, 55, 55, 90, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_fan_noi, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    //buttonToHeader(ui->tableWidget_fan_noi, ui->buttonWidget_fan_noi,
//                      SLOT(onAddButtonFanNoiClicked()), SLOT(onDelButtonFanNoiClicked()));
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
                noi->brand,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
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
                "",
                "",
                "",
                "",
                "",
                "",
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

            componentManager.addComponent(QSharedPointer<Fan_noise>(noi.release()));
            for(int i = 0; i < tableWidget->columnCount(); i++)
            {
                if(tableWidget == ui->tableWidget_fanCoil_noi)
                {
                    if(i < 8 || i > 17)
                    {
                        tableWidget->setSpan(rowCount, i, 2, 1);
                    }
                }
                else
                {
                    if(i < 7 || i > 16)
                    {
                        tableWidget->setSpan(rowCount, i, 2, 1);
                    }
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
    for (int row = 0; row < ui->tableWidget_fan_noi->rowCount(); ++row) {

        QWidget* widget = ui->tableWidget_fan_noi->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked()) {
            Fan_noise *noi = new Fan_noise();
            noi->number = ui->tableWidget_fan_noi->item(row - 1,2)->text();
            noi->brand = ui->tableWidget_fan_noi->item(row - 1,3)->text();
            noi->model = ui->tableWidget_fan_noi->item(row - 1,4)->text();
            noi->air_volume = ui->tableWidget_fan_noi->item(row - 1,5)->text();
            noi->static_pressure = ui->tableWidget_fan_noi->item(row - 1,6)->text();

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
                ui->tableWidget_fan_noi->item(row - 1,2)->setText(noi->number);
                ui->tableWidget_fan_noi->item(row - 1,3)->setText(noi->brand);
                ui->tableWidget_fan_noi->item(row - 1,4)->setText(noi->model);
                ui->tableWidget_fan_noi->item(row - 1,5)->setText(noi->air_volume);
                ui->tableWidget_fan_noi->item(row - 1,6)->setText(noi->static_pressure);

                ui->tableWidget_fan_noi->item(row - 1,8)->setText(noi->noi_in_63);
                ui->tableWidget_fan_noi->item(row - 1,9)->setText(noi->noi_in_125);
                ui->tableWidget_fan_noi->item(row - 1,10)->setText(noi->noi_in_250);
                ui->tableWidget_fan_noi->item(row - 1,11)->setText(noi->noi_in_500);
                ui->tableWidget_fan_noi->item(row - 1,12)->setText(noi->noi_in_1k);
                ui->tableWidget_fan_noi->item(row - 1,13)->setText(noi->noi_in_2k);
                ui->tableWidget_fan_noi->item(row - 1,14)->setText(noi->noi_in_4k);
                ui->tableWidget_fan_noi->item(row - 1,15)->setText(noi->noi_in_8k);
                ui->tableWidget_fan_noi->item(row - 1,16)->setText(noi->noi_in_total);

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
    headerText << "" << "序号" << "品牌" << "类型" << "型号" << "风量" << "静压" << "噪音位置" << "63Hz" << "125Hz" << "250Hz"
               << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源";
    int columnWidths[] = {30, 120, 90, 90, 90, 90, 90, 80, 55, 55, 55, 55, 55, 55, 55, 55, 80, 55};
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
                noi->brand,
                noi->type,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
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
                "",
                "",
                "",
                "",
                "",
                "",
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

            componentManager.addComponent(QSharedPointer<FanCoil_noise>(noi.release()));
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
    for (int row = 0; row < ui->tableWidget_fanCoil_noi->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_fanCoil_noi->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox && checkBox->isChecked()) {
            qDebug() << "row: " << row <<"checkBox" << checkBox->isChecked();
            FanCoil_noise *noi = new FanCoil_noise();
            noi->brand = ui->tableWidget_fanCoil_noi->item(row,2)->text();
            noi->type = ui->tableWidget_fanCoil_noi->item(row,3)->text();
            noi->model = ui->tableWidget_fanCoil_noi->item(row,4)->text();
            noi->air_volume = ui->tableWidget_fanCoil_noi->item(row,5)->text();
            noi->static_pressure = ui->tableWidget_fanCoil_noi->item(row,6)->text();

            noi->noi_in_63 = ui->tableWidget_fanCoil_noi->item(row,8)->text();
            noi->noi_in_125 = ui->tableWidget_fanCoil_noi->item(row,9)->text();
            noi->noi_in_250 = ui->tableWidget_fanCoil_noi->item(row,10)->text();
            noi->noi_in_500 = ui->tableWidget_fanCoil_noi->item(row,11)->text();
            noi->noi_in_1k = ui->tableWidget_fanCoil_noi->item(row,12)->text();
            noi->noi_in_2k = ui->tableWidget_fanCoil_noi->item(row,13)->text();
            noi->noi_in_4k = ui->tableWidget_fanCoil_noi->item(row,14)->text();
            noi->noi_in_8k = ui->tableWidget_fanCoil_noi->item(row,15)->text();
            noi->noi_in_total = ui->tableWidget_fanCoil_noi->item(row,16)->text();

            noi->noi_out_63 = ui->tableWidget_fanCoil_noi->item(row + 1,8)->text();
            noi->noi_out_125 = ui->tableWidget_fanCoil_noi->item(row + 1,9)->text();
            noi->noi_out_250 = ui->tableWidget_fanCoil_noi->item(row + 1,10)->text();
            noi->noi_out_500 = ui->tableWidget_fanCoil_noi->item(row + 1,11)->text();
            noi->noi_out_1k = ui->tableWidget_fanCoil_noi->item(row + 1,12)->text();
            noi->noi_out_2k = ui->tableWidget_fanCoil_noi->item(row + 1,13)->text();
            noi->noi_out_4k = ui->tableWidget_fanCoil_noi->item(row + 1,14)->text();
            noi->noi_out_8k = ui->tableWidget_fanCoil_noi->item(row + 1,15)->text();
            noi->noi_out_total = ui->tableWidget_fanCoil_noi->item(row + 1,16)->text();
            // 创建模态对话框，并设置为模态
            Dialog_fanCoil_noise *fanCoilNoiseDialog = new Dialog_fanCoil_noise(this,row,*noi);
            fanCoilNoiseDialog->setWindowModality(Qt::ApplicationModal);
            fanCoilNoiseDialog->setModal(true);

            // 显示对话框,等待用户操作
            if (fanCoilNoiseDialog->exec() == QDialog::Accepted)
            {
                noi = static_cast<FanCoil_noise*>(fanCoilNoiseDialog->getNoi());
                ui->tableWidget_fanCoil_noi->item(row,2)->setText(noi->brand);
                ui->tableWidget_fanCoil_noi->item(row,3)->setText(noi->type);
                ui->tableWidget_fanCoil_noi->item(row,4)->setText(noi->model);
                ui->tableWidget_fanCoil_noi->item(row,5)->setText(noi->air_volume);
                ui->tableWidget_fanCoil_noi->item(row,6)->setText(noi->static_pressure);

                ui->tableWidget_fanCoil_noi->item(row,8)->setText(noi->noi_in_63);
                ui->tableWidget_fanCoil_noi->item(row,9)->setText(noi->noi_in_125);
                ui->tableWidget_fanCoil_noi->item(row,10)->setText(noi->noi_in_250);
                ui->tableWidget_fanCoil_noi->item(row,11)->setText(noi->noi_in_500);
                ui->tableWidget_fanCoil_noi->item(row,12)->setText(noi->noi_in_1k);
                ui->tableWidget_fanCoil_noi->item(row,13)->setText(noi->noi_in_2k);
                ui->tableWidget_fanCoil_noi->item(row,14)->setText(noi->noi_in_4k);
                ui->tableWidget_fanCoil_noi->item(row,15)->setText(noi->noi_in_8k);
                ui->tableWidget_fanCoil_noi->item(row,16)->setText(noi->noi_in_total);

                ui->tableWidget_fanCoil_noi->item(row + 1,8)->setText(noi->noi_out_63);
                ui->tableWidget_fanCoil_noi->item(row + 1,9)->setText(noi->noi_out_125);
                ui->tableWidget_fanCoil_noi->item(row + 1,10)->setText(noi->noi_out_250);
                ui->tableWidget_fanCoil_noi->item(row + 1,11)->setText(noi->noi_out_500);
                ui->tableWidget_fanCoil_noi->item(row + 1,12)->setText(noi->noi_out_1k);
                ui->tableWidget_fanCoil_noi->item(row + 1,13)->setText(noi->noi_out_2k);
                ui->tableWidget_fanCoil_noi->item(row + 1,14)->setText(noi->noi_out_4k);
                ui->tableWidget_fanCoil_noi->item(row + 1,15)->setText(noi->noi_out_8k);
                ui->tableWidget_fanCoil_noi->item(row + 1,16)->setText(noi->noi_out_total);
            }
            delete noi;
        }
    }
}

#pragma endregion}
/**********风机盘管噪音**********/





/**********空调器噪音**********/
#pragma region "stack_air_noi"{

//初始化表格
void Widget::initTableWidget_air_noi()
{
    int colCount = 19;

    QStringList headerText;
    headerText << "" << "序号" << "编号" << "品牌" << "型号" << "风量" << "静压" << "类型" << "噪音位置" << "63Hz" << "125Hz" << "250Hz"
               << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源";

    int columnWidths[] = {30, 38, 120, 100, 100, 90, 90, 80, 80, 55, 55, 55, 55, 55, 55, 55, 55, 90, 60};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_air_noi, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_air_noi_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_air_noi;
    Dialog_aircondition_noise *dialog = new Dialog_aircondition_noise(this);
    std::unique_ptr<Aircondition_noise> noi;

    int rowCount = tableWidget->rowCount(); //获取当前行数
    if (dialog->exec() == QDialog::Accepted) {
        noi = std::make_unique<Aircondition_noise>(std::move(*static_cast<Aircondition_noise*>(dialog->getNoi())));
        noi->table_id = QString::number(tableWidget->rowCount() / 2 + 1);
        if (noi != nullptr) {
            QStringList data_send_in = {
                noi->table_id,
                noi->number,
                noi->brand,
                noi->model,
                noi->air_volume,
                noi->static_pressure,
                noi->type,
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
                "",
                "",
                "",
                "",
                "",
                "",
                "",
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

            if(noi->type == "双风机")
            {
                QStringList data_exhaust_in = {
                    noi->table_id,
                    noi->number,
                    noi->brand,
                    noi->model,
                    noi->air_volume,
                    noi->static_pressure,
                    noi->type,
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
                    "",
                    "",
                    "",
                    "",
                    "",
                    "",
                    "",
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
                addRowToTable(tableWidget, data_exhaust_in);
                addRowToTable(tableWidget, data_exhaust_out);
            }

            componentManager.addComponent(QSharedPointer<Aircondition_noise>(noi.release()));
            if(noi->type == "单风机")
            {
                for(int i = 0; i < tableWidget->columnCount(); i++)
                {
                    if(i < 8 || i > 17)
                    {
                        tableWidget->setSpan(rowCount, i, 2, 1);
                    }
                }
            }
            else if(noi->type == "双风机")
            {
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
}


void Widget::on_pushButton_air_noi_del_clicked()
{
    deleteRowFromTable(ui->tableWidget_air_noi, 2, "空调器");
}

//修改按钮
void Widget::on_pushButton_air_noi_revise_clicked()
{
    int colCount = 19;
    QString origin_type;
    for (int row = 0; row < ui->tableWidget_air_noi->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_air_noi->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked()) {
            Aircondition_noise *noi = new Aircondition_noise();
            if(ui->tableWidget_air_noi->item(row - 1,1)->text() == ui->tableWidget_air_noi->item(row - 3,1)->text())
            {
                noi->type = "双风机";
                noi->number = ui->tableWidget_air_noi->item(row - 3,2)->text();
                noi->brand = ui->tableWidget_air_noi->item(row - 3,3)->text();
                noi->model = ui->tableWidget_air_noi->item(row - 3,4)->text();
                noi->air_volume = ui->tableWidget_air_noi->item(row - 3,5)->text();
                noi->static_pressure = ui->tableWidget_air_noi->item(row - 3,6)->text();

                noi->noi_send_in_63 = ui->tableWidget_air_noi->item(row - 3,9)->text();
                noi->noi_send_in_125 = ui->tableWidget_air_noi->item(row - 3,10)->text();
                noi->noi_send_in_250 = ui->tableWidget_air_noi->item(row - 3,11)->text();
                noi->noi_send_in_500 = ui->tableWidget_air_noi->item(row - 3,12)->text();
                noi->noi_send_in_1k = ui->tableWidget_air_noi->item(row - 3,13)->text();
                noi->noi_send_in_2k = ui->tableWidget_air_noi->item(row - 3,14)->text();
                noi->noi_send_in_4k = ui->tableWidget_air_noi->item(row - 3,15)->text();
                noi->noi_send_in_8k = ui->tableWidget_air_noi->item(row - 3,16)->text();
                noi->noi_send_in_total = ui->tableWidget_air_noi->item(row - 3,17)->text();

                noi->noi_send_out_63 = ui->tableWidget_air_noi->item(row - 2,9)->text();
                noi->noi_send_out_125 = ui->tableWidget_air_noi->item(row - 2,10)->text();
                noi->noi_send_out_250 = ui->tableWidget_air_noi->item(row - 2,11)->text();
                noi->noi_send_out_500 = ui->tableWidget_air_noi->item(row - 2,12)->text();
                noi->noi_send_out_1k = ui->tableWidget_air_noi->item(row - 2,13)->text();
                noi->noi_send_out_2k = ui->tableWidget_air_noi->item(row - 2,14)->text();
                noi->noi_send_out_4k = ui->tableWidget_air_noi->item(row - 2,15)->text();
                noi->noi_send_out_8k = ui->tableWidget_air_noi->item(row - 2,16)->text();
                noi->noi_send_out_total = ui->tableWidget_air_noi->item(row - 2,17)->text();

                noi->noi_exhaust_in_63 = ui->tableWidget_air_noi->item(row - 1,9)->text();
                noi->noi_exhaust_in_125 = ui->tableWidget_air_noi->item(row - 1,10)->text();
                noi->noi_exhaust_in_250 = ui->tableWidget_air_noi->item(row - 1,11)->text();
                noi->noi_exhaust_in_500 = ui->tableWidget_air_noi->item(row - 1,12)->text();
                noi->noi_exhaust_in_1k = ui->tableWidget_air_noi->item(row - 1,13)->text();
                noi->noi_exhaust_in_2k = ui->tableWidget_air_noi->item(row - 1,14)->text();
                noi->noi_exhaust_in_4k = ui->tableWidget_air_noi->item(row - 1,15)->text();
                noi->noi_exhaust_in_8k = ui->tableWidget_air_noi->item(row - 1,16)->text();
                noi->noi_exhaust_in_total = ui->tableWidget_air_noi->item(row - 1,17)->text();

                noi->noi_exhaust_out_63 = ui->tableWidget_air_noi->item(row,9)->text();
                noi->noi_exhaust_out_125 = ui->tableWidget_air_noi->item(row,10)->text();
                noi->noi_exhaust_out_250 = ui->tableWidget_air_noi->item(row,11)->text();
                noi->noi_exhaust_out_500 = ui->tableWidget_air_noi->item(row,12)->text();
                noi->noi_exhaust_out_1k = ui->tableWidget_air_noi->item(row,13)->text();
                noi->noi_exhaust_out_2k = ui->tableWidget_air_noi->item(row,14)->text();
                noi->noi_exhaust_out_4k = ui->tableWidget_air_noi->item(row,15)->text();
                noi->noi_exhaust_out_8k = ui->tableWidget_air_noi->item(row,16)->text();
                noi->noi_exhaust_out_total = ui->tableWidget_air_noi->item(row,17)->text();
            }
            else
            {
                noi->type = "单风机";
                noi->number = ui->tableWidget_air_noi->item(row - 1,2)->text();
                noi->brand = ui->tableWidget_air_noi->item(row - 1,3)->text();
                noi->model = ui->tableWidget_air_noi->item(row - 1,4)->text();
                noi->air_volume = ui->tableWidget_air_noi->item(row - 1,5)->text();
                noi->static_pressure = ui->tableWidget_air_noi->item(row - 1,6)->text();

                noi->noi_send_in_63 = ui->tableWidget_air_noi->item(row - 1,9)->text();
                noi->noi_send_in_125 = ui->tableWidget_air_noi->item(row - 1,10)->text();
                noi->noi_send_in_250 = ui->tableWidget_air_noi->item(row - 1,11)->text();
                noi->noi_send_in_500 = ui->tableWidget_air_noi->item(row - 1,12)->text();
                noi->noi_send_in_1k = ui->tableWidget_air_noi->item(row - 1,13)->text();
                noi->noi_send_in_2k = ui->tableWidget_air_noi->item(row - 1,14)->text();
                noi->noi_send_in_4k = ui->tableWidget_air_noi->item(row - 1,15)->text();
                noi->noi_send_in_8k = ui->tableWidget_air_noi->item(row - 1,16)->text();
                noi->noi_send_in_total = ui->tableWidget_air_noi->item(row - 1,17)->text();

                noi->noi_send_out_63 = ui->tableWidget_air_noi->item(row,9)->text();
                noi->noi_send_out_125 = ui->tableWidget_air_noi->item(row,10)->text();
                noi->noi_send_out_250 = ui->tableWidget_air_noi->item(row,11)->text();
                noi->noi_send_out_500 = ui->tableWidget_air_noi->item(row,12)->text();
                noi->noi_send_out_1k = ui->tableWidget_air_noi->item(row,13)->text();
                noi->noi_send_out_2k = ui->tableWidget_air_noi->item(row,14)->text();
                noi->noi_send_out_4k = ui->tableWidget_air_noi->item(row,15)->text();
                noi->noi_send_out_8k = ui->tableWidget_air_noi->item(row,16)->text();
                noi->noi_send_out_total = ui->tableWidget_air_noi->item(row,17)->text();
            }
            origin_type = noi->type;
            // 创建模态对话框，并设置为模态
            Dialog_aircondition_noise *airNoiseDialog = new Dialog_aircondition_noise(this,row,*noi);
            airNoiseDialog->setWindowModality(Qt::ApplicationModal);
            airNoiseDialog->setModal(true);

            // 显示对话框,等待用户操作
            if (airNoiseDialog->exec() == QDialog::Accepted)
            {
                noi = static_cast<Aircondition_noise*>(airNoiseDialog->getNoi());
                if(origin_type == noi->type && noi->type == "单风机")
                {
                    ui->tableWidget_air_noi->item(row - 1,2)->setText(noi->number);
                    ui->tableWidget_air_noi->item(row - 1,3)->setText(noi->brand);
                    ui->tableWidget_air_noi->item(row - 1,4)->setText(noi->model);
                    ui->tableWidget_air_noi->item(row - 1,5)->setText(noi->air_volume);
                    ui->tableWidget_air_noi->item(row - 1,6)->setText(noi->static_pressure);

                    ui->tableWidget_air_noi->item(row - 1,9)->setText(noi->noi_send_in_63);
                    ui->tableWidget_air_noi->item(row - 1,10)->setText(noi->noi_send_in_125);
                    ui->tableWidget_air_noi->item(row - 1,11)->setText(noi->noi_send_in_250);
                    ui->tableWidget_air_noi->item(row - 1,12)->setText(noi->noi_send_in_500);
                    ui->tableWidget_air_noi->item(row - 1,13)->setText(noi->noi_send_in_1k);
                    ui->tableWidget_air_noi->item(row - 1,14)->setText(noi->noi_send_in_2k);
                    ui->tableWidget_air_noi->item(row - 1,15)->setText(noi->noi_send_in_4k);
                    ui->tableWidget_air_noi->item(row - 1,16)->setText(noi->noi_send_in_8k);
                    ui->tableWidget_air_noi->item(row - 1,17)->setText(noi->noi_send_in_total);

                    ui->tableWidget_air_noi->item(row,9)->setText(noi->noi_send_out_63);
                    ui->tableWidget_air_noi->item(row,10)->setText(noi->noi_send_out_125);
                    ui->tableWidget_air_noi->item(row,11)->setText(noi->noi_send_out_250);
                    ui->tableWidget_air_noi->item(row,12)->setText(noi->noi_send_out_500);
                    ui->tableWidget_air_noi->item(row,13)->setText(noi->noi_send_out_1k);
                    ui->tableWidget_air_noi->item(row,14)->setText(noi->noi_send_out_2k);
                    ui->tableWidget_air_noi->item(row,15)->setText(noi->noi_send_out_4k);
                    ui->tableWidget_air_noi->item(row,16)->setText(noi->noi_send_out_8k);
                    ui->tableWidget_air_noi->item(row,17)->setText(noi->noi_send_out_total);
                }
                else if(origin_type == noi->type && noi->type == "双风机")
                {
                    ui->tableWidget_air_noi->item(row - 3,2)->setText(noi->number);
                    ui->tableWidget_air_noi->item(row - 3,3)->setText(noi->brand);
                    ui->tableWidget_air_noi->item(row - 3,4)->setText(noi->model);
                    ui->tableWidget_air_noi->item(row - 3,5)->setText(noi->air_volume);
                    ui->tableWidget_air_noi->item(row - 3,6)->setText(noi->static_pressure);

                    ui->tableWidget_air_noi->item(row - 3,9)->setText(noi->noi_send_in_63);
                    ui->tableWidget_air_noi->item(row - 3,10)->setText(noi->noi_send_in_125);
                    ui->tableWidget_air_noi->item(row - 3,11)->setText(noi->noi_send_in_250);
                    ui->tableWidget_air_noi->item(row - 3,12)->setText(noi->noi_send_in_500);
                    ui->tableWidget_air_noi->item(row - 3,13)->setText(noi->noi_send_in_1k);
                    ui->tableWidget_air_noi->item(row - 3,14)->setText(noi->noi_send_in_2k);
                    ui->tableWidget_air_noi->item(row - 3,15)->setText(noi->noi_send_in_4k);
                    ui->tableWidget_air_noi->item(row - 3,16)->setText(noi->noi_send_in_8k);
                    ui->tableWidget_air_noi->item(row - 3,17)->setText(noi->noi_send_in_total);

                    ui->tableWidget_air_noi->item(row - 2,9)->setText(noi->noi_send_out_63);
                    ui->tableWidget_air_noi->item(row - 2,10)->setText(noi->noi_send_out_125);
                    ui->tableWidget_air_noi->item(row - 2,11)->setText(noi->noi_send_out_250);
                    ui->tableWidget_air_noi->item(row - 2,12)->setText(noi->noi_send_out_500);
                    ui->tableWidget_air_noi->item(row - 2,13)->setText(noi->noi_send_out_1k);
                    ui->tableWidget_air_noi->item(row - 2,14)->setText(noi->noi_send_out_2k);
                    ui->tableWidget_air_noi->item(row - 2,15)->setText(noi->noi_send_out_4k);
                    ui->tableWidget_air_noi->item(row - 2,16)->setText(noi->noi_send_out_8k);
                    ui->tableWidget_air_noi->item(row - 2,17)->setText(noi->noi_send_out_total);

                    ui->tableWidget_air_noi->item(row - 1,9)->setText(noi->noi_exhaust_in_63);
                    ui->tableWidget_air_noi->item(row - 1,10)->setText(noi->noi_exhaust_in_125);
                    ui->tableWidget_air_noi->item(row - 1,11)->setText(noi->noi_exhaust_in_250);
                    ui->tableWidget_air_noi->item(row - 1,12)->setText(noi->noi_exhaust_in_500);
                    ui->tableWidget_air_noi->item(row - 1,13)->setText(noi->noi_exhaust_in_1k);
                    ui->tableWidget_air_noi->item(row - 1,14)->setText(noi->noi_exhaust_in_2k);
                    ui->tableWidget_air_noi->item(row - 1,15)->setText(noi->noi_exhaust_in_4k);
                    ui->tableWidget_air_noi->item(row - 1,16)->setText(noi->noi_exhaust_in_8k);
                    ui->tableWidget_air_noi->item(row - 1,17)->setText(noi->noi_exhaust_in_total);

                    ui->tableWidget_air_noi->item(row,9)->setText(noi->noi_exhaust_out_63);
                    ui->tableWidget_air_noi->item(row,10)->setText(noi->noi_exhaust_out_125);
                    ui->tableWidget_air_noi->item(row,11)->setText(noi->noi_exhaust_out_250);
                    ui->tableWidget_air_noi->item(row,12)->setText(noi->noi_exhaust_out_500);
                    ui->tableWidget_air_noi->item(row,13)->setText(noi->noi_exhaust_out_1k);
                    ui->tableWidget_air_noi->item(row,14)->setText(noi->noi_exhaust_out_2k);
                    ui->tableWidget_air_noi->item(row,15)->setText(noi->noi_exhaust_out_4k);
                    ui->tableWidget_air_noi->item(row,16)->setText(noi->noi_exhaust_out_8k);
                    ui->tableWidget_air_noi->item(row,17)->setText(noi->noi_exhaust_out_total);
                }
                else if(origin_type != noi->type && noi->type == "单风机")
                {
                    //ui->tableWidget_air_noi->
                }
            }
            delete noi;
        }
    }
}

#pragma endregion}
/**********空调器噪音**********/





/**********变风量末端**********/
#pragma region "stack_VAV_terminal"{
//初始化表格
void Widget::initTableWidget_VAV_terminal()
{
    int colCount = 17;
    QStringList headerText;
    headerText<< "" << "序号" << "编号" << "品牌" << "型号" << "阀门开度" << "风量" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 140, 110, 140, 100, 100, 80, 80, 80, 80, 80, 80, 80, 80, 95 ,90};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_VAV_terminal, headerText, columnWidths, colCount);
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
                noi->brand,
                noi->model,
                noi->valve_open_degree,
                noi->air_volume,
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

            // 使用通用函数添加行
            addRowToTable(tableWidget, data);

            componentManager.addComponent(QSharedPointer<VAV_terminal_noise>(noi.release()));
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
    QVector<QString*> items = {
        &noi->number,
        &noi->brand,
        &noi->model,
        &noi->valve_open_degree,
        &noi->air_volume,
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
        noiseRevision<VAV_terminal_noise, Dialog_VAV_terminal>(tableWidget, row, noi, items, cols);
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
    headerText<< "" << "序号" << "品牌" << "型号" << "阀门开度" << "风量" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 100, 120, 100, 100, 65, 65, 65, 65, 65, 65, 65, 65, 90, 70};
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
                noi->brand,
                noi->model,
                noi->angle,
                noi->air_volume,
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

            componentManager.addComponent(QSharedPointer<Circular_damper_noi>(noi.release()));
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
    QVector<QString*> items = {
        &noi->brand,
        &noi->model,
        &noi->angle,
        &noi->air_volume,
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
        noiseRevision<Circular_damper_noi, Dialog_circular_damper>(tableWidget, row, noi, items, cols);
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
    headerText<< "" << "序号" << "品牌" << "型号" << "阀门开度" << "风量" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 110, 130, 110, 110, 65, 65, 65, 65, 65, 65, 65, 65, 90, 70};
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
                noi->brand,
                noi->model,
                noi->angle,
                noi->air_volume,
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

            componentManager.addComponent(QSharedPointer<Rect_damper_noi>(noi.release()));
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
    QVector<QString*> items = {
        &noi->brand,
        &noi->model,
        &noi->angle,
        &noi->air_volume,
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
        noiseRevision<Rect_damper_noi, Dialog_rect_damper>(tableWidget, row, noi, items, cols);
    }
}

#pragma endregion}
/**********方形调风门**********/





/**********布风器+散流器**********/
#pragma region "stack_air_diff"{
//初始化表格
void Widget::initTableWidget_air_diff()
{
    int colCount = 16;
    QStringList headerText;
    headerText<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 130, 250, 130, 130, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_air_diff, headerText, columnWidths, colCount);

    colCount = 15;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_air_diff_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    colCount = 15;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};

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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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

            componentManager.addComponent(QSharedPointer<AirDiff_noise>(noi));
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
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<AirDiff_noise, Dialog_air_diff>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl,"布风器+散流器");
    }
}

void Widget::on_pushButton_air_diff_terminal_atten_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_air_diff_terminal_atten;
    QTableWidget* tableWidget_noise = ui->tableWidget_air_diff;
    QTableWidget* tableWidget_atten = ui->tableWidget_air_diff_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_air_diff_terminal_refl;
    AirDiff_noise *noi = new AirDiff_noise();
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<AirDiff_noise, Dialog_air_diff>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl,"布风器+散流器");
    }
}

void Widget::on_pushButton_air_diff_terminal_refl_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_air_diff_terminal_refl;
    QTableWidget* tableWidget_noise = ui->tableWidget_air_diff;
    QTableWidget* tableWidget_atten = ui->tableWidget_air_diff_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_air_diff_terminal_refl;
    AirDiff_noise *noi = new AirDiff_noise();
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<AirDiff_noise, Dialog_air_diff>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl,"布风器+散流器");
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
    headerText<< "抽" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 130, 250, 130, 130, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90};

    initTableWidget(ui->tableWidget_pump_tuyere, headerText, columnWidths, colCount);

    colCount = 15;
    QStringList headerText_atten;
    headerText_atten<< "抽" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_pump_tuyere_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    colCount = 15;
    QStringList headerText_refl;
    headerText_refl<< "抽" << "序号" << "品牌" << "型号" << "类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 100, 160, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_pump_tuyere_terminal_refl, headerText_refl, refl_columnWidths, colCount);
}

void Widget::initTableWidget_send_tuyere()
{
    int colCount = 16;
    QStringList headerText;
    headerText<< "送" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 130, 250, 130, 130, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90};

    initTableWidget(ui->tableWidget_send_tuyere, headerText, columnWidths, colCount);

    colCount = 15;
    QStringList headerText_atten;
    headerText_atten<< "送" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_send_tuyere_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    colCount = 15;
    QStringList headerText_refl;
    headerText_refl<< "送" << "序号" << "品牌" << "型号" << "类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 100, 160, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};

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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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

            componentManager.addComponent(QSharedPointer<PumpSend_noise>(noi.release()));
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

    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
                                                        cols_noise, cols_atten, cols_refl,typeName);
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
    int colCount = 16;
    QStringList headerText;
    headerText<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 130, 250, 130, 130, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90};
    initTableWidget(ui->tableWidget_staticBox_grille, headerText, columnWidths, colCount);


    colCount = 15;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_staticBox_grille_terminal_atten, headerText_atten, atten_columnWidths, colCount);


    colCount = 15;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};

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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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

            componentManager.addComponent(QSharedPointer<StaticBox_grille_noise>(noi.release()));
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
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<StaticBox_grille_noise, Dialog_staticBox_grille>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
    }
}

void Widget::on_pushButton_staticBox_grille_terminal_refl_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_staticBox_grille_terminal_refl;
    QTableWidget* tableWidget_noise = ui->tableWidget_staticBox_grille;
    QTableWidget* tableWidget_atten = ui->tableWidget_staticBox_grille_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_staticBox_grille_terminal_refl;
    StaticBox_grille_noise *noi = new StaticBox_grille_noise();
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<StaticBox_grille_noise, Dialog_staticBox_grille>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
    }
}

void Widget::on_pushButton_staticBox_grille_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_staticBox_grille;
    QTableWidget* tableWidget_noise = ui->tableWidget_staticBox_grille;
    QTableWidget* tableWidget_atten = ui->tableWidget_staticBox_grille_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_staticBox_grille_terminal_refl;
    StaticBox_grille_noise *noi = new StaticBox_grille_noise();
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<StaticBox_grille_noise, Dialog_staticBox_grille>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
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
    headerText<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 130, 250, 130, 130, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90};

    initTableWidget(ui->tableWidget_disp_vent_terminal, headerText, columnWidths, colCount);

    colCount = 15;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_disp_vent_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    colCount = 15;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};

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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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

            componentManager.addComponent(QSharedPointer<Disp_vent_terminal_noise>(noi.release()));
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
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<Disp_vent_terminal_noise, Dialog_disp_vent_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
    }
}

void Widget::on_pushButton_disp_vent_terminal_atten_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_disp_vent_terminal_refl;
    QTableWidget* tableWidget_noise = ui->tableWidget_disp_vent_terminal;
    QTableWidget* tableWidget_atten = ui->tableWidget_disp_vent_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_disp_vent_terminal_refl;
    Disp_vent_terminal_noise *noi = new Disp_vent_terminal_noise();
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<Disp_vent_terminal_noise, Dialog_disp_vent_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
    }
}

void Widget::on_pushButton_disp_vent_terminal_refl_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_disp_vent_terminal_atten;
    QTableWidget* tableWidget_noise = ui->tableWidget_disp_vent_terminal;
    QTableWidget* tableWidget_atten = ui->tableWidget_disp_vent_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_disp_vent_terminal_refl;
    Disp_vent_terminal_noise *noi = new Disp_vent_terminal_noise();
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<Disp_vent_terminal_noise, Dialog_disp_vent_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
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
    headerText<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "备注";  //表头标题用QStringList来表示

    int columnWidths[] = {40, 50, 130, 250, 110, 110, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90, 180};

    initTableWidget(ui->tableWidget_other_send_terminal, headerText, columnWidths, colCount);

    colCount = 16;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "备注";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 110, 110, 80, 80, 80, 80, 80, 80, 80, 80, 90, 180};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_other_send_terminal_atten, headerText_atten, atten_columnWidths, colCount);


    colCount = 16;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "备注";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 140, 230, 110, 110, 80, 80, 80, 80, 80, 80, 80, 80, 90, 180};

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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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
                noi->brand,
                noi->model,
                noi->type,
                noi->size,
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

            componentManager.addComponent(QSharedPointer<Other_send_terminal_noise>(noi.release()));
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
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<Other_send_terminal_noise, Dialog_other_send_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
    }
}

void Widget::on_pushButton_other_send_terminal_atten_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_other_send_terminal_atten;
    QTableWidget* tableWidget_noise = ui->tableWidget_other_send_terminal;
    QTableWidget* tableWidget_atten = ui->tableWidget_other_send_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_other_send_terminal_refl;
    Other_send_terminal_noise *noi = new Other_send_terminal_noise();
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<Other_send_terminal_noise, Dialog_other_send_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
    }
}

void Widget::on_pushButton_other_send_terminal_refl_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_other_send_terminal_refl;
    QTableWidget* tableWidget_noise = ui->tableWidget_other_send_terminal;
    QTableWidget* tableWidget_atten = ui->tableWidget_other_send_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_other_send_terminal_refl;
    Other_send_terminal_noise *noi = new Other_send_terminal_noise();
    QVector<QString*> items_noise = {
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        &noi->brand,
        &noi->model,
        &noi->type,
        &noi->size,
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
        noiseRevision<Other_send_terminal_noise, Dialog_other_send_terminal>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
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
    headerText<< "" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 100, 120, 80, 80, 80, 80, 80, 80, 80, 80, 90};

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
                noi->brand,
                noi->model,
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

            componentManager.addComponent(QSharedPointer<Static_box>(noi.release()));
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
    QVector<QString*> items = {
        &noi->brand,
        &noi->model,
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
        noiseRevision<Static_box, Dialog_static_box>(tableWidget, row, noi, items, cols);
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
    headerText<< "" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 100, 120, 80, 80, 80, 80, 80, 80, 80, 80, 90};

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
               noi->brand,
               noi->model,
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

            componentManager.addComponent(QSharedPointer<Multi_ranc_atten>(noi.release()));
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
    QVector<QString*> items = {
        &noi->brand,
        &noi->model,
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
        noiseRevision<Multi_ranc_atten, Dialog_duct_with_multi_ranc>(tableWidget, row, noi, items, cols);
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
    headerText<< "" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示

    int columnWidths[] = {40, 50, 100, 120, 80, 80, 80, 80, 80, 80, 80, 80, 90};
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
               noi->brand,
               noi->model,
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
            componentManager.addComponent(QSharedPointer<Tee_atten>(noi.release()));
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
    QVector<QString*> items = {
        &noi->brand,
        &noi->model,
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
        noiseRevision<Tee_atten, Dialog_tee>(tableWidget, row, noi, items, cols);
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
    headerText<< "" << "序号" << "型号" << "类型" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示

    int columnWidths[] = {40, 50, 160, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};

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
            componentManager.addComponent(QSharedPointer<Pipe_atten>(noi.release()));
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
        noiseRevision<Pipe_atten, Dialog_pipe>(tableWidget, row, noi, items, cols);
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
    headerText<< "" << "序号" << "型号" << "类型" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 160, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};
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
            componentManager.addComponent(QSharedPointer<Elbow_atten>(noi.release()));
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
        noiseRevision<Elbow_atten, Dialog_elbow>(tableWidget, row, noi, items, cols);
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
    headerText<< "" << "序号" << "型号" << "类型" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 160, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90};
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
            componentManager.addComponent(QSharedPointer<Reducer_atten>(noi.release()));
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
        noiseRevision<Reducer_atten, Dialog_reducer>(tableWidget, row, noi, items, cols);
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
    headerText<< "圆形" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 40, 120, 370, 82, 82, 82, 82, 82, 82, 82, 82, 90};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_circular_silencer, headerText, columnWidths, colCount);
}

void Widget::initTableWidget_rect_silencer()
{
    int colCount = 13;
    QStringList headerText;
    headerText<< "矩形" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 40, 120, 370, 82, 82, 82, 82, 82, 82, 82, 82, 90};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_rect_silencer, headerText, columnWidths, colCount);
}

void Widget::initTableWidget_circular_silencerEb()
{
    int colCount = 13;
    QStringList headerText;
    headerText<< "圆弯" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 40, 120, 370, 82, 82, 82, 82, 82, 82, 82, 82, 90};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_circular_silencerEb, headerText, columnWidths, colCount);
}

void Widget::initTableWidget_rect_silencerEb()
{
    int colCount = 13;
    QStringList headerText;
    headerText<< "矩弯" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 40, 120, 370, 82, 82, 82, 82, 82, 82, 82, 82, 90};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_rect_silencerEb, headerText, columnWidths, colCount);
}


void Widget::on_pushButton_silencer_add_clicked()
{
    QTableWidget *tableWidget = nullptr;
    Dialog_silencer *dialog = nullptr;
    std::unique_ptr<Silencer_atten> noi;
    const char *addButtonSlot = SLOT(onAddButtonElbowClicked());
    const char *delButtonSlot = SLOT(onDelButtonElbowClicked());

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
               noi->brand,
               noi->model,
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

            componentManager.addComponent(QSharedPointer<Silencer_atten>(noi.release()));
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
        &noi->brand,
        &noi->model,
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
        noiseRevision<Silencer_atten, Dialog_silencer>(tableWidget, row, noi, items, cols, name);
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
                    QTreeWidgetItem *treeitemsystem=new QTreeWidgetItem(item,QStringList(box->getname()));
                    vec_system.append(treeitemsystem);
                    QTreeWidgetItem *treeitemcp1=new QTreeWidgetItem(map_zsq67.value(item),QStringList(box->getname()));
                    QTreeWidgetItem *treeitemcp2=new QTreeWidgetItem(map_zsq68.value(item),QStringList(box->getname()));
                    map_system67.insert(treeitemsystem,treeitemcp1);
                    map_system68.insert(treeitemsystem,treeitemcp2);
                    //  添加系统同时在6、7项的系统编号下添加界面
                    //6
                    Form_system_list *form_sl=new Form_system_list;
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
                    connect(form,SIGNAL(roomadd(QTreeWidgetItem*,QString,int)),
                            this,SLOT(upDateTreeItem8(QTreeWidgetItem*,QString,int)));
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
void Widget::upDateTreeItem8(QTreeWidgetItem *item,QString roomid,int num) //
{
    QTreeWidgetItem *treeitemfj=new QTreeWidgetItem(item,QStringList(roomid));
    for(int i=0;i<num;i++)
    {
        // 创建 房间主风管 页面对象
        room_cal_baseWidget *page = new room_cal_baseWidget;
        // 保存 房间编号下的主风管page
        map_roomid_zfgpage[roomid].push_back(page);

        // 将页面添加到堆栈窗口部件
        ui->stackedWidget->addWidget(page);

        // 这里的主风管还要插入对应page页面
        QTreeWidgetItem *treeitemfg=new QTreeWidgetItem(treeitemfj,QStringList("主风管"+QString::number(i+1)));
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

void Widget::  initRightButtonMenu()
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

void Widget::on_pushButton_choose_report_filePath_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Save Word Document"),
        "",
        tr("Word Documents (*.docx);;All Files (*)"));

    if (filename.isEmpty()) {
        return;
    } else {
        // 将文件路径显示在ui->plainTextEdit中
        ui->plainTextEdit_report_filePath->setPlainText(filename);


        // 构建资源路径
        QString templatePath = "./document/template.docx";
        templatePath.replace("/", "\\");  // 如果你需要将正斜杠替换为反斜杠
        QFile file(templatePath);
        templatePath = QDir(file.fileName()).absolutePath();

        // 打开 Word 模板文件
        wordEngine->openAndCopy(templatePath);
        wordEngine->saveAs(filename);
        wordEngine->close();

        reportPath = filename;
    }
}

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
    QString originalText = ui->plainTextEdit_reference_drawing->toPlainText();

    // 在每一段的开头插入两个空格
    QString indentedText = originalText.replace("\n", "\n    ");

    // 在首行额外缩进两个字
    indentedText.prepend("    ");

    // 插入内容
    cursor.insertText(indentedText);
}

//项目概述1.2清空
void Widget::on_pushButton_reference_drawing_clear_clicked()
{
    ui->plainTextEdit_reference_drawing->clear();

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

void Widget::on_pushButton_choose_basis_clear_clicked()
{
    ui->plainTextEdit_report_choose_basis_image->clear();
    ui->plainTextEdit_report_choose_basis->clear();
    ui->plainTextEdit_report_choose_basis_image->insertPlainText("房间选择依据说明");
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
    else if(current == item_aircondition_noise) //风机盘管噪音
    {
        ui->stackedWidget->setCurrentWidget(ui->page_air_noi);
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
    else if(current == item_room_define || current == item_room_calculate || current == item_system_list)     //设置成空白
    {
        ui->stackedWidget->setCurrentWidget(ui->page_white);
    }
}
/**************树列表************/

