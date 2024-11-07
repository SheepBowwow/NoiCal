#include "widget.h"
#include "ui_widget.h"
/**窗口类**/
#include "inputDialog/dialog_prj_manager.h""
#include "roomDefineForm/room_define_widget.h"
#include "roomDefineForm/systemcomp_list_widget.h"
#include "roomDefineForm/dialog_set_name.h"
#include "roomCal/room_cal_basewidget.h"
#include "roomCal/room_cal_total_widget.h"
#include "roomCal/outer_before_widget.h"
#include "roomCal/outer_after_widget.h"
/**窗口类**/
/**表格**/
#include "componentInpuTableWidget/widget_fan_inputtable.h"
#include "componentInpuTableWidget/widget_fanCoil_inputtable.h"
#include "componentInpuTableWidget/widget_aircondition_singlefan_inputtable.h"
#include "componentInpuTableWidget/widget_aircondition_doublefan_inputtable.h"
#include "componentInpuTableWidget/widget_vav_terminal_inputtable.h"
#include "componentInpuTableWidget/widget_circular_damper_inputtable.h"
#include "componentInpuTableWidget/widget_rect_damper_inputtable.h"
#include "componentInpuTableWidget/widget_air_diff_inputtable.h"
#include "componentInpuTableWidget/widget_pump_send_inputtable.h"
#include "componentInpuTableWidget/widget_static_box_grille_inputtable.h"
#include "componentInpuTableWidget/widget_disp_vent_terminal_inputtable.h"
#include "componentInpuTableWidget/widget_other_send_terminal_inputtable.h"
#include "componentInpuTableWidget/widget_static_box_inputtable.h"
#include "componentInpuTableWidget/widget_multi_ranc_inputtable.h"
#include "componentInpuTableWidget/widget_diffuser_branch_inputtable.h"
#include "componentInpuTableWidget/widget_tee_inputtable.h"
#include "componentInpuTableWidget/widget_pipe_inputtable.h"
#include "componentInpuTableWidget/widget_elbow_inputtable.h"
#include "componentInpuTableWidget/widget_reducer_inputtable.h"
#include "componentInpuTableWidget/widget_silencer_inputtable.h"
/**表格**/
#include <QDebug>
#include <QVector>
#include <QQueue>
#include <QColor>
#include <QTimer>
#include <QStandardItemModel>
#include <QCheckBox>
#include <QSharedPointer>
#include <QFileDialog>
#include <QResource>
#include <QDesktopServices>
#include <string>
#include <regex>
#include <QDateTime>
#include "office/wordengine.h"
#include "project/projectmanager.h"
#include "Component/componentsdb.h"
#include "roomDefineForm/roomcalinfomanager.h"

WordEngine* wordEngine = new WordEngine();

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , componentDB(nullptr)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->page_login);
    this->initTableWidget_noi_limit();
    this->initTableWidget_drawing_list();
    this->setTable();
    this->initTableWidget_system_list();
    this->initTableWidget_report_cal_room();
    this->initTableWidget_project_attachment();

    ProjectManager::getInstance();
    DatabaseManager::getInstance();
    DatabaseManager::getInstance().loadComponentsFromComponentDB();
    connect(&ProjectManager::getInstance(), &ProjectManager::clearTable, this, &Widget::clearTable);
    connect(&ProjectManager::getInstance(), &ProjectManager::loadBasicInfoDone, this, &Widget::setBasicPageWhenSwitchPrj);
    connect(&RoomCalInfoManager::getInstance(), &RoomCalInfoManager::switchWhitePage, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->page_white);
    });
}

Widget::~Widget()
{
    wordEngine->close();
    delete wordEngine;
    delete ui;
}

void Widget::initTableWidget(QTableWidget *tableWidget, const QStringList &headerText, const int *columnWidths, int colCount)
{
    tableWidget->setColumnCount(colCount);
    tableWidget->setRowCount(0);
    tableWidget->setHorizontalHeaderLabels(headerText);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
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
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
}

// 假设 CustomWidget 是你的自定义界面类
void Widget::addWidgetToPage(QWidget* targetPage, Widget_base_inputTable* widgetToAdd) {
    // 检查目标页面是否已经有布局，如果没有，则创建一个新的 QVBoxLayout
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(targetPage->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(targetPage);
        targetPage->setLayout(layout);

        // 设置布局的边距和间距为0
        layout->setContentsMargins(6, 6, 6, 6); // 设置布局的边距
    }

    // 将容器添加到页面的布局中
    layout->addWidget(widgetToAdd);
    //初始化表格
    widgetToAdd->initialize();
}

void Widget::setTable()
{
    addWidgetToPage(ui->page_fan, new Widget_fan_inputTable(false));
    addWidgetToPage(ui->page_fanCoil, new Widget_fanCoil_inputTable(false));
    addWidgetToPage(ui->page_air_noi_single_fan, new Widget_aircondition_singleFan_inputTable(false));
    addWidgetToPage(ui->page_air_noi_double_fan, new Widget_aircondition_doubleFan_inputTable(false));
    addWidgetToPage(ui->page_VAV_terminal, new Widget_VAV_terminal_inputTable(false));
    addWidgetToPage(ui->page_circular_damper, new Widget_Circular_damper_inputTable(false));
    addWidgetToPage(ui->page_rect_damper, new Widget_Rect_damper_inputTable(false));
    addWidgetToPage(ui->page_air_diff, new Widget_air_diff_inputTable(false));
    addWidgetToPage(ui->page_pump, new Widget_Pump_Send_inputTable(false, "抽风头"));
    addWidgetToPage(ui->page_send, new Widget_Pump_Send_inputTable(false, "送风头"));
    addWidgetToPage(ui->page_staticBox_grille, new Widget_static_box_grille_inputTable(false));
    addWidgetToPage(ui->page_disp_vent_terminal, new Widget_Disp_vent_terminal_inputTable(false));
    addWidgetToPage(ui->page_other_send_terminal, new Widget_Other_send_terminal_inputTable(false));
    addWidgetToPage(ui->page_static_box, new Widget_static_box_inputTable(false));
    addWidgetToPage(ui->page_duct_with_multi_ranc, new Widget_Multi_ranc_inputTable(false));
    addWidgetToPage(ui->page_diffuser_branch, new Widget_Diffuser_branch_inputTable(false));
    addWidgetToPage(ui->page_tee, new Widget_Tee_inputTable(false));
    addWidgetToPage(ui->page_pipe, new Widget_Pipe_inputTable(false));
    addWidgetToPage(ui->page_elbow, new Widget_Elbow_inputTable(false));
    addWidgetToPage(ui->page_reducer, new Widget_Reducer_inputTable(false));
    addWidgetToPage(ui->page_circular_silencer, new Widget_Silencer_inputTable(false,"圆形消音器"));
    addWidgetToPage(ui->page_rect_silencer, new Widget_Silencer_inputTable(false, "方形消音器"));
    addWidgetToPage(ui->page_circular_silencer_elbow, new Widget_Silencer_inputTable(false, "圆形消音弯头"));
    addWidgetToPage(ui->page_rect_silencer_elbow, new Widget_Silencer_inputTable(false, "方形消音弯头"));
}





/**********主界面**********/
#pragma region "main" {

