#include "componentsdb.h"
#include "ui_componentsdb.h"
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
#include "componentInpuTableWidget/widget_tee_inputtable.h"
#include "componentInpuTableWidget/widget_pipe_inputtable.h"
#include "componentInpuTableWidget/widget_elbow_inputtable.h"
#include "componentInpuTableWidget/widget_reducer_inputtable.h"
#include "componentInpuTableWidget/widget_silencer_inputtable.h"

ComponentsDB::ComponentsDB(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComponentsDB)
{
    ui->setupUi(this);
    initTree();
    setTable();

    this->setWindowFlags(Qt::Dialog);
}

ComponentsDB::~ComponentsDB()
{
    delete ui;
}

void ComponentsDB::initTree()
{
    item_sound_sorce_noise = new QTreeWidgetItem(QStringList("1.噪声源"));   //1音源噪音
    item_fan_noise = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("风机"));   //1.1风机噪音
    item_fan_coil_noise = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("风机盘管"));   //1.2风机盘管噪音
    item_aircondition_noise_single_fan = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("空调器(单风机)"));   //1.3空调器噪音(单风机)
    item_aircondition_noise_double_fan = new QTreeWidgetItem(item_sound_sorce_noise,QStringList("空调器(双风机)"));   //1.3空调器噪音(双风机)

    item_airflow_noise = new QTreeWidgetItem(QStringList("2.气流噪音"));            //2.管路及附件气流噪音
    item_valve_noise = new QTreeWidgetItem(item_airflow_noise,QStringList("阀门"));                                  //2.1 阀门噪音
    item_VAV_terminal = new QTreeWidgetItem(item_valve_noise,QStringList("变风量末端"));                                  //2.1.1 变风量末端
    item_circular_damper = new QTreeWidgetItem(item_valve_noise,QStringList("圆形调风门"));                                  //2.1.2 圆形调风门
    item_rect_damper = new QTreeWidgetItem(item_valve_noise,QStringList("方形调风门"));                                      //2.1.3 方形调风门
    item_atten_and_airflow_noise = new QTreeWidgetItem(QStringList("3.噪音衰减+气流噪音"));                           //2.2 末端气流噪声
    item_air_diff = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("布风器+散流器"));                                         //2.2.1 布风器+散流器
    item_pump = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("抽风头"));                         //2.2.2 抽/送风头
    item_send = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("送风头"));                         //2.2.2 抽/送风头
    item_staticBox_grille = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("静压箱+格栅"));                            //2.2.3 回风箱+格栅
    item_disp_vent_terminal = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("置换通风末端"));                //2.2.4 置换通风末端
    item_other_send_terminal = new QTreeWidgetItem(item_atten_and_airflow_noise,QStringList("其他送风末端"));                //2.2.5 静压箱孔板送风

    item_noise_atten = new QTreeWidgetItem(QStringList("4.噪音衰减"));                //3. 管路及附件噪音衰减
    item_branch_atten = new QTreeWidgetItem(item_noise_atten,QStringList("分支"));                             //3.1 分支衰减
    item_static_box = new QTreeWidgetItem(item_branch_atten,QStringList("静压箱"));                               //3.1.1 静压箱
    item_duct_with_multi_ranc = new QTreeWidgetItem(item_branch_atten,QStringList("风道多分支"));                     //3.1.2 风道多分支
    item_tee_atten = new QTreeWidgetItem(item_branch_atten,QStringList("三通"));                                //3.1.3 三通衰减
    item_pipe_atten = new QTreeWidgetItem(item_noise_atten,QStringList("直管"));                                //3.2 直管衰减
    item_elbow_atten = new QTreeWidgetItem(item_noise_atten,QStringList("弯头"));                                //3.3 弯头衰减
    item_reducer_atten = new QTreeWidgetItem(item_noise_atten,QStringList("变径"));                                //3.4 变径衰减
    item_circular_silencer_atten = new QTreeWidgetItem(item_noise_atten,QStringList("圆形消音器"));                                //3.5 消音器衰减
    item_rect_silencer_atten = new QTreeWidgetItem(item_noise_atten,QStringList("方形消音器"));                                //3.5 消音器衰减
    item_circular_silencer_elbow_atten = new QTreeWidgetItem(item_noise_atten,QStringList("圆形消音弯头"));                                //3.5 消音器衰减
    item_rect_silencer_elbow_atten = new QTreeWidgetItem(item_noise_atten,QStringList("方形消音弯头"));                                //3.5 消音器衰减

    ui->treeWidget->addTopLevelItem(item_sound_sorce_noise);
    ui->treeWidget->addTopLevelItem(item_airflow_noise);
    ui->treeWidget->addTopLevelItem(item_atten_and_airflow_noise);
    ui->treeWidget->addTopLevelItem(item_noise_atten);
}

