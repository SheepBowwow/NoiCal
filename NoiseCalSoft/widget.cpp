#include "widget.h"
#include "ui_widget.h"
/**窗口类**/
#include "create_prj.h"
#include "inputDialog/dialog_fan_noise.h"
#include "inputDialog/dialog_fancoil_noise.h"
#include "inputDialog/dialog_air_diff.h"
#include "inputDialog/dialog_pump_send.h"
#include "inputDialog/dialog_returnairbox_grille.h"
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
#include "roomDefineForm/dialog_add_zhushuqu.h"
#include "roomCal/room_cal_basewidget.h"
/**窗口类**/
#include <QDebug>
#include <QVector>
#include <QQueue>
#include <QColor>
#include <QTimer>
#include <QStandardItemModel>
#include <QCheckBox>
#include <QGraphicsDropShadowEffect>

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
    this->initTableWidget_return_air_box_grille();
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

void Widget::addRowToTable(QTableWidget *tableWidget, const QStringList &data, const char *addButtonSlot, const char *delButtonSlot)
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

void Widget::deleteRowFromTable(QTableWidget *tableWidget, int deleteRowNum)
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


    for (int i = selectedRows.size() - 1; i >= 0; --i)
    {
        int row = selectedRows[i];
        tableWidget->removeRow(row);
        if(deleteRowNum == 2)
        {
            tableWidget->removeRow(row - 1);
        }
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

void Widget::deleteRowFromTable(QTableWidget *tableWidget_noise, QTableWidget *tableWidget_atten, QTableWidget *tableWidget_refl)
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
    }
    delete noi;
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
    }
    delete noi;
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
//    item_input = new QTreeWidgetItem(item_prj,QStringList("输入模块"));        //输入模块
//    item_cal = new QTreeWidgetItem(item_prj,QStringList("计算模块"));          //计算模块
//    item_output = new QTreeWidgetItem(item_prj,QStringList("输出模块"));       //输出模块
//    item_auth_manage = new QTreeWidgetItem(item_prj,QStringList("权限管理"));   //权限管理

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
    item_return_air_box_grille = new QTreeWidgetItem(item_terminal_airflow_noise,QStringList("静压箱+格栅"));                            //2.2.3 回风箱+格栅
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
    item_return_air_box_grille_terminal_atten = new QTreeWidgetItem(item_terminal_atten,QStringList("回风箱+格栅"));
    item_disp_vent_terminal_atten = new QTreeWidgetItem(item_terminal_atten,QStringList("置换通风末端"));
    item_other_send_terminal_atten = new QTreeWidgetItem(item_terminal_atten,QStringList("其他送风末端"));

    item_terminal_refl_atten = new QTreeWidgetItem(item_noise_atten_in_pipe_acce,QStringList("末端反射衰减")); //3.7 末端反射衰减
    item_air_diff_relf_atten = new QTreeWidgetItem(item_terminal_refl_atten,QStringList("布风器+散流器"));
    item_pump_send_tuyere_relf_atten = new QTreeWidgetItem(item_terminal_refl_atten,QStringList("抽/送风头"));
    item_return_air_box_grille_relf_atten = new QTreeWidgetItem(item_terminal_refl_atten,QStringList("回风箱+格栅"));
    item_disp_vent_relf_atten = new QTreeWidgetItem(item_terminal_refl_atten,QStringList("置换通风末端"));
    item_other_send_relf_atten = new QTreeWidgetItem(item_terminal_refl_atten,QStringList("其他送风末端"));

    item_room_atten = new QTreeWidgetItem(QStringList("5.声压级计算类型"));                                   //5. 房间衰减
    item_room_less425 = new QTreeWidgetItem(item_room_atten,QStringList("<425m³的房间(点噪声源)"));      //5.1 体积小于425m³的房间（点噪声源）
    item_room_more425 = new QTreeWidgetItem(item_room_atten,QStringList(">425m³的房间(点噪声源)"));       //5.2 体积大于425m³的房间（点噪声源）
    item_room_noFurniture = new QTreeWidgetItem(item_room_atten,QStringList("无家具房间(点噪声源)"));    //5.3 无家具房间（点噪声源）
    item_room_open = new QTreeWidgetItem(item_room_atten,QStringList("室外开敞住所(点噪声源)"));           //5.4 室外开敞住所（点噪声源）
    item_room_gap_tuyere = new QTreeWidgetItem(item_room_atten,QStringList("条缝风口房间(线噪声源)"));     //5.5 条缝风口房间（线噪声源）
    item_room_rain = new QTreeWidgetItem(item_room_atten,QStringList("雨降风口房间(面噪声源)"));           //5.6 雨降风口房间（面噪声源）

    item_room_define = new QTreeWidgetItem(QStringList("6.计算房间"));
    item_room_calculate = new QTreeWidgetItem(QStringList("7.噪音计算"));

    ui->treeWidget->addTopLevelItem(item_prj_info);     //工程信息
    ui->treeWidget->addTopLevelItem(item_sound_sorce_noise);    //音源噪音
    ui->treeWidget->addTopLevelItem(item_pipe_and_acce_airflow_noise);    //管路及附件气流噪音
    ui->treeWidget->addTopLevelItem(item_noise_atten_in_pipe_acce);    //管路及附件噪音衰减
    ui->treeWidget->addTopLevelItem(item_terminal_refl_atten);    //末端反射衰减
    ui->treeWidget->addTopLevelItem(item_room_atten);    //房间衰减
    ui->treeWidget->addTopLevelItem(item_room_define);
    ui->treeWidget->addTopLevelItem(item_room_calculate);

    // 设置子项为展开状态
    item_prj->setExpanded(true); // 这一行将子项设置为展开状态
}