//初始化列表
void Widget::initializeTreeWidget()
{
    ui->treeWidget->clear();    //先清空
    item_prj = new QTreeWidgetItem(QStringList("工程-" + ProjectManager::getInstance().getPrjID()));  //工程名
    ui->treeWidget->setHeaderItem(item_prj);
    item_prj_info = new QTreeWidgetItem(QStringList("1.项目信息"));   //工程信息

    item_sound_sorce_noise = new QTreeWidgetItem(QStringList("2.噪音源"));   //1音源噪音
    item_fan_noise = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("风机"));   //1.1风机噪音
    item_fan_coil_noise = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("风机盘管"));   //1.2风机盘管噪音
    item_aircondition_noise_single_fan = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("空调器(单风机)"));   //1.3空调器噪音(单风机)
    item_aircondition_noise_double_fan = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("空调器(双风机)"));   //1.3空调器噪音(双风机)

    item_airflow_noise = new QTreeWidgetItem(QStringList("3.气流噪音"));            //2.管路及附件气流噪音
    item_valve_noise = new QTreeWidgetItem(item_airflow_noise,QStringList("阀门"));                                  //2.1 阀门噪音
    item_VAV_terminal = new QTreeWidgetItem(item_valve_noise,QStringList("变风量末端"));                                  //2.1.1 变风量末端
    item_circular_damper = new QTreeWidgetItem(item_valve_noise,QStringList("圆形调风门"));                                  //2.1.2 圆形调风门
    item_rect_damper = new QTreeWidgetItem(item_valve_noise,QStringList("方形调风门"));                                      //2.1.3 方形调风门

    item_atten_and_airflow_noise = new QTreeWidgetItem(QStringList("4.噪音衰减+气流噪音"));                           //2.2 末端气流噪声
    item_air_diff = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("布风器+散流器"));                                         //2.2.1 布风器+散流器
    item_pump = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("抽风头"));                         //2.2.2 抽/送风头
    item_send = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("送风头"));                         //2.2.2 抽/送风头
    item_staticBox_grille = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("静压箱+格栅"));                            //2.2.3 回风箱+格栅
    item_disp_vent_terminal = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("置换通风末端"));                //2.2.4 置换通风末端
    item_other_send_terminal = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("其他送风末端"));                //2.2.5 静压箱孔板送风

    item_noise_atten = new QTreeWidgetItem(QStringList("5.噪音衰减"));                //3. 管路及附件噪音衰减
    item_branch_atten = new QTreeWidgetItem(item_noise_atten,QStringList("分支"));                             //3.1 分支衰减
    item_static_box = new QTreeWidgetItem(item_branch_atten,QStringList("静压箱"));                               //3.1.1 静压箱
    item_duct_with_multi_ranc = new QTreeWidgetItem(item_branch_atten,QStringList("风道多分支"));                     //3.1.2 风道多分支
    item_diffuser_branch = new QTreeWidgetItem(item_branch_atten,QStringList("空调器风口分支"));                     //3.1.3 风道多分支
    item_tee_atten = new QTreeWidgetItem(item_branch_atten,QStringList("三通"));                                //3.1.4 三通衰减
    item_pipe_atten = new QTreeWidgetItem(item_noise_atten,QStringList("直管"));                                //3.2 直管衰减
    item_elbow_atten = new QTreeWidgetItem(item_noise_atten,QStringList("弯头"));                                //3.3 弯头衰减
    item_reducer_atten = new QTreeWidgetItem(item_noise_atten,QStringList("变径"));                                //3.4 变径衰减
    item_circular_silencer_atten = new QTreeWidgetItem(item_noise_atten,QStringList("圆形消音器"));                                //3.5 消音器衰减
    item_rect_silencer_atten = new QTreeWidgetItem(item_noise_atten,QStringList("方形消音器"));                                //3.5 消音器衰减
    item_circular_silencer_elbow_atten = new QTreeWidgetItem(item_noise_atten,QStringList("圆形消音弯头"));                                //3.5 消音器衰减
    item_rect_silencer_elbow_atten = new QTreeWidgetItem(item_noise_atten,QStringList("方形消音弯头"));                                //3.5 消音器衰减


    item_room_atten = new QTreeWidgetItem(QStringList("6.声压级计算类型"));                                   //5. 房间衰减
    item_room_less425 = new QTreeWidgetItem(item_room_atten,QStringList("类型1"));      //5.1 体积小于425m³的房间（点噪声源）
    item_room_more425 = new QTreeWidgetItem(item_room_atten,QStringList("类型2"));       //5.2 体积大于425m³的房间（点噪声源）
    item_room_noFurniture = new QTreeWidgetItem(item_room_atten,QStringList("类型3"));    //5.3 无家具房间（点噪声源）
    item_room_open = new QTreeWidgetItem(item_room_atten,QStringList("类型4"));           //5.4 室外开敞住所（点噪声源）
    item_room_gap_tuyere = new QTreeWidgetItem(item_room_atten,QStringList("类型5"));     //5.5 条缝风口房间（线噪声源）
    item_room_rain = new QTreeWidgetItem(item_room_atten,QStringList("类型6"));           //5.6 雨降风口房间（面噪声源）

    item_system_list = new QTreeWidgetItem(QStringList("7.系统清单"));                                    //6.系统清单
    item_room_define = new QTreeWidgetItem(QStringList("8.计算房间"));                                    //7.计算房间
    item_room_calculate = new QTreeWidgetItem(QStringList("9.噪音计算"));                                 //8.噪音计算
    item_classic_cabin = new QTreeWidgetItem(item_room_calculate,QStringList("典型住舱"));                                     //8.1典型住舱

    item_report = new QTreeWidgetItem(QStringList("10.报表"));        // 9.报表
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
    ui->treeWidget->addTopLevelItem(item_airflow_noise);    //管路及附件气流噪音
    ui->treeWidget->addTopLevelItem(item_atten_and_airflow_noise);    //管路及附件噪音衰减
    ui->treeWidget->addTopLevelItem(item_noise_atten);    //末端反射衰减
    ui->treeWidget->addTopLevelItem(item_room_atten);    //房间衰减
    ui->treeWidget->addTopLevelItem(item_system_list);    //系统清单
    ui->treeWidget->addTopLevelItem(item_room_define);   //计算房间
    ui->treeWidget->addTopLevelItem(item_room_calculate);   //噪音计算
    //ui->treeWidget->addTopLevelItem(item_report);   //报表

    // 设置子项为展开状态
    item_prj->setExpanded(true); // 这一行将子项设置为展开状态
}

//部件数据库
void Widget::on_pushButton_database_clicked()
{
    if (!componentDB) {
        componentDB = new ComponentsDB();
    }
    componentDB->show();
}

//工程管理按钮
void Widget::on_pushButto_prj_manage_clicked()
{
    Dialog_prj_manager *dialog = new Dialog_prj_manager(this);

    if(ui->stackedWidget->currentWidget() != ui->page_prj_info)
        ui->stackedWidget->setCurrentWidget(ui->page_prj_info);

    if (dialog->exec() == QDialog::Accepted)
    {
        ui->lineEdit_prj_ID->setText(ProjectManager::getInstance().getPrjID());

        initializeTreeWidget();

        //RoomCalInfoManager初始化tree和stackedwidget
        RoomCalInfoManager::getInstance().initWidgetElem(item_system_list,
                                                         item_room_define,
                                                         item_room_calculate,
                                                         item_classic_cabin,
                                                         ui->treeWidget,
                                                         ui->stackedWidget);

        RoomCalInfoManager::getInstance().loadDataFromDB();
    }
}

