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
    QTreeWidgetItem *item_pipe_and_acce_airflow_noise;            //2.管路及附件气流噪音
    QTreeWidgetItem *item_valve_noise;                                  //2.1 阀门噪音
    QTreeWidgetItem *item_VAV_terminal;                                  //2.1.1 变风量末端
    QTreeWidgetItem *item_circular_damper;                                  //2.1.2 圆形调风门
    QTreeWidgetItem *item_rect_damper;                                      //2.1.3 方形调风门
    QTreeWidgetItem *item_terminal_airflow_noise;                           //2.2 末端气流噪声
    QTreeWidgetItem *item_air_diff;                                         //2.2.1 布风器+散流器
    QTreeWidgetItem *item_air;                                         //2.2.1 布风器
    QTreeWidgetItem *item_diff;                                         //2.2.1 散流器
    QTreeWidgetItem *item_pump_send_tuyere;                         //2.2.2 抽/送风头
    QTreeWidgetItem *item_staticBox_grille;                            //2.2.3 回风箱+格栅
    QTreeWidgetItem *item_disp_vent_terminal;                //2.2.4 置换通风末端
    QTreeWidgetItem *item_other_send_terminal;                //2.2.5 其他送风末端
    QTreeWidgetItem *item_noise_atten_in_pipe_acce;                //3. 管路及附件噪音衰减
    QTreeWidgetItem *item_branch_atten;                             //3.1 分支衰减
    QTreeWidgetItem *item_static_box;                               //3.1.1 静压箱
    QTreeWidgetItem *item_duct_with_multi_ranc;                     //3.1.2 风道多分支
    QTreeWidgetItem *item_tee_atten;                                //3.1.3 三通衰减
    QTreeWidgetItem *item_pipe_atten;                                //3.2 直管衰减
    QTreeWidgetItem *item_elbow_atten;                                //3.3 弯头衰减
    QTreeWidgetItem *item_reducer_atten;                                //3.4 变径衰减
    QTreeWidgetItem *item_silencer_atten;                                //3.5 消音器衰减

    QTreeWidgetItem *item_terminal_atten;                                //3.6 末端衰减
    QTreeWidgetItem *item_air_diff_terminal_atten;
    QTreeWidgetItem *item_air_terminal_atten;
    QTreeWidgetItem *item_diff_terminal_atten;
    QTreeWidgetItem *item_pump_send_tuyere_terminal_atten;
    QTreeWidgetItem *item_staticBox_grille_terminal_atten;
    QTreeWidgetItem *item_disp_vent_terminal_atten;
    QTreeWidgetItem *item_other_send_terminal_atten;

    QTreeWidgetItem *item_terminal_refl_atten;                              //3.6 末端反射衰减
    QTreeWidgetItem *item_air_diff_relf_atten;
    QTreeWidgetItem *item_air_relf_atten;
    QTreeWidgetItem *item_diff_relf_atten;
    QTreeWidgetItem *item_pump_send_tuyere_relf_atten;
    QTreeWidgetItem *item_staticBox_grille_relf_atten;
    QTreeWidgetItem *item_disp_vent_relf_atten;
    QTreeWidgetItem *item_other_send_relf_atten;
};

#endif // COMPONENTSDB_H
