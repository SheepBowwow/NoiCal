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
#include <QMouseEvent>
#include <QMessageBox>

struct Room{
    QString name;
    QString noise;
    QString type;
};

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
    void buttonToHeader(QTableWidget *tableWidget, QWidget *buttonWidget, const char *addButtonSlot, const char *delButtonSlot);
    void addRowToTable(QTableWidget *tableWidget, const QStringList &data, const char *addButtonSlot, const char *delButtonSlot);
    void deleteRowFromTable(QTableWidget *tableWidget, int deleteRowNum);
    void deleteRowFromTable(QTableWidget *tableWidget_noise, QTableWidget *tableWidget_atten, QTableWidget *tableWidget_refl);
    template <typename NoiType, typename DialogType>
    void noiseRevision(QTableWidget *tableWidget, int row,NoiType *noi, QVector<QString*>& items, int* cols, QString name = "");
    template <typename NoiType, typename DialogType>
    void noiseRevision(QTableWidget *currentTableWidget, QTableWidget *tableWidget_noise, QTableWidget *tableWidget_atten, QTableWidget *tableWidget_refl,
                       int row,NoiType *noi, QVector<QString*>& items_noise, QVector<QString*>& items_atten, QVector<QString*>& items_refl,
                       int* cols_noise, int* cols_atten, int* cols_refl, QString name = "");
    double getNoiseLimitByName(const QString& name);
    void prj_TreeWidget();
    void input_TreeWidget();
    void initTableWidget_noi_limit();
    void initTableWidget_fan_noi();
    void initTableWidget_fanCoil_noi();
    void initTableWidget_air_diff();
    void initTableWidget_pump_send_tuyere();
    void initTableWidget_pump_tuyere();
    void initTableWidget_send_tuyere();
    void initTableWidget_return_air_box_grille();
    void initTableWidget_VAV_terminal();
    void initTableWidget_disp_vent_terminal();
    void initTableWidget_static_box();
    void initTableWidget_other_send_terminal();
    void initTableWidget_silencer();
    void initTableWidget_circular_silencer();
    void initTableWidget_rect_silencer();
    void initTableWidget_circular_silencerEb();
    void initTableWidget_rect_silencerEb();
    void initTableWidegt_tee();
    void initTableWidegt_duct_with_multi_ranc();
    void initTableWidget_circular_damper();
    void initTableWidget_rect_damper();
    void initTableWidget_pipe();
    void initTableWidget_air_noi();
    void initTableWidget_reducer();
    void initTableWidget_elbow();
    void initRightButtonMenu();
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent* e);
    bool eventFilter(QObject* obj, QEvent* event);

protected:
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
private:
    QPoint last;
    int canmove=0;

public slots:
    void onAddButtonNoiLimitClicked();
    void onDelButtonNoiLimitClicked();

    void onAddButtonFanNoiClicked();
    void onDelButtonFanNoiClicked();

    void onAddButtonFanCoilNoiClicked();
    void onDelButtonFanCoilNoiClicked();

    void onAddButtonAirDiffNoiClicked();
    void onDelButtonAirDiffNoiClicked();

    void onAddButtonPumpClicked();
    void onDelButtonPumpClicked();

    void onAddButtonSendClicked();
    void onDelButtonSendClicked();

    void onAddButtonReturnAirBoxGriClicked();
    void onDelButtonReturnAirBoxGriClicked();

    void onAddButtonVAVTerminalClicked();
    void onDelButtonVAVTerminalClicked();

    void onAddButtonDispVentTerminalClicked();
    void onDelButtonDispVentTerminalClicked();

    void onAddButtonStaticBoxClicked();
    void onDelButtonStaticBoxClicked();

    void onAddButtonOtherSendTerClicked();
    void onDelButtonOtherSendTerClicked();

    void onAddButtonCSilencerClicked();
    void onDelButtonCSilencerClicked();

    void onAddButtonRSilencerClicked();
    void onDelButtonRSilencerClicked();

    void onAddButtonCSilencerEbClicked();
    void onDelButtonCSilencerEbClicked();

    void onAddButtonRSilencerEbClicked();
    void onDelButtonRSilencerEbClicked();

    void onAddButtonTeeClicked();
    void onDelButtonTeeClicked();

    void onAddButtonMultiRancClicked();
    void onDelButtonMultiRancClicked();

    void onAddButtonCirDamperClicked();
    void onDelButtonCirDamperClicked();

    void onAddButtonRectDamperClicked();
    void onDelButtonRectDamperClicked();

    void onAddButtonPipeClicked();
    void onDelButtonPipeClicked();

    void onAddButtonAirNoiClicked();
    void onDelButtonAirNoiClicked();

    void onAddButtonReducerClicked();
    void onDelButtonReducerClicked();

    void onAddButtonElbowClicked();
    void onDelButtonElbowClicked();
    
    void TreeWidgetItemPressed_Slot(QTreeWidgetItem* item, int n);
    void upDateTreeItem7(QTreeWidgetItem *item,QString,int num);