void Widget::on_pushButton_noi_limit_add_clicked()
{
    if(!ProjectManager::getInstance().isProjectOpened())
    {
        QMessageBox::critical(this, "错误", "当前未打开任何项目");
        return;
    }
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
    QList<NoiseLimit> noiseLimits = ProjectManager::getInstance().getNoiseLimits();
    // 获取选中的行索引
    QList<int> selectedRows;
    QList<QString> selectedRoomTypes;
    for (int row = 0; row < ui->tableWidget_noi_limit->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_noi_limit->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox && checkBox->isChecked()) {
            selectedRows.append(row);
            QTableWidgetItem* item = ui->tableWidget_noi_limit->item(row, 3);
            if (item != nullptr)
            { // 确保item不为nullptr
                selectedRoomTypes.append(item->text());
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

        for (const QString& roomType : selectedRoomTypes)
        {
            auto it = std::remove_if(noiseLimits.begin(), noiseLimits.end(),
                                     [&roomType](const NoiseLimit& noiseLimit) {
                                         return noiseLimit.roomType == roomType;
                                     });
            noiseLimits.erase(it, noiseLimits.end()); // 实际删除 those elements
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
    if(!ProjectManager::getInstance().isProjectOpened())
    {
        QMessageBox::critical(this, "错误", "当前未打开任何项目");
        return;
    }
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
    QList<QLineEdit*> lineEdits = {ui->lineEdit_prj_ID, ui->lineEdit_prj_name, ui->lineEdit_ship_num, ui->lineEdit_shipyard,
                                  ui->lineEdit_prj_manager};

    for(auto& lineEdit: lineEdits)
    {
        lineEdit->setReadOnly(false);
        lineEdit->setStyleSheet("QLineEdit{background-color: white; border: 1px solid #9C9C9C;}");
    }
}

void Widget::on_pushButton_prj_info_save_clicked()
{
    if(!ProjectManager::getInstance().isProjectOpened())
    {
        QMessageBox::critical(this, "错误", "当前未打开任何项目");
        return;
    }
    QList<QLineEdit*> lineEdits = {ui->lineEdit_prj_ID, ui->lineEdit_prj_name, ui->lineEdit_ship_num, ui->lineEdit_shipyard,
                                  ui->lineEdit_prj_manager};

    for(auto& lineEdit: lineEdits)
    {
        if(lineEdit->text().isEmpty())
        {
            QMessageBox::critical(this,"错误","存在未输入的信息");
            return;
        }
    }

    if(ui->lineEdit_prj_ID->text() != ProjectManager::getInstance().getPrjID())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("修改项目编号警告");
        msgBox.setText("是否修改项目编号\n" + ProjectManager::getInstance().getPrjID() + " -> " + ui->lineEdit_prj_ID->text());
        msgBox.setIcon(QMessageBox::Warning);
        QPushButton *yesButton = msgBox.addButton("确认", QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton("取消", QMessageBox::NoRole);
        msgBox.exec();

        if (msgBox.clickedButton() == yesButton)
        {
            if(DatabaseManager::getInstance().isProjectExist(ui->lineEdit_prj_ID->text()))
            {
                QMessageBox::critical(this,"项目编号修改错误","项目 \"" + ui->lineEdit_prj_ID->text() + "\" 已经存在");
                return;
            }
            DatabaseManager::getInstance().updateProjectIDInDatabase(ProjectManager::getInstance().getPrjID(), ui->lineEdit_prj_ID->text());
        }
        else if(msgBox.clickedButton() == noButton)
        {
            ui->lineEdit_prj_ID->setText(ProjectManager::getInstance().getPrjID());
            return;
        }
    }

    for(auto& lineEdit: lineEdits)
    {
        lineEdit->setReadOnly(true);
        lineEdit->setStyleSheet("QLineEdit{background-color: rgb(240, 240, 240); border: 1px solid #9C9C9C;}");
    }

    ProjectInfo prjInfo(ui->lineEdit_prj_ID->text(),
                        ui->lineEdit_prj_name->text(),
                        ui->lineEdit_ship_num->text(),
                        ui->lineEdit_shipyard->text(),
                        ui->lineEdit_prj_manager->text(),
                        ProjectManager::getInstance().getClassSoc());
    ProjectManager::getInstance().setPrjInfo(prjInfo);
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
    if(!ProjectManager::getInstance().isProjectOpened())
    {
        QMessageBox::critical(this, "错误", "当前未打开任何项目");
        return;
    }
    // 打开文件选择对话框，让用户选择文件
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*)"));
    QString fileName = "";
    if (filePath.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(filePath);
    // 保存文件名（包括后缀）
    fileName = fileInfo.fileName();

    QTableWidget *tableWidget = ui->tableWidget_project_attachment;

    QStringList data = {
        QString::number(tableWidget->rowCount() + 1),
        fileName
    };

    int rowCount = tableWidget->rowCount();
    tableWidget->setRowCount(rowCount + 1);

    ProjectAttachment attach{QString::number(rowCount + 1), fileName, filePath};
    if(!ProjectManager::getInstance().insertAttachmentToList(attach))
    {
        qDebug() << "插入失败";
        return;
    }

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
            // 存储fileName作为自定义属性
            label->setProperty("fileName", fileName);
            tableWidget->setCellWidget(rowCount, 2, label);

            connect(label, &QLabel::linkActivated, [localFilePath=filePath](const QString &link){
                // 处理点击事件，例如打开链接
                QUrl fileUrl = QUrl::fromLocalFile(localFilePath);
                QDesktopServices::openUrl(fileUrl);
            });
        }
    }

}

void Widget::on_pushButton_project_attachment_del_clicked()
{
    // 获取选中的行索引
    QList<int> selectedRows;
    QList<QString> selectedNames;
    for (int row = 0; row < ui->tableWidget_project_attachment->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_project_attachment->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox && checkBox->isChecked()) {
            selectedRows.append(row);
            QLabel *label = qobject_cast<QLabel*>(ui->tableWidget_project_attachment->cellWidget(row, 2));
            if (label) {
                QString retrievedFileName = label->property("fileName").toString();
                // 现在你有了fileName，可以根据需要使用它
                selectedNames.append(retrievedFileName);
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
            ui->tableWidget_project_attachment->removeRow(row);
        }

        for (const QString& name : selectedNames)
        {
            ProjectManager::getInstance().removeAttachmentInList(name);
        }


        // 重新编号
        for (int row = 0; row < ui->tableWidget_project_attachment->rowCount(); ++row) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(row + 1));
            ui->tableWidget_project_attachment->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(Qt::ItemIsEditable);
        }
    }
}

void Widget::setAttachmentTable()
{
    QList<ProjectAttachment> attachments = ProjectManager::getInstance().getAttachments();
    QTableWidget *tableWidget = ui->tableWidget_project_attachment;

    for (const auto &attach : attachments) {
        int rowCount = tableWidget->rowCount();
        tableWidget->setRowCount(rowCount + 1);

        for (int col = 0; col < 3; ++col) {
            if (col == 0) {
                // 添加复选框
                QCheckBox* checkBox = new QCheckBox();
                QWidget* widget = new QWidget();
                QHBoxLayout* layout = new QHBoxLayout(widget);
                layout->addWidget(checkBox);
                layout->setAlignment(Qt::AlignCenter);
                layout->setContentsMargins(0, 0, 0, 0);
                tableWidget->setCellWidget(rowCount, col, widget);
            } else if (col == 1) {
                QTableWidgetItem *item = new QTableWidgetItem(attach.tableID);
                tableWidget->setItem(rowCount, col, item);
                item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 设置为只读
            } else if (col == 2) {
                QLabel *label = new QLabel("<a href='#'>" + attach.attachName + "</a>");
                label->setTextFormat(Qt::RichText);
                label->setCursor(Qt::PointingHandCursor); // 设置鼠标悬停时为手形光标
                label->setAlignment(Qt::AlignCenter);
                // 存储attachPath作为自定义属性
                label->setProperty("fileName", attach.attachName);
                tableWidget->setCellWidget(rowCount, col, label);

                connect(label, &QLabel::linkActivated, [localFilePath=attach.attachPath](const QString &){
                    // 处理点击事件，例如打开链接
                    QUrl fileUrl = QUrl::fromLocalFile(localFilePath);
                    QDesktopServices::openUrl(fileUrl);
                });
            }
        }
    }
}

void Widget::setDrawingTable()
{
    QList<Drawing> drawings = ProjectManager::getInstance().getDrawings();
    // 遍历列表，填充表格
    int rowCount = 0;
    for (const Drawing &drawing : drawings) {
        // 插入新行
        ui->tableWidget_drawing_list->insertRow(rowCount);

        // 添加复选框
        QCheckBox* checkBox = new QCheckBox();
        QWidget* widget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        ui->tableWidget_drawing_list->setCellWidget(rowCount, 0, widget);

        // 设置序号，图号，图名
        QTableWidgetItem *itemSerial = new QTableWidgetItem(drawing.tableID);
        ui->tableWidget_drawing_list->setItem(rowCount, 1, itemSerial);
        itemSerial->setTextAlignment(Qt::AlignCenter);
        itemSerial->setFlags(itemSerial->flags() & ~Qt::ItemIsEditable); // 设置为只读
        itemSerial->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        itemSerial->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色

        QTableWidgetItem *itemDrawingNum = new QTableWidgetItem(drawing.drawingNum);
        ui->tableWidget_drawing_list->setItem(rowCount, 2, itemDrawingNum);
        itemDrawingNum->setTextAlignment(Qt::AlignCenter);
        itemDrawingNum->setFlags(itemDrawingNum->flags() & ~Qt::ItemIsEditable); // 设置为只读
        itemDrawingNum->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        itemDrawingNum->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色

        QTableWidgetItem *itemDrawingName = new QTableWidgetItem(drawing.drawingName);
        ui->tableWidget_drawing_list->setItem(rowCount, 3, itemDrawingName);
        itemDrawingName->setTextAlignment(Qt::AlignCenter);
        itemDrawingName->setFlags(itemDrawingName->flags() & ~Qt::ItemIsEditable); // 设置为只读
        itemDrawingName->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        itemDrawingName->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色

        rowCount++;
    }
}