void ComponentsDB::addWidgetToPage(QWidget *targetPage, Widget_base_inputTable *widgetToAdd)
{
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

void ComponentsDB::setTable()
{
    addWidgetToPage(ui->page_fan, new Widget_fan_inputTable());
    addWidgetToPage(ui->page_fanCoil, new Widget_fanCoil_inputTable());
    addWidgetToPage(ui->page_air_noi_single_fan, new Widget_aircondition_singleFan_inputTable());
    addWidgetToPage(ui->page_air_noi_double_fan, new Widget_aircondition_doubleFan_inputTable());
    addWidgetToPage(ui->page_VAV_terminal, new Widget_VAV_terminal_inputTable());
    addWidgetToPage(ui->page_circular_damper, new Widget_Circular_damper_inputTable());
    addWidgetToPage(ui->page_rect_damper, new Widget_Rect_damper_inputTable());
    addWidgetToPage(ui->page_static_box, new Widget_static_box_inputTable());
    addWidgetToPage(ui->page_duct_with_multi_ranc, new Widget_Multi_ranc_inputTable());
    addWidgetToPage(ui->page_tee, new Widget_Tee_inputTable());
    addWidgetToPage(ui->page_pipe, new Widget_Pipe_inputTable());
    addWidgetToPage(ui->page_elbow, new Widget_Elbow_inputTable());
    addWidgetToPage(ui->page_reducer, new Widget_Reducer_inputTable());
    addWidgetToPage(ui->page_silencer, new Widget_Silencer_inputTable());

    Widget_air_diff_inputTable* widget_air_diff_inputTable = new Widget_air_diff_inputTable();
    addWidgetToPage(ui->page_air_diff, widget_air_diff_inputTable);

    Widget_Pump_Send_inputTable* widget_pump_send_inputTable = new Widget_Pump_Send_inputTable();
    addWidgetToPage(ui->page_pump_send, widget_pump_send_inputTable);

    Widget_static_box_grille_inputTable* widget_static_box_grille_inputTable = new Widget_static_box_grille_inputTable();
    addWidgetToPage(ui->page_staticBox_grille, widget_static_box_grille_inputTable);

    Widget_Disp_vent_terminal_inputTable* widget_disp_vent_terminal_inputTable = new Widget_Disp_vent_terminal_inputTable();
    addWidgetToPage(ui->page_disp_vent_terminal, widget_disp_vent_terminal_inputTable);

    Widget_Other_send_terminal_inputTable* widget_other_send_terminal_inputTable = new Widget_Other_send_terminal_inputTable();
    addWidgetToPage(ui->page_other_send_terminal, widget_other_send_terminal_inputTable);
}

void ComponentsDB::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
//    if(current == item_fan_noise)      //风机噪音
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_fan);
//    }
//    else if(current == item_fan_coil_noise) //风机盘管噪音
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_fanCoil);
//    }
//    else if(current == item_aircondition_noise_single_fan) //风机盘管噪音
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_air_noi_single_fan);
//    }
//    else if(current == item_aircondition_noise_double_fan) //风机盘管噪音
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_air_noi_double_fan);
//    }
//    else if(current == item_VAV_terminal)   //变风量末端噪音
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_VAV_terminal);
//    }
//    else if(current == item_circular_damper)   //圆形调风门
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_circular_damper);
//    }
//    else if(current == item_rect_damper)   //方形调风门
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_rect_damper);
//    }
//    else if(current == item_air_diff)       //布风器+散流器
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_air_diff);
//        ui->stackedWidget->currentWidget()->findChild<Widget_air_diff_inputTable*>()->setCurrentTable(0);
//    }
//    else if(current == item_pump)   //抽/送风头
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_pump_send);
//        ui->stackedWidget->currentWidget()->findChild<Widget_Pump_Send_inputTable*>()->setCurrentTable(0);
//    }
//    else if(current == item_staticBox_grille)  //回风箱+格栅
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_staticBox_grille);
//        ui->stackedWidget->currentWidget()->findChild<Widget_static_box_grille_inputTable*>()->setCurrentTable(0);
//    }
//    else if(current == item_disp_vent_terminal)     //置换通风末端
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_disp_vent_terminal);
//        ui->stackedWidget->currentWidget()->findChild<Widget_Disp_vent_terminal_inputTable*>()->setCurrentTable(0);
//    }
//    else if (current == item_other_send_terminal)       //静压箱孔板送风
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_other_send_terminal);
//        ui->stackedWidget->currentWidget()->findChild<Widget_Other_send_terminal_inputTable*>()->setCurrentTable(0);
//    }
//    else if(current == item_static_box)         //静压箱
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_static_box);
//    }
//    else if(current == item_duct_with_multi_ranc)         //风道多分支
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_duct_with_multi_ranc);
//    }
//    else if(current == item_tee_atten)         //三通
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_tee);
//    }
//    else if(current == item_pipe_atten)         //直管
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_pipe);
//    }
//    else if(current == item_elbow_atten)         //直管
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_elbow);
//    }
//    else if(current == item_reducer_atten)         //变径
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_reducer);
//    }
//    else if(current == item_circular_silencer_atten)     //消音器
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_silencer);
//    }
//    else if(current == item_air_diff_terminal_atten)     //布风器散流器末端衰减
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_air_diff);
//        ui->stackedWidget->currentWidget()->findChild<Widget_air_diff_inputTable*>()->setCurrentTable(1);
//    }
//    else if(current == item_pump_terminal_atten)     //抽送风头末端衰减
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_pump_send);
//        ui->stackedWidget->currentWidget()->findChild<Widget_Pump_Send_inputTable*>()->setCurrentTable(1);
//    }
//    else if(current == item_staticBox_grille_terminal_atten)     //抽送风头末端衰减
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_staticBox_grille);
//        ui->stackedWidget->currentWidget()->findChild<Widget_static_box_grille_inputTable*>()->setCurrentTable(1);
//    }
//    else if(current == item_disp_vent_terminal_atten)     //置换通风末端衰减
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_disp_vent_terminal);
//        ui->stackedWidget->currentWidget()->findChild<Widget_Disp_vent_terminal_inputTable*>()->setCurrentTable(1);
//    }
//    else if(current == item_other_send_terminal_atten)     //其他送风末端衰减
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_other_send_terminal);
//        ui->stackedWidget->currentWidget()->findChild<Widget_Other_send_terminal_inputTable*>()->setCurrentTable(1);
//    }
//    else if(current == item_air_diff_relf_atten)     //末端布风器散流器反射衰减
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_air_diff);
//        ui->stackedWidget->currentWidget()->findChild<Widget_air_diff_inputTable*>()->setCurrentTable(2);
//    }
//    else if(current == item_pump_relf_atten)     //抽送风头末端反射衰减
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_pump_send);
//        ui->stackedWidget->currentWidget()->findChild<Widget_Pump_Send_inputTable*>()->setCurrentTable(2);
//    }
//    else if(current == item_staticBox_grille_relf_atten)     //回风箱＋格栅末端反射衰减
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_staticBox_grille);
//        ui->stackedWidget->currentWidget()->findChild<Widget_static_box_grille_inputTable*>()->setCurrentTable(2);
//    }
//    else if(current == item_disp_vent_relf_atten)     //置换通风末端反射衰减
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_disp_vent_terminal);
//        ui->stackedWidget->currentWidget()->findChild<Widget_Disp_vent_terminal_inputTable*>()->setCurrentTable(2);
//    }
//    else if(current == item_other_send_relf_atten)     //其他送风末端反射衰减
//    {
//        ui->stackedWidget->setCurrentWidget(ui->page_other_send_terminal);
//        ui->stackedWidget->currentWidget()->findChild<Widget_Other_send_terminal_inputTable*>()->setCurrentTable(2);
//    }

}