private slots:
    void on_pushButto_prj_manage_clicked();

    void on_pushButton_input_clicked();

    void on_pushButton_fanNoi_revise_clicked();

    void on_min_clicked();

    void on_max_clicked();

    void on_close_clicked();

    void on_pushButton_start_clicked();

    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_pushButton_fanCoil_noi_revise_clicked();

    void on_pushButton_air_diff_revise_clicked();

    void on_pushButton_pump_tuyere_revise_clicked();

    void on_pushButton_send_tuyere_revise_clicked();

    void on_pushButton_return_air_box_grille_revise_clicked();

    void on_pushButton_VAV_terminal_revise_clicked();

    void on_pushButton_disp_vent_terminal_revise_clicked();

    void on_pushButton_static_box_revise_clicked();

    void on_pushButton_other_send_terminal_revise_clicked();

    void on_pushButton_circular_silencer_revise_clicked();

    void on_pushButton_rect_silencer_revise_clicked();

    void on_pushButton_circular_silencerEb_revise_clicked();

    void on_pushButton_rect_silencerEb_revise_clicked();

    void on_pushButton_tee_revise_clicked();

    void on_pushButton_duct_with_multi_ranc_revise_clicked();

    void on_pushButton_circular_damper_revise_clicked();

    void on_pushButton_rect_damper_revise_clicked();

    void on_pushButton_pipe_revise_clicked();

    void on_pushButton_air_noi_revise_clicked();

    void on_pushButton_reducer_revise_clicked();

    void on_pushButton_elbow_revise_clicked();

    void on_pushButton_pump_table_clicked();

    void on_pushButton_send_table_clicked();

    void on_tableWidget_pump_tuyere_itemDoubleClicked(QTableWidgetItem *item);

    void on_tableWidget_send_tuyere_itemDoubleClicked(QTableWidgetItem *item);

    void on_pushButton_circular_silencer_table_clicked();

    void on_pushButton_rect_silencer_table_clicked();

    void on_pushButton_circular_silencerEb_table_clicked();

    void on_pushButton_rect_silencerEb_table_clicked();

    void on_pushButton_prj_revise_clicked();

    void on_pushButton_prj_save_clicked();

    void on_tableWidget_noi_limit_itemChanged(QTableWidgetItem *item);

    void on_pushButton_air_diff_terminal_atten_revise_clicked();

    void on_pushButton_air_diff_terminal_refl_revise_clicked();

    void on_pushButton_pump_tuyere_terminal_atten_revise_clicked();

    void on_pushButton_send_tuyere_terminal_atten_revise_clicked();

    void on_pushButton_pump_tuyere_terminal_refl_revise_clicked();

    void on_pushButton_send_tuyere_terminal_refl_revise_clicked();

    void on_pushButton_pump_terminal_atten_table_clicked();

    void on_pushButton_send_terminal_atten_table_clicked();

    void on_pushButton_send_terminal_refl_table_clicked();

    void on_pushButton_pump_terminal_refl_table_clicked();

    void on_pushButton_return_air_box_grille_terminal_atten_revise_clicked();

    void on_pushButton_return_air_box_grille_terminal_refl_revise_clicked();

    void on_pushButton_disp_vent_terminal_atten_revise_clicked();

    void on_pushButton_disp_vent_terminal_refl_revise_clicked();

    void on_pushButton_other_send_terminal_atten_revise_clicked();

    void on_pushButton_other_send_terminal_refl_revise_clicked();

    void on_pushButton_fanNoi_del_clicked();

    void on_pushButton_fanNoi_add_clicked();