void Widget::setNoiseLimit()
{
    QList<NoiseLimit> noiseLimits = ProjectManager::getInstance().getNoiseLimits();

    // 遍历噪声限值列表，为每个项目添加表格行
    int rowCount = 0;
    foreach (const NoiseLimit &noiseLimit, noiseLimits) {
        ui->tableWidget_noi_limit->insertRow(rowCount);

        // 处理复选框
        QCheckBox* checkBox = new QCheckBox();
        QWidget* widget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        ui->tableWidget_noi_limit->setCellWidget(rowCount, 0, widget);

        // 设置序号
        QTableWidgetItem *itemTableID = new QTableWidgetItem(noiseLimit.tableID);
        itemTableID->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_noi_limit->setItem(rowCount, 1, itemTableID);
        itemTableID->setFlags(itemTableID->flags() & ~Qt::ItemIsEditable); // 设置为只读
        itemTableID->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        itemTableID->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色

        // 设置处所类型
        QTableWidgetItem *itemPlaceType = new QTableWidgetItem(noiseLimit.placeType);
        itemPlaceType->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_noi_limit->setItem(rowCount, 2, itemPlaceType);
        itemPlaceType->setFlags(itemPlaceType->flags() & ~Qt::ItemIsEditable); // 设置为只读
        itemPlaceType->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        itemPlaceType->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色

        // 设置房间类型
        QTableWidgetItem *itemRoomType = new QTableWidgetItem(noiseLimit.roomType);
        itemRoomType->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_noi_limit->setItem(rowCount, 3, itemRoomType);
        itemRoomType->setFlags(itemRoomType->flags() & ~Qt::ItemIsEditable); // 设置为只读
        itemRoomType->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        itemRoomType->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色

        // 设置噪声限值dB(A)
        QTableWidgetItem *itemNoiseLimit = new QTableWidgetItem(noiseLimit.noiseLimit);
        itemNoiseLimit->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_noi_limit->setItem(rowCount, 4, itemNoiseLimit);
        itemNoiseLimit->setFlags(itemNoiseLimit->flags() & ~Qt::ItemIsEditable); // 设置为只读
        itemNoiseLimit->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        itemNoiseLimit->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色

        rowCount++;
    }
}

void Widget::setPrjBasicInfo()
{
    QList<QLineEdit*> lineEdits = {ui->lineEdit_prj_ID, ui->lineEdit_prj_name, ui->lineEdit_ship_num, ui->lineEdit_shipyard,
                                  ui->lineEdit_prj_manager, ui->lineEdit_class_soc};

    for(auto& lineEdit: lineEdits)
    {
        lineEdit->setReadOnly(true);
        lineEdit->setStyleSheet("QLineEdit{background-color: rgb(192, 192, 192); border: 1px solid #9C9C9C; color: rgb(70, 70, 70);}");
    }

    ProjectInfo prjInfo = ProjectManager::getInstance().getPrjInfo();

    ui->lineEdit_prj_ID->setText(prjInfo.prjID);
    ui->lineEdit_prj_name->setText(prjInfo.prjName);
    ui->lineEdit_ship_num->setText(prjInfo.shipNum);
    ui->lineEdit_shipyard->setText(prjInfo.shipyard);
    ui->lineEdit_prj_manager->setText(prjInfo.prjManager);
    ui->lineEdit_class_soc->setText(prjInfo.classSoc);
}

void Widget::setBasicPageWhenSwitchPrj()
{
    setAttachmentTable();
    setDrawingTable();
    setNoiseLimit();
    setPrjBasicInfo();
}

/**
 * @brief 保存参考图纸清单
 */
void Widget::on_pushButton_drawing_list_save_clicked()
{
    if(!ProjectManager::getInstance().isProjectOpened())
    {
        QMessageBox::critical(this, "错误", "当前未打开任何项目");
        return;
    }
    QList<Drawing> drawings; // 创建一个Drawing的列表用来存储所有行的数据

    // 遍历每一行
    for (int row = 0; row < ui->tableWidget_drawing_list->rowCount(); ++row) {
        QTableWidgetItem* tableIDItem = ui->tableWidget_drawing_list->item(row, 1);
        QTableWidgetItem* drawingNumItem = ui->tableWidget_drawing_list->item(row, 2);
        QTableWidgetItem* drawingNameItem = ui->tableWidget_drawing_list->item(row, 3);

        // 检查是否有空字段
        if (!drawingNumItem || drawingNumItem->text().isEmpty() ||
            !drawingNameItem || drawingNameItem->text().isEmpty()) {
            QMessageBox::critical(this, tr("空字段错误"),
                                 tr("第%1行中发现空字段，请检查并填写完整。").arg(row + 1));
            return; // 直接退出函数
        }

        // 无空字段，添加Drawing到列表
        Drawing drawing;
        drawing.tableID = tableIDItem->text();
        drawing.drawingNum = drawingNumItem->text();
        drawing.drawingName = drawingNameItem->text();
        drawings.append(drawing);
    }

    // 遍历表格中的每一行和列，除了第一列
    for (int row = 0; row < ui->tableWidget_drawing_list->rowCount(); ++row) {
        for (int col = 1; col < ui->tableWidget_drawing_list->columnCount(); ++col) {
            QTableWidgetItem* item = ui->tableWidget_drawing_list->item(row, col);
            if (!item) {
                item = new QTableWidgetItem();
                ui->tableWidget_drawing_list->setItem(row, col, item);
            }
            // 设置背景色为灰色
            item->setBackground(QBrush(QColor(192, 192, 192)));
            item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色

            // Center text in the table items
            item->setTextAlignment(Qt::AlignCenter);

            // 如果你也想设置这些单元格为只读，可以取消以下注释
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
    }

    ProjectManager::getInstance().setDrawings(drawings);
}

void Widget::on_pushButton_drawing_list_revise_clicked()
{
    // 遍历表格中的每一行和列，除了第一列
    for (int row = 0; row < ui->tableWidget_drawing_list->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_drawing_list->cellWidget(row, 0);
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            for (int col = 1; col < ui->tableWidget_drawing_list->columnCount(); ++col) {
                QTableWidgetItem* item = ui->tableWidget_drawing_list->item(row, col);
                if (!item) {
                    item = new QTableWidgetItem();
                    ui->tableWidget_drawing_list->setItem(row, col, item);
                }
                item->setBackground(QBrush(Qt::white));
                item->setData(Qt::ForegroundRole, QBrush(Qt::black));

                // 如果之前设置了只读，这里恢复为可编辑状态
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            }
        }
    }
}

void Widget::initTableWidget_noi_limit()
{
    int colCount = 5;
    // 设置表头标题
    QStringList headerText;
    headerText << "" << "序号" << "处所类型"  << "房间类型" << "噪声限值dB(A)";
    // 设置每列的宽度
    int columnWidths[] = {30, 38, 130, 130, 90};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_noi_limit, headerText, columnWidths, colCount);
    colCount = 4;
    QStringList headerText1;
    headerText1 << "序号" << "处所类型"  << "房间类型" << "噪声限值dB(A)";
    int columnWidths1[] = {38, 130, 130, 90};
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
    int columnWidths[] = {1, 2, 8, 11};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_drawing_list, headerText, columnWidths, colCount);
}