//工程管理按钮
void Widget::on_pushButto_prj_manage_clicked()
{
    create_prj *newPrjWindow = new create_prj();  // 创建create_prj对象，传入当前窗口为父窗口

    // 禁用主窗口的用户输入
    setEnabled(false);

    // 显示新窗口
    newPrjWindow->show();

    if(ui->stackedWidget->currentWidget() != ui->page_prj_info)
        ui->stackedWidget->setCurrentWidget(ui->page_prj_info);

    // 等待新窗口关闭
    while (newPrjWindow->isVisible()) {
        QCoreApplication::processEvents();
    }

    // 禁用主窗口的用户输入
    setEnabled(true);

    ui->lineEdit_boat_num->setText(project.prj_name);
    initializeTreeWidget();
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonNoiLimitClicked()
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

    // 处理加减按钮
    QWidget* widget_addDel = new QWidget();
    QHBoxLayout* layout_addDel = new QHBoxLayout(widget_addDel);

    QPushButton* addButton = new QPushButton("+");
    addButton->setFixedSize(15, 15);
    layout_addDel->addWidget(addButton);

    QPushButton* delButton = new QPushButton("-");
    delButton->setFixedSize(15, 15);
    layout_addDel->addWidget(delButton);

    ui->tableWidget_noi_limit->setCellWidget(rowCount, ui->tableWidget_noi_limit->columnCount() - 1, widget_addDel);

    // 在构造函数或者初始化函数中连接信号和槽函数
    connect(addButton, SIGNAL(clicked()), this, SLOT(onAddButtonNoiLimitClicked()));
    connect(delButton, SIGNAL(clicked()), this, SLOT(onDelButtonNoiLimitClicked()));
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonNoiLimitClicked()
{
    deleteRowFromTable(ui->tableWidget_noi_limit, 1);

    for (int row = 0; row < ui->tableWidget_noi_limit->rowCount(); ++row) {
        QTableWidgetItem* item = ui->tableWidget_noi_limit->item(row, 1);
        item->setBackground(QBrush(Qt::white));
        item->setData(Qt::ForegroundRole, QColor(0, 0, 0));
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
    int colCount = 6;
    // 设置表头标题
    QStringList headerText;
    headerText << "" << "序号" << "房间类型" << "噪声限值dB(A)" << "处所类型" << "";
    // 设置每列的宽度
    int columnWidths[] = {30, 38, 130, 190, 125, 90};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_noi_limit, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_noi_limit, ui->buttonWidget_noi_limit,
                      SLOT(onAddButtonNoiLimitClicked()), SLOT(onDelButtonNoiLimitClicked()));
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

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonFanNoiClicked()
{

}

void Widget::on_pushButton_fanNoi_add_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_fan_noi;
    int rowCount = tableWidget->rowCount(); //获取当前行数
    Fan_noise* noi;
    Dialog_fan_noise *dialog = new Dialog_fan_noise(this);
    const char *addButtonSlot = SLOT(onAddButtonFanNoiClicked());
    const char *delButtonSlot = SLOT(onDelButtonFanNoiClicked());

    if (dialog->exec() == QDialog::Accepted) {

        noi = static_cast<Fan_noise*>(dialog->getNoi());
        if (noi != nullptr) {
            QStringList data_in = {
                QString::number(tableWidget->rowCount() / 2 + 1),
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
            addRowToTable(tableWidget, data_in, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget, data_out, addButtonSlot, delButtonSlot);
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
    deleteRowFromTable(ui->tableWidget_fan_noi, 2);
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonFanNoiClicked()
{

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
    int colCount = 20;
    QStringList headerText;
    headerText << "" << "序号" << "编号" << "品牌" << "类型" << "型号" << "风量" << "静压" << "噪音位置" << "63Hz" << "125Hz" << "250Hz"
               << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "";
    int columnWidths[] = {30, 40, 120, 90, 90, 90, 90, 90, 80, 55, 55, 55, 55, 55, 55, 55, 55, 80, 55, 60};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_fanCoil_noi, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_fanCoil_noi, ui->buttonWidget_fanCoil_noi,
                      SLOT(onAddButtonFanCoilNoiClicked()), SLOT(onDelButtonFanCoilNoiClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonFanCoilNoiClicked()
{
    //修改
    QTableWidget *tableWidget = ui->tableWidget_fanCoil_noi;
    Dialog_fanCoil_noise *dialog = new Dialog_fanCoil_noise(this);
    FanCoil_noise* noi;
    const char *addButtonSlot = SLOT(onAddButtonFanCoilNoiClicked());
    const char *delButtonSlot = SLOT(onDelButtonFanCoilNoiClicked());

    int rowCount = tableWidget->rowCount(); //获取当前行数
    if (dialog->exec() == QDialog::Accepted) {
        noi = static_cast<FanCoil_noise*>(dialog->getNoi());
        if (noi != nullptr) {
            QStringList data_in = {
                QString::number(tableWidget->rowCount() / 2 + 1),
                noi->number,
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
            addRowToTable(tableWidget, data_in, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget, data_out, addButtonSlot, delButtonSlot);
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

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonFanCoilNoiClicked()
{
    deleteRowFromTable(ui->tableWidget_fanCoil_noi, 2);
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
            noi->number = ui->tableWidget_fanCoil_noi->item(row,2)->text();
            noi->brand = ui->tableWidget_fanCoil_noi->item(row,3)->text();
            noi->type = ui->tableWidget_fanCoil_noi->item(row,4)->text();
            noi->model = ui->tableWidget_fanCoil_noi->item(row,5)->text();
            noi->air_volume = ui->tableWidget_fanCoil_noi->item(row,6)->text();
            noi->static_pressure = ui->tableWidget_fanCoil_noi->item(row,7)->text();

            noi->noi_in_63 = ui->tableWidget_fanCoil_noi->item(row,9)->text();
            noi->noi_in_125 = ui->tableWidget_fanCoil_noi->item(row,10)->text();
            noi->noi_in_250 = ui->tableWidget_fanCoil_noi->item(row,11)->text();
            noi->noi_in_500 = ui->tableWidget_fanCoil_noi->item(row,12)->text();
            noi->noi_in_1k = ui->tableWidget_fanCoil_noi->item(row,13)->text();
            noi->noi_in_2k = ui->tableWidget_fanCoil_noi->item(row,14)->text();
            noi->noi_in_4k = ui->tableWidget_fanCoil_noi->item(row,15)->text();
            noi->noi_in_8k = ui->tableWidget_fanCoil_noi->item(row,16)->text();
            noi->noi_in_total = ui->tableWidget_fanCoil_noi->item(row,17)->text();

            noi->noi_out_63 = ui->tableWidget_fanCoil_noi->item(row + 1,9)->text();
            noi->noi_out_125 = ui->tableWidget_fanCoil_noi->item(row + 1,10)->text();
            noi->noi_out_250 = ui->tableWidget_fanCoil_noi->item(row + 1,11)->text();
            noi->noi_out_500 = ui->tableWidget_fanCoil_noi->item(row + 1,12)->text();
            noi->noi_out_1k = ui->tableWidget_fanCoil_noi->item(row + 1,13)->text();
            noi->noi_out_2k = ui->tableWidget_fanCoil_noi->item(row + 1,14)->text();
            noi->noi_out_4k = ui->tableWidget_fanCoil_noi->item(row + 1,15)->text();
            noi->noi_out_8k = ui->tableWidget_fanCoil_noi->item(row + 1,16)->text();
            noi->noi_out_total = ui->tableWidget_fanCoil_noi->item(row + 1,17)->text();
            // 创建模态对话框，并设置为模态
            Dialog_fanCoil_noise *fanCoilNoiseDialog = new Dialog_fanCoil_noise(this,row,*noi);
            fanCoilNoiseDialog->setWindowModality(Qt::ApplicationModal);
            fanCoilNoiseDialog->setModal(true);

            // 显示对话框,等待用户操作
            if (fanCoilNoiseDialog->exec() == QDialog::Accepted)
            {
                noi = static_cast<FanCoil_noise*>(fanCoilNoiseDialog->getNoi());
                ui->tableWidget_fanCoil_noi->item(row,2)->setText(noi->number);
                ui->tableWidget_fanCoil_noi->item(row,3)->setText(noi->brand);
                ui->tableWidget_fanCoil_noi->item(row,4)->setText(noi->type);
                ui->tableWidget_fanCoil_noi->item(row,5)->setText(noi->model);
                ui->tableWidget_fanCoil_noi->item(row,6)->setText(noi->air_volume);
                ui->tableWidget_fanCoil_noi->item(row,7)->setText(noi->static_pressure);

                ui->tableWidget_fanCoil_noi->item(row,9)->setText(noi->noi_in_63);
                ui->tableWidget_fanCoil_noi->item(row,10)->setText(noi->noi_in_125);
                ui->tableWidget_fanCoil_noi->item(row,11)->setText(noi->noi_in_250);
                ui->tableWidget_fanCoil_noi->item(row,12)->setText(noi->noi_in_500);
                ui->tableWidget_fanCoil_noi->item(row,13)->setText(noi->noi_in_1k);
                ui->tableWidget_fanCoil_noi->item(row,14)->setText(noi->noi_in_2k);
                ui->tableWidget_fanCoil_noi->item(row,15)->setText(noi->noi_in_4k);
                ui->tableWidget_fanCoil_noi->item(row,16)->setText(noi->noi_in_8k);
                ui->tableWidget_fanCoil_noi->item(row,17)->setText(noi->noi_in_total);

                ui->tableWidget_fanCoil_noi->item(row + 1,9)->setText(noi->noi_out_63);
                ui->tableWidget_fanCoil_noi->item(row + 1,10)->setText(noi->noi_out_125);
                ui->tableWidget_fanCoil_noi->item(row + 1,11)->setText(noi->noi_out_250);
                ui->tableWidget_fanCoil_noi->item(row + 1,12)->setText(noi->noi_out_500);
                ui->tableWidget_fanCoil_noi->item(row + 1,13)->setText(noi->noi_out_1k);
                ui->tableWidget_fanCoil_noi->item(row + 1,14)->setText(noi->noi_out_2k);
                ui->tableWidget_fanCoil_noi->item(row + 1,15)->setText(noi->noi_out_4k);
                ui->tableWidget_fanCoil_noi->item(row + 1,16)->setText(noi->noi_out_8k);
                ui->tableWidget_fanCoil_noi->item(row + 1,17)->setText(noi->noi_out_total);
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
    headerText << "" << "序号" << "编号" << "品牌" << "型号" << "风量" << "静压" << "噪音位置" << "63Hz" << "125Hz" << "250Hz"
               << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "";

    int columnWidths[] = {30, 38, 120, 100, 100, 90, 90, 80, 55, 55, 55, 55, 55, 55, 55, 55, 90, 60, 55};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_air_noi, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_air_noi, ui->buttonWidget_air_noi,
                      SLOT(onAddButtonAirNoiClicked()), SLOT(onDelButtonAirNoiClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonAirNoiClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_air_noi;
    Dialog_aircondition_noise *dialog = new Dialog_aircondition_noise(this);
    Aircondition_noise* noi;
    const char *addButtonSlot = SLOT(onAddButtonAirNoiClicked());
    const char *delButtonSlot = SLOT(onDelButtonAirNoiClicked());

    int rowCount = tableWidget->rowCount(); //获取当前行数
    if (dialog->exec() == QDialog::Accepted) {
        noi = static_cast<Aircondition_noise*>(dialog->getNoi());
        if (noi != nullptr) {
            QStringList data_in = {
                QString::number(tableWidget->rowCount() / 2 + 1),
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
            addRowToTable(tableWidget, data_in, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget, data_out, addButtonSlot, delButtonSlot);
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

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonAirNoiClicked()
{
    deleteRowFromTable(ui->tableWidget_air_noi, 2);
}

//修改按钮
void Widget::on_pushButton_air_noi_revise_clicked()
{
    int colCount = 19;
    for (int row = 0; row < ui->tableWidget_air_noi->rowCount(); ++row) {
        QWidget* widget = ui->tableWidget_air_noi->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked()) {
            Aircondition_noise *noi = new Aircondition_noise();
            noi->number = ui->tableWidget_air_noi->item(row,2)->text();
            noi->brand = ui->tableWidget_air_noi->item(row,3)->text();
            noi->model = ui->tableWidget_air_noi->item(row,4)->text();
            noi->air_volume = ui->tableWidget_air_noi->item(row,5)->text();
            noi->static_pressure = ui->tableWidget_air_noi->item(row,6)->text();

            noi->noi_in_63 = ui->tableWidget_air_noi->item(row,8)->text();
            noi->noi_in_125 = ui->tableWidget_air_noi->item(row,9)->text();
            noi->noi_in_250 = ui->tableWidget_air_noi->item(row,10)->text();
            noi->noi_in_500 = ui->tableWidget_air_noi->item(row,11)->text();
            noi->noi_in_1k = ui->tableWidget_air_noi->item(row,12)->text();
            noi->noi_in_2k = ui->tableWidget_air_noi->item(row,13)->text();
            noi->noi_in_4k = ui->tableWidget_air_noi->item(row,14)->text();
            noi->noi_in_8k = ui->tableWidget_air_noi->item(row,15)->text();
            noi->noi_in_total = ui->tableWidget_air_noi->item(row,16)->text();

            noi->noi_out_63 = ui->tableWidget_air_noi->item(row + 1,8)->text();
            noi->noi_out_125 = ui->tableWidget_air_noi->item(row + 1,9)->text();
            noi->noi_out_250 = ui->tableWidget_air_noi->item(row + 1,10)->text();
            noi->noi_out_500 = ui->tableWidget_air_noi->item(row + 1,11)->text();
            noi->noi_out_1k = ui->tableWidget_air_noi->item(row + 1,12)->text();
            noi->noi_out_2k = ui->tableWidget_air_noi->item(row + 1,13)->text();
            noi->noi_out_4k = ui->tableWidget_air_noi->item(row + 1,14)->text();
            noi->noi_out_8k = ui->tableWidget_air_noi->item(row + 1,15)->text();
            noi->noi_out_total = ui->tableWidget_air_noi->item(row + 1,16)->text();
            // 创建模态对话框，并设置为模态
            Dialog_aircondition_noise *airNoiseDialog = new Dialog_aircondition_noise(this,row,*noi);
            airNoiseDialog->setWindowModality(Qt::ApplicationModal);
            airNoiseDialog->setModal(true);

            // 显示对话框,等待用户操作
            if (airNoiseDialog->exec() == QDialog::Accepted)
            {
                noi = static_cast<Aircondition_noise*>(airNoiseDialog->getNoi());
                ui->tableWidget_air_noi->item(row,2)->setText(noi->number);
                ui->tableWidget_air_noi->item(row,3)->setText(noi->brand);
                ui->tableWidget_air_noi->item(row,4)->setText(noi->model);
                ui->tableWidget_air_noi->item(row,5)->setText(noi->air_volume);
                ui->tableWidget_air_noi->item(row,6)->setText(noi->static_pressure);

                ui->tableWidget_air_noi->item(row,8)->setText(noi->noi_in_63);
                ui->tableWidget_air_noi->item(row,9)->setText(noi->noi_in_125);
                ui->tableWidget_air_noi->item(row,10)->setText(noi->noi_in_250);
                ui->tableWidget_air_noi->item(row,11)->setText(noi->noi_in_500);
                ui->tableWidget_air_noi->item(row,12)->setText(noi->noi_in_1k);
                ui->tableWidget_air_noi->item(row,13)->setText(noi->noi_in_2k);
                ui->tableWidget_air_noi->item(row,14)->setText(noi->noi_in_4k);
                ui->tableWidget_air_noi->item(row,15)->setText(noi->noi_in_8k);
                ui->tableWidget_air_noi->item(row,16)->setText(noi->noi_in_total);

                ui->tableWidget_air_noi->item(row + 1,8)->setText(noi->noi_out_63);
                ui->tableWidget_air_noi->item(row + 1,9)->setText(noi->noi_out_125);
                ui->tableWidget_air_noi->item(row + 1,10)->setText(noi->noi_out_250);
                ui->tableWidget_air_noi->item(row + 1,11)->setText(noi->noi_out_500);
                ui->tableWidget_air_noi->item(row + 1,12)->setText(noi->noi_out_1k);
                ui->tableWidget_air_noi->item(row + 1,13)->setText(noi->noi_out_2k);
                ui->tableWidget_air_noi->item(row + 1,14)->setText(noi->noi_out_4k);
                ui->tableWidget_air_noi->item(row + 1,15)->setText(noi->noi_out_8k);
                ui->tableWidget_air_noi->item(row + 1,16)->setText(noi->noi_out_total);
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
    int colCount = 18;
    QStringList headerText;
    headerText<< "" << "序号" << "编号" << "品牌" << "型号" << "阀门开度" << "风量" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 140, 110, 140, 100, 100, 80, 80, 80, 80, 80, 80, 80, 80, 95 ,90, 70};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_VAV_terminal, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_VAV_terminal, ui->buttonWidget_VAV_terminal,
                      SLOT(onAddButtonVAVTerminalClicked()), SLOT(onDelButtonVAVTerminalClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonVAVTerminalClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_VAV_terminal;
    Dialog_VAV_terminal *dialog = new Dialog_VAV_terminal(this);
    VAV_terminal_noise* noi;
    const char *addButtonSlot = SLOT(onAddButtonVAVTerminalClicked());
    const char *delButtonSlot = SLOT(onDelButtonVAVTerminalClicked());

    if (dialog->exec() == QDialog::Accepted) {
       noi = static_cast<VAV_terminal_noise*>(dialog->getNoi());
        if (noi != nullptr) {
            QStringList data = {
                QString::number(tableWidget->rowCount() + 1),
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
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonVAVTerminalClicked()
{
    deleteRowFromTable(ui->tableWidget_VAV_terminal, 1);
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
    int colCount = 17;
    QStringList headerText;
    headerText<< "" << "序号" << "品牌" << "型号" << "阀门开度" << "风量" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 100, 120, 100, 100, 65, 65, 65, 65, 65, 65, 65, 65, 90, 70, 70};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_circular_damper, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_circular_damper, ui->buttonWidget_circular_damper,
                      SLOT(onAddButtonCirDamperClicked()), SLOT(onDelButtonCirDamperClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonCirDamperClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_circular_damper;
    Dialog_circular_damper *dialog = new Dialog_circular_damper(this);
    Circular_damper_noi* noi;
    const char *addButtonSlot = SLOT(onAddButtonCirDamperClicked());
    const char *delButtonSlot = SLOT(onDelButtonCirDamperClicked());

    if (dialog->exec() == QDialog::Accepted) {
       noi = static_cast<Circular_damper_noi*>(dialog->getNoi());
        if (noi != nullptr) {
            QStringList data = {
                QString::number(tableWidget->rowCount() + 1),
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
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonCirDamperClicked()
{
    deleteRowFromTable(ui->tableWidget_circular_damper, 1);
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
    int colCount = 17;
    QStringList headerText;
    headerText<< "" << "序号" << "品牌" << "型号" << "阀门开度" << "风量" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 110, 130, 110, 110, 65, 65, 65, 65, 65, 65, 65, 65, 90, 70, 70};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_rect_damper, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_rect_damper, ui->buttonWidget_rect_damper,
                      SLOT(onAddButtonRectDamperClicked()), SLOT(onDelButtonRectDamperClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonRectDamperClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_rect_damper;
    Dialog_rect_damper *dialog = new Dialog_rect_damper(this);
    Rect_damper_noi* noi;
    const char *addButtonSlot = SLOT(onAddButtonRectDamperClicked());
    const char *delButtonSlot = SLOT(onDelButtonRectDamperClicked());

    if (dialog->exec() == QDialog::Accepted) {
        noi = static_cast<Rect_damper_noi*>(dialog->getNoi());
        if (noi != nullptr) {
            QStringList data = {
                QString::number(tableWidget->rowCount() + 1),
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
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonRectDamperClicked()
{
    deleteRowFromTable(ui->tableWidget_rect_damper, 1);
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
    int colCount = 17;
    QStringList headerText;
    headerText<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 130, 250, 130, 130, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90, 70};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_air_diff, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_air_diff, ui->buttonWidget_air_diff,
                      SLOT(onAddButtonAirDiffNoiClicked()), SLOT(onDelButtonAirDiffNoiClicked()));

    colCount = 16;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_air_diff_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_air_diff_terminal_atten, ui->buttonWidget_air_diff_terminal_atten,
                      SLOT(onAddButtonAirDiffNoiClicked()), SLOT(onDelButtonAirDiffNoiClicked()));

    colCount = 16;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_air_diff_terminal_refl, headerText_refl, refl_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_air_diff_terminal_refl, ui->buttonWidget_air_diff_terminal_refl,
                      SLOT(onAddButtonAirDiffNoiClicked()), SLOT(onDelButtonAirDiffNoiClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonAirDiffNoiClicked()
{
    QTableWidget *tableWidget_noise = ui->tableWidget_air_diff;
    QTableWidget *tableWidget_atten = ui->tableWidget_air_diff_terminal_atten;
    QTableWidget *tableWidget_refl = ui->tableWidget_air_diff_terminal_refl;
    Dialog_air_diff *dialog = new Dialog_air_diff("布风器+散流器",this);
    AirDiff_noise* noi;
    const char *addButtonSlot = SLOT(onAddButtonAirDiffNoiClicked());
    const char *delButtonSlot = SLOT(onDelButtonAirDiffNoiClicked());

    if (dialog->exec() == QDialog::Accepted) {
        noi = static_cast<AirDiff_noise*>(dialog->getNoi());
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
            addRowToTable(tableWidget_noise, data_noise, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_atten, data_atten, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_refl, data_refl, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonAirDiffNoiClicked()
{
    deleteRowFromTable(ui->tableWidget_air_diff, ui->tableWidget_air_diff_terminal_atten, ui->tableWidget_air_diff_terminal_refl);
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
    int colCount = 17;
    QStringList headerText;
    headerText<< "抽" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 130, 250, 130, 130, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90, 70};

    initTableWidget(ui->tableWidget_pump_tuyere, headerText, columnWidths, colCount);

    buttonToHeader(ui->tableWidget_pump_tuyere, ui->buttonWidget_pump_tuyere,
                      SLOT(onAddButtonPumpClicked()), SLOT(onDelButtonPumpClicked()));

    colCount = 16;
    QStringList headerText_atten;
    headerText_atten<< "抽" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_pump_tuyere_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_pump_tuyere_terminal_atten, ui->buttonWidget_pump_tuyere_terminal_atten,
                      SLOT(onAddButtonPumpClicked()), SLOT(onDelButtonPumpClicked()));

    colCount = 16;
    QStringList headerText_refl;
    headerText_refl<< "抽" << "序号" << "品牌" << "型号" << "类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 100, 160, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_pump_tuyere_terminal_refl, headerText_refl, refl_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_pump_tuyere_terminal_refl, ui->buttonWidget_pump_tuyere_terminal_refl,
                      SLOT(onAddButtonPumpClicked()), SLOT(onDelButtonPumpClicked()));
}

void Widget::initTableWidget_send_tuyere()
{
    int colCount = 17;
    QStringList headerText;
    headerText<< "送" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 130, 250, 130, 130, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90, 70};

    initTableWidget(ui->tableWidget_send_tuyere, headerText, columnWidths, colCount);

    buttonToHeader(ui->tableWidget_send_tuyere, ui->buttonWidget_send_tuyere,
                      SLOT(onAddButtonSendClicked()), SLOT(onDelButtonSendClicked()));

    colCount = 16;
    QStringList headerText_atten;
    headerText_atten<< "送" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_send_tuyere_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_send_tuyere_terminal_atten, ui->buttonWidget_send_tuyere_terminal_atten,
                      SLOT(onAddButtonSendClicked()), SLOT(onDelButtonSendClicked()));

    colCount = 16;
    QStringList headerText_refl;
    headerText_refl<< "送" << "序号" << "品牌" << "型号" << "类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 100, 160, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_send_tuyere_terminal_refl, headerText_refl, refl_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_send_tuyere_terminal_refl, ui->buttonWidget_send_tuyere_terminal_refl,
                      SLOT(onAddButtonSendClicked()), SLOT(onDelButtonSendClicked()));
}


// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonPumpClicked()
{
    QTableWidget *tableWidget_noise = ui->tableWidget_pump_tuyere;
    QTableWidget *tableWidget_atten = ui->tableWidget_pump_tuyere_terminal_atten;
    QTableWidget *tableWidget_refl = ui->tableWidget_pump_tuyere_terminal_refl;
    Dialog_pump_send *dialog = new Dialog_pump_send("抽风头",this);
    PumpSend_noise* noi;
    const char *addButtonSlot = SLOT(onAddButtonPumpClicked());
    const char *delButtonSlot = SLOT(onDelButtonPumpClicked());

    if (dialog->exec() == QDialog::Accepted) {
        noi = static_cast<PumpSend_noise*>(dialog->getNoi());
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
            addRowToTable(tableWidget_noise, data_noise, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_atten, data_atten, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_refl, data_refl, addButtonSlot, delButtonSlot);
        }
    }
}

void Widget::onAddButtonSendClicked()
{
    QTableWidget *tableWidget_noise = ui->tableWidget_send_tuyere;
    QTableWidget *tableWidget_atten = ui->tableWidget_send_tuyere_terminal_atten;
    QTableWidget *tableWidget_refl = ui->tableWidget_send_tuyere_terminal_refl;
    Dialog_pump_send *dialog = new Dialog_pump_send("送风头",this);
    PumpSend_noise* noi;
    const char *addButtonSlot = SLOT(onAddButtonSendClicked());
    const char *delButtonSlot = SLOT(onDelButtonSendClicked());

    if (dialog->exec() == QDialog::Accepted) {
        noi = static_cast<PumpSend_noise*>(dialog->getNoi());
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
            addRowToTable(tableWidget_noise, data_noise, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_atten, data_atten, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_refl, data_refl, addButtonSlot, delButtonSlot);
        }
    }
}


// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonPumpClicked()
{
    deleteRowFromTable(ui->tableWidget_pump_tuyere, ui->tableWidget_pump_tuyere_terminal_atten, ui->tableWidget_pump_tuyere_terminal_refl);
}

void Widget::onDelButtonSendClicked()
{
    deleteRowFromTable(ui->tableWidget_send_tuyere, ui->tableWidget_send_tuyere_terminal_atten, ui->tableWidget_send_tuyere_terminal_refl);
}


//修改按钮
void Widget::on_pushButton_pump_tuyere_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_pump_tuyere;
    QTableWidget* tableWidget_noise = ui->tableWidget_pump_tuyere;
    QTableWidget* tableWidget_atten = ui->tableWidget_pump_tuyere_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_pump_tuyere_terminal_refl;
    PumpSend_noise *noi = new PumpSend_noise();
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
                                                        cols_noise, cols_atten, cols_refl,"抽风头");
    }
}

void Widget::on_pushButton_send_tuyere_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_send_tuyere;
    QTableWidget* tableWidget_noise = ui->tableWidget_send_tuyere;
    QTableWidget* tableWidget_atten = ui->tableWidget_send_tuyere_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_send_tuyere_terminal_refl;
    PumpSend_noise *noi = new PumpSend_noise();
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
                                                        cols_noise, cols_atten, cols_refl,"送风头");
    }
}

void Widget::on_pushButton_pump_tuyere_terminal_atten_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_pump_tuyere_terminal_atten;
    QTableWidget* tableWidget_noise = ui->tableWidget_pump_tuyere;
    QTableWidget* tableWidget_atten = ui->tableWidget_pump_tuyere_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_pump_tuyere_terminal_refl;
    PumpSend_noise *noi = new PumpSend_noise();
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
                                                        cols_noise, cols_atten, cols_refl,"抽风头");
    }
}

void Widget::on_pushButton_send_tuyere_terminal_atten_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_send_tuyere_terminal_atten;
    QTableWidget* tableWidget_noise = ui->tableWidget_send_tuyere;
    QTableWidget* tableWidget_atten = ui->tableWidget_send_tuyere_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_send_tuyere_terminal_refl;
    PumpSend_noise *noi = new PumpSend_noise();
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
                                                        cols_noise, cols_atten, cols_refl,"送风头");
    }
}

void Widget::on_pushButton_pump_tuyere_terminal_refl_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_pump_tuyere_terminal_refl;
    QTableWidget* tableWidget_noise = ui->tableWidget_pump_tuyere;
    QTableWidget* tableWidget_atten = ui->tableWidget_pump_tuyere_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_pump_tuyere_terminal_refl;
    PumpSend_noise *noi = new PumpSend_noise();
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
                                                        cols_noise, cols_atten, cols_refl,"抽风头");
    }
}

void Widget::on_pushButton_send_tuyere_terminal_refl_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_send_tuyere_terminal_refl;
    QTableWidget* tableWidget_noise = ui->tableWidget_send_tuyere;
    QTableWidget* tableWidget_atten = ui->tableWidget_send_tuyere_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_send_tuyere_terminal_refl;
    PumpSend_noise *noi = new PumpSend_noise();
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
                                                        cols_noise, cols_atten, cols_refl,"送风头");
    }
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
    on_pushButton_pump_tuyere_revise_clicked();
    checkBox->setChecked(false);
    item->setFlags(Qt::ItemIsEnabled); // 设置为只读
}

void Widget::on_tableWidget_send_tuyere_itemDoubleClicked(QTableWidgetItem *item)
{
    item->setFlags(Qt::ItemIsEditable); // 设置为只读

    QWidget* widget = ui->tableWidget_send_tuyere->cellWidget(item->row(), 0); // Assuming the checkbox is in the first column (index 0)
    QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
    checkBox->setChecked(true);
    on_pushButton_send_tuyere_revise_clicked();
    checkBox->setChecked(false);
    item->setFlags(Qt::ItemIsEnabled); // 设置为只读
}
#pragma endregion}
/**********抽/送风头**********/




/**********回风箱+格栅**********/
#pragma region "stack_return_air_box_grille"{
void Widget::initTableWidget_return_air_box_grille()
{
    int colCount = 17;
    QStringList headerText;
    headerText<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 130, 250, 130, 130, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90, 70};
    initTableWidget(ui->tableWidget_return_air_box_grille, headerText, columnWidths, colCount);

    buttonToHeader(ui->tableWidget_return_air_box_grille, ui->buttonWidget_return_air_box_grille,
                      SLOT(onAddButtonReturnAirBoxGriClicked()), SLOT(onDelButtonReturnAirBoxGriClicked()));

    colCount = 16;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_return_air_box_grille_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_return_air_box_grille_terminal_atten, ui->buttonWidget_return_air_box_grille_terminal_atten,
                      SLOT(onAddButtonReturnAirBoxGriClicked()), SLOT(onDelButtonReturnAirBoxGriClicked()));

    colCount = 16;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_return_air_box_grille_terminal_refl, headerText_refl, refl_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_return_air_box_grille_terminal_refl, ui->buttonWidget_return_air_box_grille_terminal_refl,
                      SLOT(onAddButtonReturnAirBoxGriClicked()), SLOT(onDelButtonReturnAirBoxGriClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonReturnAirBoxGriClicked()
{
    QTableWidget *tableWidget_noise = ui->tableWidget_return_air_box_grille;
    QTableWidget *tableWidget_atten = ui->tableWidget_return_air_box_grille_terminal_atten;
    QTableWidget *tableWidget_refl = ui->tableWidget_return_air_box_grille_terminal_refl;
    Dialog_returnAirBox_grille *dialog = new Dialog_returnAirBox_grille(this);
    ReturnAirBox_grille_noise* noi;
    const char *addButtonSlot = SLOT(onAddButtonReturnAirBoxGriClicked());
    const char *delButtonSlot = SLOT(onDelButtonReturnAirBoxGriClicked());

    if (dialog->exec() == QDialog::Accepted) {
        noi = static_cast<ReturnAirBox_grille_noise*>(dialog->getNoi());
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
            addRowToTable(tableWidget_noise, data_noise, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_atten, data_atten, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_refl, data_refl, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonReturnAirBoxGriClicked()
{
    deleteRowFromTable(ui->tableWidget_return_air_box_grille, ui->tableWidget_return_air_box_grille_terminal_atten, ui->tableWidget_return_air_box_grille_terminal_refl);
}

//修改按钮
void Widget::on_pushButton_return_air_box_grille_terminal_atten_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_return_air_box_grille_terminal_atten;
    QTableWidget* tableWidget_noise = ui->tableWidget_return_air_box_grille;
    QTableWidget* tableWidget_atten = ui->tableWidget_return_air_box_grille_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_return_air_box_grille_terminal_refl;
    ReturnAirBox_grille_noise *noi = new ReturnAirBox_grille_noise();
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
        noiseRevision<ReturnAirBox_grille_noise, Dialog_returnAirBox_grille>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
    }
}

void Widget::on_pushButton_return_air_box_grille_terminal_refl_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_return_air_box_grille_terminal_refl;
    QTableWidget* tableWidget_noise = ui->tableWidget_return_air_box_grille;
    QTableWidget* tableWidget_atten = ui->tableWidget_return_air_box_grille_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_return_air_box_grille_terminal_refl;
    ReturnAirBox_grille_noise *noi = new ReturnAirBox_grille_noise();
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
        noiseRevision<ReturnAirBox_grille_noise, Dialog_returnAirBox_grille>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
    }
}

void Widget::on_pushButton_return_air_box_grille_revise_clicked()
{
    QTableWidget* currentTableWidget = ui->tableWidget_return_air_box_grille;
    QTableWidget* tableWidget_noise = ui->tableWidget_return_air_box_grille;
    QTableWidget* tableWidget_atten = ui->tableWidget_return_air_box_grille_terminal_atten;
    QTableWidget* tableWidget_refl = ui->tableWidget_return_air_box_grille_terminal_refl;
    ReturnAirBox_grille_noise *noi = new ReturnAirBox_grille_noise();
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
        noiseRevision<ReturnAirBox_grille_noise, Dialog_returnAirBox_grille>(currentTableWidget, tableWidget_noise, tableWidget_atten, tableWidget_refl, row, noi, items_noise, items_atten, items_refl, cols_noise, cols_atten, cols_refl);
    }
}

#pragma endregion}
/**********回风箱+格栅**********/




/**********置换通风末端**********/
#pragma region "stack_disp_vent_terminal"{
//初始化表格
void Widget::initTableWidget_disp_vent_terminal()
{
    int colCount = 17;
    QStringList headerText;
    headerText<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 130, 250, 130, 130, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90, 70};

    initTableWidget(ui->tableWidget_disp_vent_terminal, headerText, columnWidths, colCount);

    buttonToHeader(ui->tableWidget_disp_vent_terminal, ui->buttonWidget_disp_vent_terminal,
                      SLOT(onAddButtonDispVentTerminalClicked()), SLOT(onDelButtonDispVentTerminalClicked()));

    colCount = 16;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_disp_vent_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_disp_vent_terminal_atten, ui->buttonWidget_disp_vent_terminal_atten,
                      SLOT(onAddButtonDispVentTerminalClicked()), SLOT(onDelButtonDispVentTerminalClicked()));

    colCount = 16;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 140, 230, 130, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_disp_vent_terminal_refl, headerText_refl, refl_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_disp_vent_terminal_refl, ui->buttonWidget_disp_vent_terminal_refl,
                      SLOT(onAddButtonDispVentTerminalClicked()), SLOT(onDelButtonDispVentTerminalClicked()));

}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonDispVentTerminalClicked()
{
    QTableWidget *tableWidget_noise = ui->tableWidget_disp_vent_terminal;
    QTableWidget *tableWidget_atten = ui->tableWidget_disp_vent_terminal_atten;
    QTableWidget *tableWidget_refl = ui->tableWidget_disp_vent_terminal_refl;
    Dialog_disp_vent_terminal *dialog = new Dialog_disp_vent_terminal(this);
    Disp_vent_terminal_noise* noi;
    const char *addButtonSlot = SLOT(onAddButtonDispVentTerminalClicked());
    const char *delButtonSlot = SLOT(onDelButtonDispVentTerminalClicked());

    if (dialog->exec() == QDialog::Accepted) {
        noi = static_cast<Disp_vent_terminal_noise*>(dialog->getNoi());
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
            addRowToTable(tableWidget_noise, data_noise, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_atten, data_atten, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_refl, data_refl, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonDispVentTerminalClicked()
{
    deleteRowFromTable(ui->tableWidget_disp_vent_terminal, ui->tableWidget_disp_vent_terminal_atten, ui->tableWidget_disp_vent_terminal_refl);
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
    int colCount = 18;
    QStringList headerText;
    headerText<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "总值dB(A)" << "来源" << "备注" << "";  //表头标题用QStringList来表示

    int columnWidths[] = {40, 50, 130, 250, 110, 110, 75, 75, 75, 75, 75, 75, 75, 75, 110 ,90, 180, 70};

    initTableWidget(ui->tableWidget_other_send_terminal, headerText, columnWidths, colCount);

    buttonToHeader(ui->tableWidget_other_send_terminal, ui->buttonWidget_other_send_terminal,
                      SLOT(onAddButtonOtherSendTerClicked()), SLOT(onDelButtonOtherSendTerClicked()));

    colCount = 17;
    QStringList headerText_atten;
    headerText_atten<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
                    << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "备注" << "";  //表头标题用QStringList来表示
    int atten_columnWidths[] = {40, 50, 140, 230, 110, 110, 80, 80, 80, 80, 80, 80, 80, 80, 90, 180, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_other_send_terminal_atten, headerText_atten, atten_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_other_send_terminal_atten, ui->buttonWidget_other_send_terminal_atten,
                      SLOT(onAddButtonOtherSendTerClicked()), SLOT(onDelButtonOtherSendTerClicked()));

    colCount = 17;
    QStringList headerText_refl;
    headerText_refl<< "" << "序号" << "品牌" << "型号" << "末端类型" << "末端尺寸" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "备注" << "";  //表头标题用QStringList来表示
    int refl_columnWidths[] = {40, 50, 140, 230, 110, 110, 80, 80, 80, 80, 80, 80, 80, 80, 90, 180, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_other_send_terminal_refl, headerText_refl, refl_columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_other_send_terminal_refl, ui->buttonWidget_other_send_terminal_refl,
                      SLOT(onAddButtonOtherSendTerClicked()), SLOT(onDelButtonOtherSendTerClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonOtherSendTerClicked()
{
    QTableWidget *tableWidget_noise = ui->tableWidget_other_send_terminal;
    QTableWidget *tableWidget_atten = ui->tableWidget_other_send_terminal_atten;
    QTableWidget *tableWidget_refl = ui->tableWidget_other_send_terminal_refl;
    Dialog_other_send_terminal *dialog = new Dialog_other_send_terminal(this);
    Other_send_terminal_noise* noi;
    const char *addButtonSlot = SLOT(onAddButtonOtherSendTerClicked());
    const char *delButtonSlot = SLOT(onDelButtonOtherSendTerClicked());

    if (dialog->exec() == QDialog::Accepted) {
        noi = static_cast<Other_send_terminal_noise*>(dialog->getNoi());
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
            addRowToTable(tableWidget_noise, data_noise, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_atten, data_atten, addButtonSlot, delButtonSlot);
            addRowToTable(tableWidget_refl, data_refl, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonOtherSendTerClicked()
{
    deleteRowFromTable(ui->tableWidget_other_send_terminal, ui->tableWidget_other_send_terminal_atten, ui->tableWidget_other_send_terminal_refl);
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
    int colCount = 14;
    QStringList headerText;
    headerText<< "" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 100, 120, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    initTableWidget(ui->tableWidget_static_box, headerText, columnWidths, colCount);

    buttonToHeader(ui->tableWidget_static_box, ui->buttonWidget_static_box,
                      SLOT(onAddButtonStaticBoxClicked()), SLOT(onDelButtonStaticBoxClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonStaticBoxClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_static_box;
    Dialog_static_box *dialog = new Dialog_static_box(this);
    Static_box* noi;
    const char *addButtonSlot = SLOT(onAddButtonStaticBoxClicked());
    const char *delButtonSlot = SLOT(onDelButtonStaticBoxClicked());

    if (dialog->exec() == QDialog::Accepted) {
        noi = static_cast<Static_box*>(dialog->getNoi());
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
                noi->getMode,
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonStaticBoxClicked()
{
    deleteRowFromTable(ui->tableWidget_static_box, 1);
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





/**********三通衰减**********/
#pragma region "stack_tee_atten"{
void Widget::initTableWidegt_tee()
{
    int colCount = 14;
    QStringList headerText;
    headerText<< "" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示

    int columnWidths[] = {40, 50, 100, 120, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};
    initTableWidget(ui->tableWidget_tee, headerText, columnWidths, colCount);

    buttonToHeader(ui->tableWidget_tee, ui->buttonWidget_tee,
                      SLOT(onAddButtonTeeClicked()), SLOT(onDelButtonTeeClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonTeeClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_tee;
    Dialog_tee *dialog = new Dialog_tee(this);
    Tee_atten* noi;
    const char *addButtonSlot = SLOT(onAddButtonTeeClicked());
    const char *delButtonSlot = SLOT(onDelButtonTeeClicked());

    if (dialog->exec() == QDialog::Accepted) {
       noi = static_cast<Tee_atten*>(dialog->getNoi());
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
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonTeeClicked()
{
    deleteRowFromTable(ui->tableWidget_tee, 1);
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



/**********风道多分支**********/
#pragma region "stack_duct_with_multi_ranc"{
void Widget::initTableWidegt_duct_with_multi_ranc()
{
    int colCount = 14;
    QStringList headerText;
    headerText<< "" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 100, 120, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    initTableWidget(ui->tableWidget_duct_with_multi_ranc, headerText, columnWidths, colCount);

    buttonToHeader(ui->tableWidget_duct_with_multi_ranc, ui->buttonWidget_duct_with_multi_ranc,
                      SLOT(onAddButtonMultiRancClicked()), SLOT(onDelButtonMultiRancClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonMultiRancClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_duct_with_multi_ranc;
    Dialog_duct_with_multi_ranc *dialog = new Dialog_duct_with_multi_ranc(this);
    Multi_ranc_atten* noi;
    const char *addButtonSlot = SLOT(onAddButtonMultiRancClicked());
    const char *delButtonSlot = SLOT(onDelButtonMultiRancClicked());

    if (dialog->exec() == QDialog::Accepted) {
       noi = static_cast<Multi_ranc_atten*>(dialog->getNoi());
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
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonMultiRancClicked()
{
    deleteRowFromTable(ui->tableWidget_duct_with_multi_ranc, 1);
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




/**********直管衰减**********/
#pragma region "stack_pipe_atten"{
void Widget::initTableWidget_pipe()
{
    int colCount = 14;

    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "类型" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示

    int columnWidths[] = {40, 50, 160, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};

    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_pipe, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_pipe, ui->buttonWidget_pipe,
                      SLOT(onAddButtonPipeClicked()), SLOT(onDelButtonPipeClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonPipeClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_pipe;
    Dialog_pipe *dialog = new Dialog_pipe(this);
    Pipe_atten* noi;
    const char *addButtonSlot = SLOT(onAddButtonPipeClicked());
    const char *delButtonSlot = SLOT(onDelButtonPipeClicked());

    if (dialog->exec() == QDialog::Accepted) {
       noi = static_cast<Pipe_atten*>(dialog->getNoi());
        if (noi != nullptr) {
            QStringList data = {
               QString::number(tableWidget->rowCount() + 1),
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
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonPipeClicked()
{
    deleteRowFromTable(ui->tableWidget_pipe, 1);
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
    int colCount = 14;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "类型" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 160, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_elbow, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_elbow, ui->buttonWidget_elbow,
                      SLOT(onAddButtonElbowClicked()), SLOT(onDelButtonElbowClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonElbowClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_elbow;
    Dialog_elbow *dialog = new Dialog_elbow(this);
    Elbow_atten* noi;
    const char *addButtonSlot = SLOT(onAddButtonElbowClicked());
    const char *delButtonSlot = SLOT(onDelButtonElbowClicked());

    if (dialog->exec() == QDialog::Accepted) {
       noi = static_cast<Elbow_atten*>(dialog->getNoi());
        if (noi != nullptr) {
            QStringList data = {
               QString::number(tableWidget->rowCount() + 1),
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
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonElbowClicked()
{
    deleteRowFromTable(ui->tableWidget_elbow, 1);
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
    int colCount = 14;

    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "类型" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 50, 160, 130, 80, 80, 80, 80, 80, 80, 80, 80, 90, 70};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_reducer, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_reducer, ui->buttonWidget_reducer,
                      SLOT(onAddButtonReducerClicked()), SLOT(onDelButtonReducerClicked()));
}

// 处理"+"按钮点击事件的逻辑
void Widget::onAddButtonReducerClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_reducer;
    Dialog_reducer *dialog = new Dialog_reducer(this);
    Reducer_atten* noi;
    const char *addButtonSlot = SLOT(onAddButtonElbowClicked());
    const char *delButtonSlot = SLOT(onDelButtonElbowClicked());

    if (dialog->exec() == QDialog::Accepted) {
       noi = static_cast<Reducer_atten*>(dialog->getNoi());
        if (noi != nullptr) {
            QStringList data = {
               QString::number(tableWidget->rowCount() + 1),
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
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

// 处理"-"按钮点击事件的逻辑
void Widget::onDelButtonReducerClicked()
{
    deleteRowFromTable(ui->tableWidget_reducer, 1);
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
    int colCount = 14;
    QStringList headerText;
    headerText<< "圆形" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 40, 120, 370, 82, 82, 82, 82, 82, 82, 82, 82, 90, 80};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_circular_silencer, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_circular_silencer, ui->buttonWidget_circular_silencer,
                      SLOT(onAddButtonCSilencerClicked()), SLOT(onDelButtonCSilencerClicked()));
}

void Widget::initTableWidget_rect_silencer()
{
    int colCount = 14;
    QStringList headerText;
    headerText<< "矩形" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 40, 120, 370, 82, 82, 82, 82, 82, 82, 82, 82, 90, 80};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_rect_silencer, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_rect_silencer, ui->buttonWidget_rect_silencer,
                      SLOT(onAddButtonRSilencerClicked()), SLOT(onDelButtonRSilencerClicked()));
}

void Widget::initTableWidget_circular_silencerEb()
{
    int colCount = 14;
    QStringList headerText;
    headerText<< "圆弯" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 40, 120, 370, 82, 82, 82, 82, 82, 82, 82, 82, 90, 80};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_circular_silencerEb, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_circular_silencerEb, ui->buttonWidget_circular_silencerEb,
                      SLOT(onAddButtonCSilencerEbClicked()), SLOT(onDelButtonCSilencerEbClicked()));
}

void Widget::initTableWidget_rect_silencerEb()
{
    int colCount = 14;
    QStringList headerText;
    headerText<< "矩弯" << "序号" << "品牌" << "型号" << "63Hz" << "125Hz" << "250Hz"
              << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz" << "来源" << "";  //表头标题用QStringList来表示
    int columnWidths[] = {40, 40, 120, 370, 82, 82, 82, 82, 82, 82, 82, 82, 90, 80};
    // 调用封装好的初始化表格函数
    initTableWidget(ui->tableWidget_rect_silencerEb, headerText, columnWidths, colCount);

    // 使用通用添加按钮到表头函数
    buttonToHeader(ui->tableWidget_rect_silencerEb, ui->buttonWidget_rect_silencerEb,
                      SLOT(onAddButtonRSilencerEbClicked()), SLOT(onDelButtonRSilencerEbClicked()));
}


void Widget::onAddButtonCSilencerClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_circular_silencer;
    Dialog_silencer *dialog = new Dialog_silencer("圆形消音器",this);
    Silencer_atten* noi;
    const char *addButtonSlot = SLOT(onAddButtonCSilencerClicked());
    const char *delButtonSlot = SLOT(onDelButtonCSilencerClicked());

    if (dialog->exec() == QDialog::Accepted) {
       noi = static_cast<Silencer_atten*>(dialog->getNoi());
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
               "厂家"
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

void Widget::onDelButtonCSilencerClicked()
{
    deleteRowFromTable(ui->tableWidget_circular_silencer, 1);
}

void Widget::onAddButtonRSilencerClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_rect_silencer;
    Dialog_silencer *dialog = new Dialog_silencer("矩形消音器",this);
    Silencer_atten* noi;
    const char *addButtonSlot = SLOT(onAddButtonRSilencerClicked());
    const char *delButtonSlot = SLOT(onDelButtonRSilencerClicked());

    if (dialog->exec() == QDialog::Accepted) {
       noi = static_cast<Silencer_atten*>(dialog->getNoi());
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
               "厂家"
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

void Widget::onDelButtonRSilencerClicked()
{
    deleteRowFromTable(ui->tableWidget_rect_silencer, 1);
}

void Widget::onAddButtonCSilencerEbClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_circular_silencerEb;
    Dialog_silencer *dialog = new Dialog_silencer("圆形消音弯头",this);
    Silencer_atten* noi;
    const char *addButtonSlot = SLOT(onAddButtonCSilencerEbClicked());
    const char *delButtonSlot = SLOT(onDelButtonCSilencerEbClicked());

    if (dialog->exec() == QDialog::Accepted) {
       noi = static_cast<Silencer_atten*>(dialog->getNoi());
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
               "厂家"
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

void Widget::onDelButtonCSilencerEbClicked()
{
    deleteRowFromTable(ui->tableWidget_circular_silencerEb, 1);
}

void Widget::onAddButtonRSilencerEbClicked()
{
    QTableWidget *tableWidget = ui->tableWidget_rect_silencerEb;
    Dialog_silencer *dialog = new Dialog_silencer("矩形消音弯头",this);
    Silencer_atten* noi;
    const char *addButtonSlot = SLOT(onAddButtonRSilencerEbClicked());
    const char *delButtonSlot = SLOT(onAddButtonRSilencerEbClicked());

    if (dialog->exec() == QDialog::Accepted) {
       noi = static_cast<Silencer_atten*>(dialog->getNoi());
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
               "厂家"
            };

            // 使用通用函数添加行
            addRowToTable(tableWidget, data, addButtonSlot, delButtonSlot);
        }
    }
}

void Widget::onDelButtonRSilencerEbClicked()
{
    deleteRowFromTable(ui->tableWidget_rect_silencerEb, 1);
}


void Widget::on_pushButton_circular_silencer_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_circular_silencer;
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
        noiseRevision<Silencer_atten, Dialog_silencer>(tableWidget, row, noi, items, cols, "圆形消音器");
    }
}

void Widget::on_pushButton_rect_silencer_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_rect_silencer;
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
        noiseRevision<Silencer_atten, Dialog_silencer>(tableWidget, row, noi, items, cols, "矩形消音器");
    }
}

void Widget::on_pushButton_circular_silencerEb_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_circular_silencerEb;
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
        noiseRevision<Silencer_atten, Dialog_silencer>(tableWidget, row, noi, items, cols, "圆形消音弯头");
    }
}

void Widget::on_pushButton_rect_silencerEb_revise_clicked()
{
    QTableWidget* tableWidget = ui->tableWidget_rect_silencerEb;
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
        noiseRevision<Silencer_atten, Dialog_silencer>(tableWidget, row, noi, items, cols, "矩形消音弯头");
    }
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

// 右键点击添加主竖区甲板
void Widget::TreeWidgetItemPressed_Slot(QTreeWidgetItem *item, int n)
{
    if (qApp->mouseButtons() == Qt::RightButton) // 1、首先判断是否为右键点击
    {
        if(item_zhufenguan.contains(item)) // 2、判断右击了哪个项目
        {
            mzfgMenu->exec(QCursor::pos());
        }
        if(item==item_room_define) //  点击了6.计算房间
        {
            if(mAddAct1==mAddMenu1->exec(QCursor::pos()))   // 弹出添加主竖区菜单
            {     
                Dialog_add_zhushuqu *box=new Dialog_add_zhushuqu;
                box->setlabeltext("主竖区");
                if(box->exec()==QDialog::Accepted)
                {
                    QTreeWidgetItem *treeitem=new QTreeWidgetItem(item_room_define,QStringList(box->getname()));
                    ui->treeWidget->addTopLevelItem(treeitem);
                    QTreeWidgetItem *treeitemx=new QTreeWidgetItem(item_room_calculate,QStringList(box->getname()));
                    ui->treeWidget->addTopLevelItem(treeitemx);
                    zsqmap.insert(treeitem,treeitemx);  // 保存第6项和第7项主竖区对应关系
                    item_zhushuqu.append(treeitem);
                    return;
                }
            }
        }
        if(item_zhushuqu.contains(item))            // 点击了甲板的item
        {
            if(mAddAct2==mAddMenu2->exec(QCursor::pos()))
            {
                Dialog_add_zhushuqu *box=new Dialog_add_zhushuqu;
                box->setlabeltext("甲板");
                if(box->exec()==QDialog::Accepted)
                {
                    QTreeWidgetItem *treeitemjb=new QTreeWidgetItem(item,QStringList(box->getname()));
                    ui->treeWidget->addTopLevelItem(treeitemjb);
                    item_jiaban.append(treeitemjb);
                    QTreeWidgetItem *treeitemcp=new QTreeWidgetItem(zsqmap.value(item),QStringList(box->getname()));
                    ui->treeWidget->addTopLevelItem(treeitemcp);

                    //  添加甲板的同时要添加一个页面
                    Form_room_define *form=new Form_room_define;
                    form->setjiabanItem(treeitemcp);
                    ui->stackedWidget->addWidget(form);
                    // 关联form发出的添加房间、删除房间信号
                    connect(form,SIGNAL(roomadd(QTreeWidgetItem*,QString,int)),
                            this,SLOT(upDateTreeItem7(QTreeWidgetItem*,QString,int)));
                    connect(form,SIGNAL(roomdel(QTreeWidgetItem*,QString)),
                            this,SLOT(delroom(QTreeWidgetItem*,QString)));

                    connect(ui->treeWidget,&QTreeWidget::itemClicked,this,[=](QTreeWidgetItem *item1,int n){
                        if(item1==treeitemjb)     //     点击甲板会显示添加的页面
                            ui->stackedWidget->setCurrentWidget(form);
                    });
                    return;
                }
            }
        }

    }
}
//  更新第七项房间底下的主风管信息 房间名，主风管数量
void Widget::upDateTreeItem7(QTreeWidgetItem *item,QString name,int num) //
{
    qDebug()<<num;
    QTreeWidgetItem *treeitemfj=new QTreeWidgetItem(item,QStringList(name));
    ui->treeWidget->addTopLevelItem(treeitemfj);
    for(int i=0;i<num;i++)
    {
        // 创建页面对象
        room_cal_baseWidget *page = new room_cal_baseWidget;

        // 将页面添加到堆栈窗口部件
        ui->stackedWidget->addWidget(page);

        // 这里的主风管还要插入对应page页面
        QTreeWidgetItem *treeitemfg=new QTreeWidgetItem(treeitemfj,QStringList("主风管"+QString::number(i+1)));
        ui->treeWidget->addTopLevelItem(treeitemfg);
        item_zhufenguan.append(treeitemfg);     // 保存主风管ID
        map_zsg_pag.insert(treeitemfg,page);

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

}

void Widget::delroom(QTreeWidgetItem* itemjb,QString roomid)
{
    for(int i=0;i<itemjb->childCount();i++)
    {
        qDebug()<<itemjb->child(i)->text(0);
        if(itemjb->child(i)->text(0)==roomid) // 删除房间
        {
            QTreeWidgetItem* itemfj=itemjb->child(i);
            for(int j=0;j<itemfj->childCount();j++) // 删除房间下的主风管
            {
                qDebug()<<itemfj->child(i)->text(0);
                ui->stackedWidget->removeWidget(map_zsg_pag.value(itemfj->child(j)));
                map_zsg_pag.remove(itemfj->child(j));      // 删除记录
                item_zhufenguan.removeOne(itemfj->child(j));
            }
            itemjb->removeChild(itemjb->child(i));
        }
    }
}

void Widget::initRightButtonMenu()
{
    mAddMenu1 = new QMenu(this);
    mAddMenu2 = new QMenu(this);
    mAddAct1 = new QAction("添加主竖区");
    mAddAct2 = new QAction("添加甲板");
    mAddMenu1->addAction(mAddAct1);
    mAddMenu2->addAction(mAddAct2);

    mzfgMenu = new QMenu(this);
    addzfgAct = new QAction("添加主风管");
    delzfgAct = new QAction("删除主风管");
    modnameAct = new QAction("修改名称");
    mzfgMenu->addAction(modnameAct);
    mzfgMenu->addAction(addzfgAct);
    mzfgMenu->addAction(delzfgAct);

    // 关联树Item的点击信号，但是在槽中判断是不是右键
    connect(ui->treeWidget, SIGNAL(itemPressed(QTreeWidgetItem*, int)),
            this, SLOT(TreeWidgetItemPressed_Slot(QTreeWidgetItem*, int)));
}

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
    if(current == item_prj)     //工程信息
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
    else if(current == item_return_air_box_grille)  //回风箱+格栅
    {
        ui->stackedWidget->setCurrentWidget(ui->page_return_air_box_grille);
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
    else if(current == item_return_air_box_grille_terminal_atten)     //抽送风头末端衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_return_air_box_grille_terminal_atten);
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
    else if(current == item_return_air_box_grille_relf_atten)     //回风箱＋格栅末端反射衰减
    {
        ui->stackedWidget->setCurrentWidget(ui->page_return_air_box_grille_terminal_refl);
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
}
/**************树列表************/