private:
    Ui::Widget *ui;
    QTreeWidgetItem *item_prj;   //工程
    QTreeWidgetItem *item_prj_info;   //工程信息
    QTreeWidgetItem *item_input;        //输入模块
    QTreeWidgetItem *item_cal;          //计算模块
    QTreeWidgetItem *item_output;       //输出模块
    QTreeWidgetItem *item_auth_manage;   //权限管理
    QMenu *mAddMenu1;        // 右键点击treeitem弹窗
    QMenu *mAddMenu2;
    QAction *mAddAct1;      // 主竖区
    QAction *mAddAct2;      // 甲板

    QVector<Room> rooms;

    //输入模块
    QTreeWidgetItem *item_sound_sorce_noise;   //1音源噪音
    QTreeWidgetItem *item_fan_noise;   //1.1风机噪音 1                                                  1
    QTreeWidgetItem *item_fan_coil_noise;   //1.2风机盘管噪音                                             1
    QTreeWidgetItem *item_aircondition_noise;   //1.3空调器噪音
    QTreeWidgetItem *item_pipe_and_acce_airflow_noise;            //2.管路及附件气流噪音
    QTreeWidgetItem *item_valve_noise;                                  //2.1 阀门噪音
    QTreeWidgetItem *item_VAV_terminal;                                  //2.1.1 变风量末端              1
    QTreeWidgetItem *item_circular_damper;                                  //2.1.2 圆形调风门
    QTreeWidgetItem *item_rect_damper;                                      //2.1.3 方形调风门
    QTreeWidgetItem *item_terminal_airflow_noise;                           //2.2 末端气流噪声
    QTreeWidgetItem *item_air_diff;                                         //2.2.1 布风器+散流器     1
    QTreeWidgetItem *item_air;                                         //2.2.1 布风器
    QTreeWidgetItem *item_diff;                                         //2.2.1 散流器
    QTreeWidgetItem *item_pump_send_tuyere;                         //2.2.2 抽/送风头                   1
    QTreeWidgetItem *item_return_air_box_grille;                            //2.2.3 回风箱+格栅          1
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
    QTreeWidgetItem *item_return_air_box_grille_terminal_atten;
    QTreeWidgetItem *item_disp_vent_terminal_atten;
    QTreeWidgetItem *item_other_send_terminal_atten;

    QTreeWidgetItem *item_terminal_refl_atten;                              //3.6 末端反射衰减
    QTreeWidgetItem *item_air_diff_relf_atten;
    QTreeWidgetItem *item_air_relf_atten;
    QTreeWidgetItem *item_diff_relf_atten;
    QTreeWidgetItem *item_pump_send_tuyere_relf_atten;
    QTreeWidgetItem *item_return_air_box_grille_relf_atten;
    QTreeWidgetItem *item_disp_vent_relf_atten;
    QTreeWidgetItem *item_other_send_relf_atten;

    QTreeWidgetItem *item_room_atten;                                   //5. 房间衰减
    QTreeWidgetItem *item_room_less425;                                   //5.1 体积小于425m³的房间（点噪声源）
    QTreeWidgetItem *item_room_more425;                                   //5.2 体积大于425m³的房间（点噪声源）
    QTreeWidgetItem *item_room_noFurniture;                                   //5.3 无家具房间（点噪声源）
    QTreeWidgetItem *item_room_open;                                   //5.4 室外开敞住所（点噪声源）
    QTreeWidgetItem *item_room_gap_tuyere;                                   //5.5 条缝风口房间（线噪声源）
    QTreeWidgetItem *item_room_rain;                                   //5.6 雨降风口房间（面噪声源）

    QTreeWidgetItem *item_room_define;
    QVector<QTreeWidgetItem *> item_zhushuqu;
    QVector<QTreeWidgetItem *> item_jiaban;
    QMap<QTreeWidgetItem *,QTreeWidgetItem *> zsqmap;       // 第六项的主竖区和第七项的主竖区的一一对应
    QTreeWidgetItem *item_room_calculate;
};
#endif // WIDGET_H