void Widget::on_pushButton_noi_limit_save_clicked()
{
    if(!ProjectManager::getInstance().isProjectOpened())
    {
        QMessageBox::critical(this, "错误", "当前未打开任何项目");
        return;
    }
    if(ui->lineEdit_class_soc->text().isEmpty())
    {
        QMessageBox::critical(this, "空字段错误", "船级社不能为空");
        return;
    }

    ui->lineEdit_class_soc->setReadOnly(true);
    ui->lineEdit_class_soc->setStyleSheet("QLineEdit{background-color: rgb(192, 192, 192); border: 1px solid #9C9C9C; color: rgb(70, 70, 70);}");
    QList<NoiseLimit> noiseLimits; // 创建一个Drawing的列表用来存储所有行的数据

    // 遍历每一行
    for (int row = 0; row < ui->tableWidget_noi_limit->rowCount(); ++row) {
        QTableWidgetItem* tableIDItem = ui->tableWidget_noi_limit->item(row, 1);
        QTableWidgetItem* placeTypeItem = ui->tableWidget_noi_limit->item(row, 2);
        QTableWidgetItem* roomTypeItem = ui->tableWidget_noi_limit->item(row, 3);
        QTableWidgetItem* noiseLimitItem = ui->tableWidget_noi_limit->item(row, 4);

        // 检查是否有空字段
        if (!roomTypeItem || roomTypeItem->text().isEmpty() ||
            !noiseLimitItem || noiseLimitItem->text().isEmpty() ||
            !placeTypeItem || placeTypeItem->text().isEmpty()) {
            QMessageBox::critical(this, tr("空字段错误"),
                                 tr("第%1行中发现空字段，请检查并填写完整。").arg(row + 1));
            return; // 直接退出函数
        }

        // 无空字段，创建NoiseLimit对象并添加到列表
        NoiseLimit noiseLimit;
        noiseLimit.tableID = tableIDItem->text();
        noiseLimit.placeType = placeTypeItem->text();
        noiseLimit.roomType = roomTypeItem->text();
        noiseLimit.noiseLimit = noiseLimitItem->text();
        noiseLimits.append(noiseLimit);
    }

    // 遍历表格中的每一行和列，除了第一列
    for (int row = 0; row < ui->tableWidget_noi_limit->rowCount(); ++row) {
        for (int col = 1; col < ui->tableWidget_noi_limit->columnCount(); ++col) {
            QTableWidgetItem* item = ui->tableWidget_noi_limit->item(row, col);
            if (!item) {
                item = new QTableWidgetItem();
                ui->tableWidget_noi_limit->setItem(row, col, item);
            }
            // 设置背景色为灰色
            item->setBackground(QBrush(QColor(192, 192, 192)));
            item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色

            // Center text in the table items
            item->setTextAlignment(Qt::AlignCenter);

            // 如果你也想设置这些单元格为只读，可以取消以下注释
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
    }

    ProjectManager::getInstance().setNoiseLimits(noiseLimits);
    ProjectManager::getInstance().setClassSoc(ui->lineEdit_class_soc->text());
}

void Widget::on_pushButton_noi_limit_revise_clicked()
{
    ui->lineEdit_class_soc->setReadOnly(false);
    ui->lineEdit_class_soc->setStyleSheet("QLineEdit{background-color: white; border: 1px solid #9C9C9C; color: black;}");
    // 遍历表格中的每一行和列，除了第一列
    for (int row = 0; row < ui->tableWidget_noi_limit->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_noi_limit->cellWidget(row, 0); // Assuming the checkbox is in the first column
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            for (int col = 1; col < ui->tableWidget_noi_limit->columnCount(); ++col) {
                QTableWidgetItem* item = ui->tableWidget_noi_limit->item(row, col);
                if (!item) {
                    item = new QTableWidgetItem();
                    ui->tableWidget_noi_limit->setItem(row, col, item);
                }
                // 恢复单元格的背景色（清除之前设置的灰色背景）
                item->setBackground(QBrush(Qt::white)); // 或者使用 item->setBackground(QBrush()); 来清除背景色
                item->setData(Qt::ForegroundRole, QBrush(Qt::black));
                // 如果之前设置了只读，这里恢复为可编辑状态
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            }
        }
    }


}

//当表格item改变，rooms跟着改变
void Widget::on_tableWidget_noi_limit_itemChanged(QTableWidgetItem *item)
{
    if (!item || item->text().isEmpty())
        return;

    int rowCount = ui->tableWidget_noi_limit->rowCount();
    QList<NoiseLimit> noiseLimits; // 创建一个Drawing的列表用来存储所有行的数据

    for (int i = 0; i < rowCount; i++) {
        if (!ui->tableWidget_noi_limit->item(i, 2) || ui->tableWidget_noi_limit->item(i, 2)->text().isEmpty() ||
            !ui->tableWidget_noi_limit->item(i, 3) || ui->tableWidget_noi_limit->item(i, 3)->text().isEmpty() ||
            !ui->tableWidget_noi_limit->item(i, 4) || ui->tableWidget_noi_limit->item(i, 4)->text().isEmpty()) {
            // 如果任一必需的单元格为空，则跳过这一行
            continue;
        }

        NoiseLimit noiseLimit;
        noiseLimit.tableID = ui->tableWidget_noi_limit->item(i, 1)->text();
        noiseLimit.placeType = ui->tableWidget_noi_limit->item(i, 2)->text();
        noiseLimit.roomType = ui->tableWidget_noi_limit->item(i, 3)->text();
        noiseLimit.noiseLimit = ui->tableWidget_noi_limit->item(i, 4)->text();
        noiseLimits.append(noiseLimit);
    }

    ProjectManager::getInstance().setNoiseLimits(noiseLimits);
}

//输入模块按钮
void Widget::on_pushButton_input_clicked()
{
    if(!ProjectManager::getInstance().isProjectOpened())
    {
        QMessageBox::critical(this, "错误", "当前未打开任何项目");
        return;
    }
    if(ui->stackedWidget->currentWidget() == ui->page_fan)
        return;
    ui->stackedWidget->setCurrentWidget(ui->page_fan);
}

//登录按钮
void Widget::on_pushButton_start_clicked()
{
    this->on_pushButto_prj_manage_clicked();
}

/**
 * @brief 用于清空界面
 */
void Widget::clearTable()
{
    // 定义一个Lambda表达式用于递归清空QTableWidgets和QLineEdits
    std::function<void(QWidget*)> clearAllWidgets = [&](QWidget* parent) {
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
                // 尝试将当前控件转换为QLineEdit
                QLineEdit *lineEdit = qobject_cast<QLineEdit*>(currentWidget);
                if (lineEdit) {
                    // 如果转换成功，清空QLineEdit的内容
                    lineEdit->clear();
                } else {
                    // 如果当前控件既不是QTableWidget也不是QLineEdit，将它的子控件加入队列
                    const auto children = currentWidget->children();
                    for (QObject *child : children) {
                        QWidget *childWidget = qobject_cast<QWidget*>(child);
                        if (childWidget) {
                            queue.enqueue(childWidget);
                        }
                    }
                }
            }
        }
    };

    // 使用定义的Lambda表达式从该窗口控件开始递归清空所有QTableWidget
    clearAllWidgets(ui->page_prj_info);
}

#pragma endregion }
/**********主界面**********/




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

}

