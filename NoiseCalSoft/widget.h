#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QObject>
#include <QTreeWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QComboBox>
#include "globle_var.h"
#include <QMouseEvent>
#include <QAbstractItemView>
#include <QMenu>
#include "inputbasedialog.h"
#include "componentInpuTableWidget/widget_base_inputtable.h"
#include <QMouseEvent>
#include <QMessageBox>



QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void initializeTreeWidget();
    void initTableWidget(QTableWidget *tableWidget, const QStringList &headerText, const int *columnWidths, int colCount);
    void addWidgetToPage(QWidget* targetPage, Widget_base_inputTable* widgetToAdd);
    void setTable();
    double getNoiseLimitByName(const QString& name);
    void prj_TreeWidget();
    void input_TreeWidget();
    void initTableWidget_project_attachment();
    void initTableWidget_noi_limit();
    void initTableWidget_drawing_list();
    void initTableWidget_system_list();
    void initTableWidget_report_cal_room();
    void initRightButtonMenu();
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent* e);
    bool eventFilter(QObject* obj, QEvent* event);
    void setAttachmentTable();
    void setDrawingTable();
    void setNoiseLimit();
    void setPrjBasicInfo();

    /*  调用函数可创建树状列表，并生成对应页面
    **  item与page对应关系存储在map_system_systemlist_form等map中
    **  后续可以在这些容器中找到存在的page
    **
    **/
    QTreeWidgetItem* create_zsq(QString zsq_name);
    QTreeWidgetItem* create_system(QTreeWidgetItem* zsq_item,QString sys_name);

    QTreeWidgetItem* create_room(QTreeWidgetItem* sys_item,QString roomid);
    QVector<QTreeWidgetItem*> create_pipe(QTreeWidgetItem* room_item,QStringList pipename);

    QTreeWidgetItem* create_outer(QTreeWidgetItem* outersys_item,QString outerid);
    QVector<QTreeWidgetItem*> create_outer_pipe(QTreeWidgetItem* outer_item,QStringList outer_pipename);
    QTreeWidgetItem* create_classic_room(QString croom_name);

protected:
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
private:
    QPoint last;
    int canmove=0;

public slots:
    void TreeWidgetItemPressed_Slot(QTreeWidgetItem* item, int n);
    void upDateTreeItem8(QTreeWidgetItem *item,QString,int num, QString jiaban, QString limit);
    void delroom(QTreeWidgetItem*,QString);

private slots:
    void on_pushButto_prj_manage_clicked();

    void on_pushButton_input_clicked();

    void on_min_clicked();

    void on_max_clicked();

    void on_close_clicked();

    void on_pushButton_start_clicked();

    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_pushButton_prj_revise_clicked();

    void on_pushButton_prj_info_save_clicked();

    void on_tableWidget_noi_limit_itemChanged(QTableWidgetItem *item);

    void on_pushButton_noi_limit_add_clicked();

    void on_pushButton_noi_limit_del_clicked();

    void on_pushButton_cover_entry_clicked();

    void on_pushButton_cover_clear_clicked();

    void on_pushButton_dictionary_entry_clicked();

    void on_pushButton_dictionary_clear_clicked();

    void on_pushButton_project_overview_entry_clicked();

    void on_pushButton_project_overview_clear_clicked();

    void on_pushButton_reference_drawing_clear_clicked();

    void on_pushButton_reference_drawing_entry_clicked();

    void on_pushButton_noise_require_entry_clicked();

    void on_pushButton_noise_require_clear_clicked();

    void on_pushButton_choose_basis_entry_clicked();

    void on_pushButton_choose_basis_clear_clicked();

    void on_pushButton_noise_require_table_entry_clicked();

    void on_pushButton_drawing_list_add_clicked();

    void on_pushButton_drawing_list_del_clicked();

    void on_pushButton_project_attachment_add_clicked();

    void LoadExeclData_noisourse(QTableWidget *table,int spancolfirst,int spancollast);

    void SaveExeclData_noisourse(QTableWidget *table,QString component_name,int mergeCellfirst,int mergeCelllast);

    void LoadExeclData(QTableWidget *table);

    void SaveExeclData(QTableWidget *table,QString component_name);

    void clearTable();  //用于切换项目清理界面

    void setBasicPageWhenSwitchPrj();   //切换项目时调用

//    void on_pushButton_fanNoi_input_clicked();

//    void on_pushButton_fanNoi_output_clicked();

