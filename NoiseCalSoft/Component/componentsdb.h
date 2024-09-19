#ifndef COMPONENTSDB_H
#define COMPONENTSDB_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "componentInpuTableWidget/widget_base_inputtable.h"

namespace Ui {
class ComponentsDB;
}

class ComponentsDB : public QWidget
{
    Q_OBJECT

public:
    explicit ComponentsDB(QWidget *parent = nullptr);
    ~ComponentsDB();
    void initTree();
    void addWidgetToPage(QWidget* targetPage, Widget_base_inputTable* widgetToAdd);
    void setTable();

private slots:
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    Ui::ComponentsDB *ui;

private:
    QTreeWidgetItem *item_sound_sorce_noise;   //1音源噪音
    QTreeWidgetItem *item_fan_noise;   //1.1风机噪音 1
    QTreeWidgetItem *item_fan_coil_noise;   //1.2风机盘管噪音
    QTreeWidgetItem *item_aircondition_noise_single_fan;   //1.3空调器噪音(单风机)
    QTreeWidgetItem *item_aircondition_noise_double_fan;   //1.3空调器噪音(双风机)
    QTreeWidgetItem *item_airflow_noise;            //2.气流噪音
    QTreeWidgetItem *item_valve_noise;                                  //2.1 阀门噪音
    QTreeWidgetItem *item_VAV_terminal;                                  //2.1.1 变风量末端
    QTreeWidgetItem *item_circular_damper;                                  //2.1.2 圆形调风门
    QTreeWidgetItem *item_rect_damper;                                      //2.1.3 方形调风门
    QTreeWidgetItem *item_atten_and_airflow_noise;                           //3. 噪音衰减+气流噪音
    QTreeWidgetItem *item_air_diff;                                         //3.1 布风器+散流器
    QTreeWidgetItem *item_pump;                         //3.2 抽风头
    QTreeWidgetItem *item_send;                         //3.3 送风头
    QTreeWidgetItem *item_staticBox_grille;                            //3.4 回风箱+格栅
    QTreeWidgetItem *item_disp_vent_terminal;                //3.5 置换通风末端
    QTreeWidgetItem *item_other_send_terminal;                //3.6 其他送风末端
    QTreeWidgetItem *item_noise_atten;                      //4.噪音衰减
    QTreeWidgetItem *item_branch_atten;                             //4.1 分支衰减
    QTreeWidgetItem *item_static_box;                               //4.1.1 静压箱
    QTreeWidgetItem *item_duct_with_multi_ranc;                     //4.1.2 风道多分支
    QTreeWidgetItem *item_tee_atten;                                //4.1.3 三通衰减
    QTreeWidgetItem *item_pipe_atten;                                //4.2 直管衰减
    QTreeWidgetItem *item_elbow_atten;                                //4.3 弯头衰减
    QTreeWidgetItem *item_reducer_atten;                                //4.4 变径衰减
    QTreeWidgetItem *item_circular_silencer_atten;                                //4.5 圆形消音器衰减
    QTreeWidgetItem *item_rect_silencer_atten;                                //4.6 方形消音器衰减
    QTreeWidgetItem *item_circular_silencer_elbow_atten;                                //4.7 圆形消音弯头
    QTreeWidgetItem *item_rect_silencer_elbow_atten;                                //4.8 方形消音弯头
};

#endif // COMPONENTSDB_H