bool Widget::eventFilter(QObject* obj, QEvent* event)
{

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
    if(!ProjectManager::getInstance().isProjectOpened())
    {
        QMessageBox::critical(this, "错误", "当前未打开任何项目");
        return;
    }

    if(current == item_prj_info)     //工程信息
    {
        ui->stackedWidget->setCurrentWidget(ui->page_prj_info);
    }
    else if(current == item_fan_noise)      //风机噪音
    {
        ui->stackedWidget->setCurrentWidget(ui->page_fan);
    }
    else if(current == item_fan_coil_noise) //风机盘管噪音
    {
        ui->stackedWidget->setCurrentWidget(ui->page_fanCoil);
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
    else if(current == item_pump)   //抽/送风头
    {
        ui->stackedWidget->setCurrentWidget(ui->page_pump);
    }
    else if(current == item_send)   //抽/送风头
    {
        ui->stackedWidget->setCurrentWidget(ui->page_send);
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
    else if(current == item_diffuser_branch)         //风道多分支
    {
        ui->stackedWidget->setCurrentWidget(ui->page_diffuser_branch);
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
    else if(current == item_circular_silencer_atten)     //消音器
    {
        ui->stackedWidget->setCurrentWidget(ui->page_circular_silencer);
    }
    else if(current == item_rect_silencer_atten)     //消音器
    {
        ui->stackedWidget->setCurrentWidget(ui->page_rect_silencer);
    }
    else if(current == item_circular_silencer_elbow_atten)     //消音器
    {
        ui->stackedWidget->setCurrentWidget(ui->page_circular_silencer_elbow);
    }
    else if(current == item_rect_silencer_elbow_atten)     //消音器
    {
        ui->stackedWidget->setCurrentWidget(ui->page_rect_silencer_elbow);
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

/******************导入导出表格********************/
#pragma region "excel" {

///*
//日期: 2024-3-23
//作者: JJH
//环境: win10 QT5.14.2 MinGW32
//参数：要提取的表格，输出器件名+时间戳
//功能: 提取tablewidget的数据（除去第一二列）导出数据到execl表格
//*/
void Widget::SaveExeclData(QTableWidget *table,QString component_name) {}
//{
//    // 获取当前时间
//       QDateTime currentDateTime = QDateTime::currentDateTime();
//       QString defaultFileName = component_name+"_" + currentDateTime.toString("yyyyMMdd_hhmmss") + ".xlsx";
//    //获取保存路径
//       QString filepath=QFileDialog::getSaveFileName(this,tr("Save"),defaultFileName,tr(" (*.xlsx)"));
//       if(!filepath.isEmpty()){
//           QAxObject *excel = new QAxObject(this);
//           //连接Excel控件
//           excel->setControl("Excel.Application");
//           if (excel->isNull()) {
//               QMessageBox::critical(this, tr("错误"), tr("未能创建 Excel 对象，请安装 Microsoft Excel。"));
//               delete excel;
//               return;
//           }
//           //不显示窗体
//           excel->dynamicCall("SetVisible (bool Visible)","false");
//           //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
//           excel->setProperty("DisplayAlerts", false);
//           //获取工作簿集合
//           QAxObject *workbooks = excel->querySubObject("WorkBooks");
//           //新建一个工作簿
//           workbooks->dynamicCall("Add");
//           //获取当前工作簿
//           QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
//           //获取工作表集合
//           QAxObject *worksheets = workbook->querySubObject("Sheets");
//           //获取工作表集合的工作表1，即sheet1
//           QAxObject *worksheet = worksheets->querySubObject("Item(int)",1);

//           /*注意事项：
//            1.提取值 不要第一二列
//            2.设置某行某列,在 Qt 中，数组的索引是从 0 开始的，
//              而在 Excel 中，列的索引是从 1 开始的，所以需要进行调整。
//            */

//           //设置表头值
//           for(int i=1;i<table->columnCount()-1;i++)
//           {
//               if(table->horizontalHeaderItem(i+1) != nullptr) {//空值空指针会使程序崩溃
//                   QAxObject *Range = worksheet->querySubObject("Cells(int,int)", 1, i);
//                   //调用Excel的函数设置设置表头
//                   Range->dynamicCall("SetValue(const QString &)",table->horizontalHeaderItem(i+1)->text());
//               }
//           }
//           //设置表格数据
//           for(int i=1;i<table->rowCount()+1;i++)
//           {
//               for(int j = 1;j<table->columnCount()-1;j++)
//               {
//                   if(table->item(i-1,j+1) != nullptr) {
//                       QAxObject *Range = worksheet->querySubObject("Cells(int,int)", i+1, j);
//                       Range->dynamicCall("SetValue(const QString &)",table->item(i-1,j+1)->data(Qt::DisplayRole).toString());
//                   }
//               }
//           }
//           workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(filepath));//保存至filepath
//           workbook->dynamicCall("Close()");//关闭工作簿
//           excel->dynamicCall("Quit()");//关闭excel
//           delete excel;
//           excel=NULL;

//           QMessageBox::information(this,"提示","导出成功",QMessageBox::Yes);
//       }
//}


///*
//日期: 2024-3-23
//作者: JJH
//环境: win10 QT5.14.2 MinGW32
//参数：要导入到的表格
//功能: 从Excel中提取数据到tableWidget（第一列自动添加复选框，第二列加递增序号）
//*/
void Widget::LoadExeclData(QTableWidget *table) {}
//{
//    // 提示用户清空表格
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, tr("注意"), tr("确定清除当前表格再导入新表格"), QMessageBox::Yes|QMessageBox::No);
//    if (reply == QMessageBox::Yes) {
//        // 清空表格
//        table->clearContents();
//    } else {
//        // 用户选择不清空表格，直接返回
//        return;
//    }

//    // 选择要导入的 Excel 文件
//    QString filePath = QFileDialog::getOpenFileName(this, tr("打开Excel文件"), ".", tr("Excel Files (*.xls *.xlsx)"));
//    if (filePath.isEmpty()) {
//        QMessageBox::warning(this,"警告","路径错误！");
//        return;
//    }

//    QAxObject *excel = new QAxObject(this);
//    //连接Excel控件
//    excel->setControl("Excel.Application");
//    if (excel->isNull()) {
//        QMessageBox::critical(this, tr("错误"), tr("未能创建 Excel 对象，请安装 Microsoft Excel。"));
//        delete excel;
//        return;
//    }
//    excel->dynamicCall("SetVisible(bool)", false); // 不显示 Excel 界面

//    QAxObject *workbooks = excel->querySubObject("Workbooks");
//    QAxObject *workbook = workbooks->querySubObject("Open(const QString&)", filePath);
//    QAxObject *worksheets = workbook->querySubObject("Worksheets");

//    // 假设只导入第一个工作表
//    QAxObject *worksheet = worksheets->querySubObject("Item(int)", 1);
//    QAxObject *usedRange = worksheet->querySubObject("UsedRange");
//    QAxObject *rows = usedRange->querySubObject("Rows");
//    QAxObject *columns = usedRange->querySubObject("Columns");

//    //计算Excel表格行列数
//    int rowCount = rows->property("Count").toInt();
//    int columnCount = columns->property("Count").toInt();

//    // 设置表格的行列数
//    table->setRowCount(rowCount-1);// 不算表头的行数
//    table->setColumnCount(columnCount+2); //加上一二列

//    // 读取数据并填充表格
//    for (int row = 0; row < rowCount; ++row) {

//        // 添加复选框
//        QCheckBox* checkBox = new QCheckBox();
//        QWidget* widget = new QWidget();
//        widget->setStyleSheet("background-color: #C0C0C0;");
//        QHBoxLayout* layout = new QHBoxLayout(widget);
//        layout->addWidget(checkBox);
//        layout->setAlignment(Qt::AlignCenter);
//        layout->setContentsMargins(0, 0, 0, 0);
//        table->setCellWidget(row, 0, widget);
//        //添加序号
//        QTableWidgetItem *item = new QTableWidgetItem(QString::number(row+1));
//        table->setItem(row, 1, item);
//        item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
//        item->setFlags(Qt::ItemIsEditable); // 设置为只读
//        item->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
//        item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色


//        for (int col = 0; col < columnCount; ++col) {
//            //提取数据时，从Excel的第二行、第一列开始
//            QAxObject *cell = worksheet->querySubObject("Cells(int,int)", row+2, col+1);
//            QString value = cell->dynamicCall("Value()").toString();

//            QTableWidgetItem *item = new QTableWidgetItem(value);
//            table->setItem(row, col+2, item);
//            item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
//            item->setFlags(Qt::ItemIsEditable); // 设置为只读
//            item->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
//            item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色
//            delete cell;
//        }
//    }

//    workbook->dynamicCall("Close()");//关闭工作簿
//    excel->dynamicCall("Quit()");//关闭excel
//    delete excel;
//    excel=NULL;

//}


///*
//日期: 2024-3-23
//作者: JJH
//环境: win10 QT5.14.2 MinGW32
//参数：要提取的表格，输出器件名+时间戳
//功能: 提取噪声源tablewidget的数据（除去第一二列）导出数据到execl表格，并合并某些单元格
//*/
void Widget::SaveExeclData_noisourse(QTableWidget *table,QString component_name,int mergeCellfirst,int mergeCelllast) {}
//{
//    // 获取当前时间
//       QDateTime currentDateTime = QDateTime::currentDateTime();
//       QString defaultFileName = component_name+"_" + currentDateTime.toString("yyyyMMdd_hhmmss") + ".xlsx";
//    //获取保存路径
//       QString filepath=QFileDialog::getSaveFileName(this,tr("Save"),defaultFileName,tr(" (*.xlsx)"));
//       if(!filepath.isEmpty()){
//           QAxObject *excel = new QAxObject(this);
//           //连接Excel控件
//           excel->setControl("Excel.Application");
//           if (excel->isNull()) {
//               QMessageBox::critical(this, tr("错误"), tr("未能创建 Excel 对象，请安装 Microsoft Excel。"));
//               delete excel;
//               return;
//           }
//           //不显示窗体
//           excel->dynamicCall("SetVisible (bool Visible)","false");
//           //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
//           excel->setProperty("DisplayAlerts", false);
//           //获取工作簿集合
//           QAxObject *workbooks = excel->querySubObject("WorkBooks");
//           //新建一个工作簿
//           workbooks->dynamicCall("Add");
//           //获取当前工作簿
//           QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
//           //获取工作表集合
//           QAxObject *worksheets = workbook->querySubObject("Sheets");
//           //获取工作表集合的工作表1，即sheet1
//           QAxObject *worksheet = worksheets->querySubObject("Item(int)",1);

//           /*注意事项：
//            1.提取值 不要第一二列
//            2.设置某行某列,在 Qt 中，数组的索引是从 0 开始的，
//              而在 Excel 中，列的索引是从 1 开始的，所以需要进行调整。
//            */

//           //设置表头值
//           for(int i=1;i<table->columnCount()-1;i++)
//           {
//               if(table->horizontalHeaderItem(i+1) != nullptr) {//空值空指针会使程序崩溃
//                   QAxObject *Range = worksheet->querySubObject("Cells(int,int)", 1, i);
//                   //调用Excel的函数设置设置表头
//                   Range->dynamicCall("SetValue(const QString &)",table->horizontalHeaderItem(i+1)->text());
//               }
//           }
//           //设置表格数据
//           for(int i=1;i<table->rowCount()+1;i++)
//           {
//               for(int j = 1;j<table->columnCount()-1;j++)
//               {
//                   if(table->item(i-1,j+1) != nullptr) {
//                       QAxObject *Range = worksheet->querySubObject("Cells(int,int)", i+1, j);
//                       Range->dynamicCall("SetValue(const QString &)",table->item(i-1,j+1)->data(Qt::DisplayRole).toString());
//                   }
//               }
//           }

//           //合并单元格
//           for(int i=2;i<table->rowCount()+2;i+=2)
//           {
//               //Excel 0到mergeCellfirst合并
//               for(int j=0;j<mergeCellfirst;j++)
//               {
//                   QString letter = QChar('A' + j);
//                   QString mergeCell = letter + QString::number(i) + ":" + letter + QString::number(i + 1);
//                   QAxObject *rangeObject = worksheet->querySubObject("Range(const QString&)", mergeCell);
//                   rangeObject->setProperty("MergeCells", true);
//                   rangeObject->setProperty("HorizontalAlignment", -4108); // xlCenter
//               }
//               //mergeCelllast合并
//               QString letter = QChar('A' + mergeCelllast-1);
//               QString mergeCell = letter + QString::number(i) + ":" + letter + QString::number(i + 1);
//               QAxObject *rangeObject = worksheet->querySubObject("Range(const QString&)", mergeCell);
//               rangeObject->setProperty("MergeCells", true);
//               rangeObject->setProperty("HorizontalAlignment", -4108); // xlCenter
//           }

//           workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(filepath));//保存至filepath
//           workbook->dynamicCall("Close()");//关闭工作簿
//           excel->dynamicCall("Quit()");//关闭excel
//           delete excel;
//           excel=NULL;

//           QMessageBox::information(this,"提示","导出成功",QMessageBox::Yes);
//       }
//}

///*
//日期: 2024-3-23
//作者: JJH
//环境: win10 QT5.14.2 MinGW32
//参数：要导入到的表格
//功能: 从Excel中提取数据到tableWidget（第一列自动添加复选框，第二列加递增序号）并合并某些单元格
//*/
void Widget::LoadExeclData_noisourse(QTableWidget *table,int spancolfirst,int spancollast) {}
//{
//    // 提示用户清空表格
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, tr("注意"), tr("确定清除当前表格再导入新表格"), QMessageBox::Yes|QMessageBox::No);
//    if (reply == QMessageBox::Yes) {
//        // 清空表格
//        table->clearContents();
//    } else {
//        // 用户选择不清空表格，直接返回
//        return;
//    }

//    // 选择要导入的 Excel 文件
//    QString filePath = QFileDialog::getOpenFileName(this, tr("打开Excel文件"), ".", tr("Excel Files (*.xls *.xlsx)"));
//    if (filePath.isEmpty()) {
//        QMessageBox::warning(this,"警告","路径错误！");
//        return;
//    }

//    QAxObject *excel = new QAxObject(this);
//    //连接Excel控件
//    excel->setControl("Excel.Application");
//    if (excel->isNull()) {
//        QMessageBox::critical(this, tr("错误"), tr("未能创建 Excel 对象，请安装 Microsoft Excel。"));
//        delete excel;
//        return;
//    }
//    excel->dynamicCall("SetVisible(bool)", false); // 不显示 Excel 界面

//    QAxObject *workbooks = excel->querySubObject("Workbooks");
//    QAxObject *workbook = workbooks->querySubObject("Open(const QString&)", filePath);
//    QAxObject *worksheets = workbook->querySubObject("Worksheets");

//    // 假设只导入第一个工作表
//    QAxObject *worksheet = worksheets->querySubObject("Item(int)", 1);
//    QAxObject *usedRange = worksheet->querySubObject("UsedRange");
//    QAxObject *rows = usedRange->querySubObject("Rows");
//    QAxObject *columns = usedRange->querySubObject("Columns");

//    //计算Excel表格行列数
//    int excel_rowCount = rows->property("Count").toInt();
//    int excel_columnCount = columns->property("Count").toInt();

//    // 设置表格的行列数
//    table->setRowCount(excel_rowCount-1);// 不算表头的行数
//    table->setColumnCount(excel_columnCount+2); //加上一二列

//    // 读取数据并填充表格
//    //以Excel的行列为基准
//    for (int i=2;i<=excel_rowCount;i++)
//    {
//        for(int j=1;j<=excel_columnCount;j++){
//            QAxObject *cell = worksheet->querySubObject("Cells(int,int)", i, j);
//            QString value = cell->dynamicCall("Value()").toString();

//            QTableWidgetItem *item = new QTableWidgetItem(value);
//            table->setItem(i-2, j+1, item);
//            item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
//            item->setFlags(Qt::ItemIsEditable); // 设置为只读
//            item->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
//            item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色
//        }

//        // 处理复选框
//        QCheckBox* checkBox = new QCheckBox();
//        QWidget* widget = new QWidget();
//        widget->setStyleSheet("background-color: #C0C0C0;");
//        QHBoxLayout* layout = new QHBoxLayout(widget);
//        layout->addWidget(checkBox);
//        layout->setAlignment(Qt::AlignCenter);
//        layout->setContentsMargins(0, 0, 0, 0);
//        table->setCellWidget(i-2, 0, widget);
//    }

//    //合并
//    int spancolf=spancolfirst;
//    int spancoll=spancollast;
//    int row=1;
//    for(int i = 0; i < table->rowCount(); i+=2)
//    {
//        for (int j=0;j<table->columnCount();j++) {
//            if(j <= spancolf || j >= spancoll)
//            {
//                table->setSpan(i, j, 2, 1);
//            }
//        }
//        //添加序号
//        QTableWidgetItem *item = new QTableWidgetItem(QString::number(row++));
//        table->setItem(i, 1, item);
//        item->setTextAlignment(Qt::AlignCenter); // 将内容居中对齐
//        item->setFlags(Qt::ItemIsEditable); // 设置为只读
//        item->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
//        item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 只读单元格文本颜色设置为深灰色
//    }

//    workbook->dynamicCall("Close()");//关闭工作簿
//    excel->dynamicCall("Quit()");//关闭excel
//    delete excel;
//    excel=NULL;

//}

////噪声源
//void Widget::on_pushButton_fanNoi_input_clicked(){
//    LoadExeclData_noisourse(ui->tableWidget_fan_noi,6,17);
//}

//void Widget::on_pushButton_fanNoi_output_clicked(){
//    SaveExeclData_noisourse(ui->tableWidget_fan_noi,"风机",5,16);
//}

//void Widget::on_pushButton_fanCoil_noi_input_clicked(){
//    LoadExeclData_noisourse(ui->tableWidget_fanCoil_noi,6,17);
//}

//void Widget::on_pushButton_fanCoil_noi_output_clicked(){
//    SaveExeclData_noisourse(ui->tableWidget_fanCoil_noi,"风机盘管",5,16);
//}

//void Widget::on_pushButton_air_noi_single_fan_input_clicked(){
//    //空调器单风机
//    LoadExeclData_noisourse(ui->tableWidget_air_noi_single_fan,7,18);
//}

//void Widget::on_pushButton_air_noi_single_fan_output_clicked(){
//    SaveExeclData_noisourse(ui->tableWidget_air_noi_single_fan,"空调器单风机",6,17);
//}

//void Widget::on_pushButton_air_noi_double_fan_input_clicked(){
//    //空调器多风机
//}

//void Widget::on_pushButton_air_noi_double_fan_output_clicked(){

//}

////其他器件
//void Widget::on_pushButton_VAV_terminal_input_clicked(){
//    LoadExeclData(ui->tableWidget_VAV_terminal);
//}

//void Widget::on_pushButton_VAV_terminal_output_clicked(){
//    SaveExeclData(ui->tableWidget_VAV_terminal,"变风量末端");
//}

//void Widget::on_pushButton_circular_damper_input_clicked(){
//    LoadExeclData(ui->tableWidget_circular_damper);
//}

//void Widget::on_pushButton_circular_damper_output_clicked(){
//    SaveExeclData(ui->tableWidget_circular_damper,"圆形调风门");
//}

//void Widget::on_pushButton_rect_damper_input_clicked(){
//    LoadExeclData(ui->tableWidget_rect_damper);
//}

//void Widget::on_pushButton_rect_damper_output_clicked(){
//    SaveExeclData(ui->tableWidget_rect_damper,"方形调风门");
//}

//void Widget::on_pushButton_air_diff_input_clicked(){
//    LoadExeclData(ui->tableWidget_air_diff);
//}

//void Widget::on_pushButton_air_diff_output_clicked(){
//    SaveExeclData(ui->tableWidget_air_diff,"布风器+散流器");
//}

//void Widget::on_pushButton_pump_send_input_clicked(){
//    //询问是送风头还是抽风头
//    QMessageBox messageBox;
//    messageBox.setWindowTitle("提示");
//    messageBox.setText("导入抽风头还是送风头？");

//    // 添加自定义按钮，并设置按钮文本
//    QPushButton *b1 = messageBox.addButton("抽风头", QMessageBox::ActionRole);
//    QPushButton *b2 = messageBox.addButton("送风头", QMessageBox::ActionRole);
//    QPushButton *b5 = messageBox.addButton(QMessageBox::Abort);
//    b5->setText("取消");

//    // 显示消息框并等待用户响应
//    messageBox.exec();

//    // 根据用户响应执行相应的操作
//    if (messageBox.clickedButton() == b1) {
//        LoadExeclData(ui->tableWidget_pump_tuyere);
//    } else if (messageBox.clickedButton() == b2) {
//        LoadExeclData(ui->tableWidget_send_tuyere);
//    }
//}

//void Widget::on_pushButton_pump_send_output_clicked(){
//    //询问是送风头还是抽风头
//    QMessageBox messageBox;
//    messageBox.setWindowTitle("提示");
//    messageBox.setText("导出抽风头还是送风头？");

//    // 添加自定义按钮，并设置按钮文本
//    QPushButton *b1 = messageBox.addButton("抽风头", QMessageBox::ActionRole);
//    QPushButton *b2 = messageBox.addButton("送风头", QMessageBox::ActionRole);
//    QPushButton *b5 = messageBox.addButton(QMessageBox::Abort);
//    b5->setText("取消");

//    // 显示消息框并等待用户响应
//    messageBox.exec();

//    // 根据用户响应执行相应的操作
//    if (messageBox.clickedButton() == b1) {
//        SaveExeclData(ui->tableWidget_pump_tuyere,"抽风头");
//    } else if (messageBox.clickedButton() == b2) {
//        SaveExeclData(ui->tableWidget_send_tuyere,"送风头");
//    }
//}

//void Widget::on_pushButton_staticBox_grille_input_clicked(){
//    LoadExeclData(ui->tableWidget_staticBox_grille);
//}

//void Widget::on_pushButton_staticBox_grille_output_clicked(){
//    SaveExeclData(ui->tableWidget_staticBox_grille,"静压箱+格栅");
//}


//void Widget::on_pushButton_disp_vent_terminal_input_clicked(){
//    LoadExeclData(ui->tableWidget_disp_vent_terminal);
//}

//void Widget::on_pushButton_disp_vent_terminal_output_clicked()
//{
//    SaveExeclData(ui->tableWidget_disp_vent_terminal,"置换通风末端");
//}

//void Widget::on_pushButton_other_send_terminal_input_clicked(){
//    LoadExeclData(ui->tableWidget_other_send_terminal);
//}

//void Widget::on_pushButton_other_send_terminal_output_clicked(){
//    SaveExeclData(ui->tableWidget_other_send_terminal,"其他送风末端");
//}

//void Widget::on_pushButton_static_box_input_clicked(){
//    LoadExeclData(ui->tableWidget_static_box);
//}

//void Widget::on_pushButton_static_box_output_clicked(){
//    SaveExeclData(ui->tableWidget_static_box,"静压箱");
//}

//void Widget::on_pushButton_duct_with_multi_ranc_input_clicked(){
//    LoadExeclData(ui->tableWidget_duct_with_multi_ranc);
//}

//void Widget::on_pushButton_duct_with_multi_ranc_output_clicked(){
//    SaveExeclData(ui->tableWidget_duct_with_multi_ranc,"风道多分支");
//}

//void Widget::on_pushButton_tee_input_clicked(){
//    LoadExeclData(ui->tableWidget_tee);
//}

//void Widget::on_pushButton_tee_output_clicked(){
//    SaveExeclData(ui->tableWidget_tee,"三通");
//}

//void Widget::on_pushButton_pipe_input_clicked(){
//    LoadExeclData(ui->tableWidget_pipe);
//}

//void Widget::on_pushButton_pipe_output_clicked(){
//    SaveExeclData(ui->tableWidget_pipe,"直管");
//}

//void Widget::on_pushButton_elbow_input_clicked(){
//    LoadExeclData(ui->tableWidget_elbow);
//}

//void Widget::on_pushButton_elbow_output_clicked(){
//    SaveExeclData(ui->tableWidget_elbow,"弯头");
//}

//void Widget::on_pushButton_reducer_input_clicked(){
//    LoadExeclData(ui->tableWidget_reducer);
//}

//void Widget::on_pushButton_reducer_output_clicked(){
//    SaveExeclData(ui->tableWidget_reducer,"变径");
//}

//void Widget::on_pushButton_silencer_input_clicked(){
//    //询问哪个消音器
//    QMessageBox msgBox;
//    msgBox.setWindowTitle("提示");
//    msgBox.setText("导入哪种消音器？");

//    QPushButton *b1 = msgBox.addButton("圆形消音器", QMessageBox::ActionRole);
//    QPushButton *b2 = msgBox.addButton("矩形消音器", QMessageBox::ActionRole);
//    QPushButton *b3 = msgBox.addButton("圆形消音弯头", QMessageBox::ActionRole);
//    QPushButton *b4 = msgBox.addButton("矩形消音弯头", QMessageBox::ActionRole);
//    QPushButton *b5 = msgBox.addButton(QMessageBox::Abort);
//    b5->setText("取消");

//    msgBox.exec();

//    if (msgBox.clickedButton() == b1) {
//        LoadExeclData(ui->tableWidget_circular_silencer);
//    } else if (msgBox.clickedButton() == b2) {
//        LoadExeclData(ui->tableWidget_rect_silencer);
//    } else if (msgBox.clickedButton() == b3) {
//        LoadExeclData(ui->tableWidget_circular_silencerEb);
//    } else if (msgBox.clickedButton() == b4) {
//        LoadExeclData(ui->tableWidget_rect_silencerEb);
//    }
//}

//void Widget::on_pushButton_silencer_output_clicked(){
//    //询问哪个消音器
//    QMessageBox msgBox;
//    msgBox.setWindowTitle("提示");
//    msgBox.setText("导出哪种消音器？");

//    QPushButton *b1 = msgBox.addButton("圆形消音器", QMessageBox::ActionRole);
//    QPushButton *b2 = msgBox.addButton("矩形消音器", QMessageBox::ActionRole);
//    QPushButton *b3 = msgBox.addButton("圆形消音弯头", QMessageBox::ActionRole);
//    QPushButton *b4 = msgBox.addButton("矩形消音弯头", QMessageBox::ActionRole);
//    QPushButton *b5 = msgBox.addButton(QMessageBox::Abort);
//    b5->setText("取消");
//    msgBox.exec();

//    if (msgBox.clickedButton() == b1) {
//        SaveExeclData(ui->tableWidget_circular_silencer,"圆形消音器");
//    } else if (msgBox.clickedButton() == b2) {
//        SaveExeclData(ui->tableWidget_rect_silencer,"矩形消音器");
//    } else if (msgBox.clickedButton() == b3) {
//        SaveExeclData(ui->tableWidget_circular_silencerEb,"圆形消音弯头");
//    } else if (msgBox.clickedButton() == b4) {
//        SaveExeclData(ui->tableWidget_rect_silencerEb,"矩形消音弯头");
//    }


//}

#pragma end}
/******************导入导出表格********************/

void Widget::closeEvent(QCloseEvent *event)
{
    if (componentDB) {
        componentDB->close(); // 关闭子窗口
        delete componentDB; // 释放内存
        componentDB = nullptr; // 将指针设为nullptr
    }

    QWidget::closeEvent(event); // 调用基类的关闭事件处理
}