//    void on_pushButton_fanCoil_noi_input_clicked();

//    void on_pushButton_fanCoil_noi_output_clicked();

//    void on_pushButton_air_noi_single_fan_input_clicked();

//    void on_pushButton_air_noi_single_fan_output_clicked();

//    void on_pushButton_air_noi_double_fan_input_clicked();

//    void on_pushButton_air_noi_double_fan_output_clicked();

//    void on_pushButton_VAV_terminal_input_clicked();

//    void on_pushButton_VAV_terminal_output_clicked();

//    void on_pushButton_circular_damper_input_clicked();

//    void on_pushButton_circular_damper_output_clicked();

//    void on_pushButton_rect_damper_input_clicked();

//    void on_pushButton_rect_damper_output_clicked();

//    void on_pushButton_air_diff_input_clicked();

//    void on_pushButton_air_diff_output_clicked();

//    void on_pushButton_pump_send_input_clicked();

//    void on_pushButton_pump_send_output_clicked();

//    void on_pushButton_staticBox_grille_input_clicked();

//    void on_pushButton_staticBox_grille_output_clicked();

//    void on_pushButton_disp_vent_terminal_input_clicked();

//    void on_pushButton_disp_vent_terminal_output_clicked();

//    void on_pushButton_other_send_terminal_input_clicked();

//    void on_pushButton_other_send_terminal_output_clicked();

//    void on_pushButton_static_box_input_clicked();

//    void on_pushButton_static_box_output_clicked();

//    void on_pushButton_duct_with_multi_ranc_input_clicked();

//    void on_pushButton_duct_with_multi_ranc_output_clicked();

//    void on_pushButton_tee_input_clicked();

//    void on_pushButton_tee_output_clicked();

//    void on_pushButton_pipe_input_clicked();

//    void on_pushButton_pipe_output_clicked();

//    void on_pushButton_elbow_input_clicked();

//    void on_pushButton_elbow_output_clicked();

//    void on_pushButton_reducer_input_clicked();

//    void on_pushButton_reducer_output_clicked();

//    void on_pushButton_silencer_input_clicked();

//    void on_pushButton_silencer_output_clicked();

    void on_pushButton_drawing_list_save_clicked();

    void on_pushButton_noi_limit_save_clicked();

    void on_pushButton_project_attachment_del_clicked();

    void on_pushButton_noi_limit_revise_clicked();

    void on_pushButton_drawing_list_revise_clicked();

    void on_pushButton_database_clicked();

private:
    Ui::Widget *ui;
    QTreeWidgetItem *item_prj;   //工程
    QTreeWidgetItem *item_prj_info;   //工程信息
    QTreeWidgetItem *item_input;        //输入模块
    QTreeWidgetItem *item_cal;          //计算模块
    QTreeWidgetItem *item_output;       //输出模块
    QTreeWidgetItem *item_auth_manage;   //权限管理

    QMenu *menusystemlist;        // 系统清单的菜单 1.添加主竖区
    QAction *actAddzsq;
    QMenu *menuzsq;        // 主竖区的菜单 1.添加系统 2.修改主竖区名称 3.删除主竖区
    QAction *actAddsystem;
    QAction *actModzsqname;
    QAction *actDelzsq;
    QMenu *menusystem;      // 系统的菜单 1.修改系统名称 2.删除系统
    QAction *actModsystemname;
    QAction *actDelsystem;
    QMenu *menuzfg;        // 主风管的菜单 1.修改主风管名称 2..删除主风管
    QAction *actDelzfg;
    QAction *actModzfgname;
    QMenu *menucalroom;         // 房间的菜单 1.添加主竖区
    QAction *actAddzfg;
    QMenu *menuclassiccabin;      //典型住舱的菜单 1.添加典型房间
    QAction *actAddclassicroom;
    QMenu *menuclassicroom;       //典型房间的菜单 1.修改典型房间名称 2.删除典型房间
    QAction *actModclassicroomname;
    QAction *actDelclassicroom;

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

    QTreeWidgetItem *item_room_atten;                                   //5. 房间衰减
    QTreeWidgetItem *item_room_less425;                                   //5.1 体积小于425m³的房间（点噪声源）
    QTreeWidgetItem *item_room_more425;                                   //5.2 体积大于425m³的房间（点噪声源）
    QTreeWidgetItem *item_room_noFurniture;                                   //5.3 无家具房间（点噪声源）
    QTreeWidgetItem *item_room_open;                                   //5.4 室外开敞住所（点噪声源）
    QTreeWidgetItem *item_room_gap_tuyere;                                   //5.5 条缝风口房间（线噪声源）
    QTreeWidgetItem *item_room_rain;                                   //5.6 雨降风口房间（面噪声源）

    QTreeWidgetItem *item_system_list;                                 //6.系统清单
    QTreeWidgetItem *item_room_define;                                 //7.定义计算房间

    /**************容器*********************/
    QVector<QTreeWidgetItem *> vec_zsq;                        //保存主竖区item
    QVector<QTreeWidgetItem *> vec_system;                     //保存系统item
    QVector<QTreeWidgetItem *> vec_outersys;                     //保存室外item(指的是主竖区下个层级的室外)
    QVector<QTreeWidgetItem *> vec_room;                     //保存房间item
    QVector<QTreeWidgetItem *> vec_outer;                     //保存室外item

    QMap<QTreeWidgetItem *,QTreeWidgetItem *> map_zsq67;       // 第六项的主竖区和第七项的主竖区对应关系
    QMap<QTreeWidgetItem *,QTreeWidgetItem *> map_zsq68;       // 第六项的主竖区和第八项的主竖区对应关系
    QMap<QTreeWidgetItem *,QTreeWidgetItem *> map_system67;       // 第六项的系统和第七项的系统对应关系
    QMap<QTreeWidgetItem *,QTreeWidgetItem *> map_system68;       // 第六项的系统和第八项的系统对应关系
    QVector<QTreeWidgetItem *> vec_zfg;         // 保存主风管item,方便右键点击

    QMap<QString,QVector<QWidget *>> map_roomid_zfgpage;           //房间编号对应的主风管page 一对多关系
    QVector<QWidget *> vec_roomzfg;
    QVector<QTreeWidgetItem *> vec_classicroom;                     //保存典型房间item
    //2024.5.25
    QMap<QTreeWidgetItem *,QWidget *> map_system_systemlist_form;       //第6项，系统item->系统清单的page
    QMap<QTreeWidgetItem *,QWidget *> map_system_roomdefine_form;       //第7项，系统item->定义房间的page
    QMap<QTreeWidgetItem *,QWidget *> map_outersys_outerdefine_form;             //第7项，室外item->室外的page

    QMap<QTreeWidgetItem *,QWidget *> map_roompipe_roomcal_form;        //第8项，主风管item->主风管的page
    QMap<QTreeWidgetItem *,QWidget *> map_room_roomcaltotal_form;  //第8项，房间噪音item->房间噪音的page

    QMap<QTreeWidgetItem *,QWidget *> map_outerpipe_roomcal_form;        //第8项，支管item->支管的page
    QMap<QTreeWidgetItem *,QWidget *> map_outer_before_form;          //第8项，汇合前叠加的item->汇合前叠加的page
    QMap<QTreeWidgetItem *,QWidget *> map_outer_after_form;       //第8项，汇合后叠加的item->汇合后叠加的page
    QMap<QTreeWidgetItem *,QWidget *> map_outer_outercaltotal_form;      //第8项，室外噪音的item->室外噪音的page

    /**************容器*********************/

    QTreeWidgetItem *item_room_calculate;                             // 8.噪音计算
    QTreeWidgetItem *item_cabin_classic;        // 8.1典型住舱

    QTreeWidgetItem *item_report;        // 9.报表
    QTreeWidgetItem *item_report_cover;        // 9.1封面
    QTreeWidgetItem *item_report_dictionary;        // 9.2目录
    QTreeWidgetItem *item_report_overview;        // 9.3项目概述
    QTreeWidgetItem *item_report_noise_require;        // 9.4噪音要求
    QTreeWidgetItem *item_report_noise_require_basis;        // 9.4噪音要求
    QTreeWidgetItem *item_report_noise_require_table;        // 9.4噪音要求
    QTreeWidgetItem *item_report_system_list;        // 9.5系统清单
    QTreeWidgetItem *item_report_cal_room;        // 9.6计算房间
    QTreeWidgetItem *item_report_room_choose_basis;        // 9.6房间选择依据
    QTreeWidgetItem *item_report_cal_room_table;        // 9.6计算房间表格
    QTreeWidgetItem *item_report_cal_summarize;        // 9.7计算结果汇总
    QTreeWidgetItem *item_report_cal_detaile;        // 9.8舱室噪音详细计算
};
#endif // WIDGET_H
