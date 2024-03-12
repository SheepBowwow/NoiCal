#include "roomcaltable.h"
#include "ui_roomcaltable.h"
#include <QContextMenuEvent>
#include "globle_var.h"
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout> // 添加此行
#include <QDebug>
#include <cmath>

RoomCalTable::RoomCalTable(QString systemName, QWidget *parent,QString currentTableType) :
    QWidget(parent),
    systemName(systemName),
    isCollapsed(false),
    ui(new Ui::RoomCalTable)
{
    ui->setupUi(this);
    //connect(ui->comboBox_noi_src_num, &ClickableComboBox::clicked, this, &RoomCalTable::updateComboBoxItems);

    this->isValid = true;
    this->currentTableType = currentTableType;

    variations.reserve(8);
    for (int i = 0; i < 8; ++i) {
        variations.push_back(QString());
    }

    if(currentTableType == "" || currentTableType == "声源噪音")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_unit);
        ui->comboBox_sound_type->setCurrentText("声源噪音");
        ui->comboBox_unit_name->setCurrentIndex(0);
        ui->stackedWidget_table->setCurrentWidget(ui->page_fan);
        this->currentTableType = "声源噪音";
    }
    else if (currentTableType == "气流噪音")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_unit);
        ui->comboBox_sound_type->setCurrentText("气流噪音");
        ui->comboBox_unit_name->setCurrentIndex(0);
        ui->stackedWidget_table->setCurrentWidget(ui->page_VAV_terminal);
    }
    else if(currentTableType == "噪音衰减+气流噪音")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_room_cal);
        ui->comboBox_sound_type->setCurrentText("噪音衰减+气流噪音");
        ui->comboBox_unit_name->setCurrentIndex(0);
        ui->stackedWidget_table->setCurrentWidget(ui->page_air_diff);
    }
    else if(currentTableType == "噪音衰减")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_room_cal);
        ui->comboBox_sound_type->setCurrentText("噪音衰减");
        ui->comboBox_unit_name->setCurrentIndex(0);
        ui->stackedWidget_table->setCurrentWidget(ui->page_static_box);
    }
    else if(currentTableType == "声压级计算")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_room_cal);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
        ui->comboBox_room_type->setCurrentIndex(0);
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_less425);
    }
    else
    {
        this->isValid = false;
    }

    // 清除最小和最大大小限制
    this->setMinimumSize(QSize(0, 0));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    // 获取当前显示的页面的部件
    QWidget *currentWidget = ui->stackedWidget_table->currentWidget();

    if (currentWidget) {
            int tableHeight = currentWidget->height();
            int titleHeight = ui->widget_title->height();

            // 设置窗口的高度
            this->setMinimumSize(QSize(this->width(), tableHeight + titleHeight));
            this->setMaximumSize(QSize(this->width(), tableHeight + titleHeight));
    }


    auto qComboBoxGroup= this->findChildren<QComboBox*>();
    for(auto each:qComboBoxGroup)
    {
       each->installEventFilter(this);
    }
    if(ui->stackedWidget_title->currentWidget() == ui->page_unit)
        this->currentUnitType = ui->comboBox_unit_name->currentText();
    else
        this->currentUnitType = ui->comboBox_room_type->currentText();
}

int RoomCalTable::getIndexInLayout() const
{
    if (parentWidget()) {
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(parentWidget()->layout()); // 修改此行
        if (layout) {
            int count = layout->count();
            for (int i = 0; i < count; ++i) {
                if (layout->itemAt(i)->widget() == this) {
                    return i;
                }
            }
        }
    }
    return -1; // Not found
}

void RoomCalTable::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction *addBeforeAction = menu.addAction("在前面添加");
    QAction *addAfterAction = menu.addAction("在后面添加");
    QAction *deleteAction = menu.addAction("删除");

    QAction *selectedAction = menu.exec(event->globalPos());

    int indexInLayout = getIndexInLayout();
    if (selectedAction == addBeforeAction) {
        emit addBeforeClicked(indexInLayout);
    } else if (selectedAction == addAfterAction) {
        emit addAfterClicked(indexInLayout);
    } else if (selectedAction == deleteAction) {
        emit deleteClicked(indexInLayout);
    }
}

void RoomCalTable::on_stackedWidget_table_currentChanged(int arg1)
{
    Q_UNUSED(arg1);

    // 清除最小和最大大小限制
    this->setMinimumSize(QSize(0, 0));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    // 获取当前显示的页面的部件
    QWidget *currentWidget = ui->stackedWidget_table->currentWidget();

    if (currentWidget) {
            int tableHeight = currentWidget->height();
            int titleHeight = ui->widget_title->height();

            // 设置窗口的高度
            this->setMinimumSize(QSize(this->width(), tableHeight + titleHeight));
            this->setMaximumSize(QSize(this->width(), tableHeight + titleHeight));
    }
}

RoomCalTable::~RoomCalTable()
{
    delete ui;
}

void RoomCalTable::clearPage(QWidget *widget)
{
    // 递归查找 QLineEdit 和 QComboBox
    for (QObject *child : widget->children()) {
        if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(child)) {
            // 找到 QLineEdit，清空文本
            lineEdit->clear();
        } else if (QComboBox *comboBox = qobject_cast<QComboBox*>(child)) {
            // 找到 QComboBox，清空选择
            comboBox->setCurrentIndex(-1); // 或者使用 comboBox->clear();
        } else if (QWidget *childWidget = qobject_cast<QWidget*>(child)) {
            // 递归查找子部件
            clearPage(childWidget);
        }
    }
}


void RoomCalTable::on_comboBox_sound_type_currentTextChanged(const QString &arg1)
{
    ui->stackedWidget_title->setCurrentWidget(ui->page_unit);
    if(arg1 == "声源噪音")
    {
        ui->comboBox_unit_name->clear();
        ui->comboBox_unit_name->addItem("风机");
        ui->comboBox_unit_name->addItem("风机盘管");
        ui->comboBox_unit_name->addItem("空调器");
    }
    else if(arg1 == "气流噪音")
    {
        ui->comboBox_unit_name->clear();
        ui->comboBox_unit_name->addItem("阀门/变风量末端");
        ui->comboBox_unit_name->addItem("阀门/圆形调风门");
        ui->comboBox_unit_name->addItem("阀门/方形调风门");
    }
    else if(arg1 == "噪音衰减+气流噪音")
    {
        ui->comboBox_unit_name->clear();
        ui->comboBox_unit_name->addItem("末端/布风器+散流器");
        ui->comboBox_unit_name->addItem("末端/抽风头");
        ui->comboBox_unit_name->addItem("末端/送风头");
        ui->comboBox_unit_name->addItem("末端/静压箱+格栅");
        ui->comboBox_unit_name->addItem("末端/置换通风末端");
        ui->comboBox_unit_name->addItem("末端/其他通风末端");
    }
    else if(arg1 == "噪音衰减")
    {
        ui->comboBox_unit_name->clear();
        ui->comboBox_unit_name->addItem("分支/静压箱");
        ui->comboBox_unit_name->addItem("分支/风道多分支");
        ui->comboBox_unit_name->addItem("分支/三通");
        ui->comboBox_unit_name->addItem("直管");
        ui->comboBox_unit_name->addItem("弯头");
        ui->comboBox_unit_name->addItem("变径");
        ui->comboBox_unit_name->addItem("消音器");
    }
    else if(arg1 == "声压级计算")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_room_cal);
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_less425);
        ui->comboBox_room_type->setCurrentIndex(0);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
    }
}


void RoomCalTable::on_comboBox_unit_name_currentTextChanged(const QString &arg1)
{
    // 递归清空切换前页面中的 QLineEdit 和 QComboBox
    clearPage(ui->stackedWidget_table->currentWidget());

    if(arg1 == "风机")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_fan);

        ui->comboBox_fan_number->clear();
        for(QString number: systemListMap[systemName][arg1])
        {
            ui->comboBox_fan_number->addItem(number);
        }
        noi_after_cal_lineEdits = { ui->lineEdit_fan_63, ui->lineEdit_fan_125, ui->lineEdit_fan_250,ui->lineEdit_fan_500,
                                        ui->lineEdit_fan_1k, ui->lineEdit_fan_2k, ui->lineEdit_fan_4k, ui->lineEdit_fan_8k,
                                        ui->lineEdit_fan_total };

        noi_lineEdits.clear();
        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "空调器")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_aircondition);

        ui->comboBox_aircondition_model->clear();
        for(QString number: systemListMap[systemName][arg1])
        {
            ui->comboBox_aircondition_model->addItem(number);
        }
        noi_after_cal_lineEdits = { ui->lineEdit_aircondition_63, ui->lineEdit_aircondition_125, ui->lineEdit_aircondition_250,ui->lineEdit_aircondition_500,
                                        ui->lineEdit_aircondition_1k, ui->lineEdit_aircondition_2k, ui->lineEdit_aircondition_4k, ui->lineEdit_aircondition_8k,
                                        ui->lineEdit_aircondition_total };
        noi_lineEdits.clear();
        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "风机盘管")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_fanCoil);

        ui->comboBox_fanCoil_number->clear();
        for(QString number: systemListMap[systemName][arg1])
        {
            ui->comboBox_fanCoil_number->addItem(number);
        }
        noi_after_cal_lineEdits = { ui->lineEdit_aircondition_63, ui->lineEdit_aircondition_125, ui->lineEdit_aircondition_250,ui->lineEdit_aircondition_500,
                                        ui->lineEdit_aircondition_1k, ui->lineEdit_aircondition_2k, ui->lineEdit_aircondition_4k, ui->lineEdit_aircondition_8k,
                                        ui->lineEdit_aircondition_total };
        noi_lineEdits.clear();
        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "阀门/变风量末端")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_VAV_terminal);

        noi_after_cal_lineEdits = { ui->lineEdit_VAV_terminal_after_63, ui->lineEdit_VAV_terminal_after_125, ui->lineEdit_VAV_terminal_after_250,ui->lineEdit_VAV_terminal_after_500,
                                        ui->lineEdit_VAV_terminal_after_1k, ui->lineEdit_VAV_terminal_after_2k, ui->lineEdit_VAV_terminal_after_4k, ui->lineEdit_VAV_terminal_after_8k,
                                        ui->lineEdit_VAV_terminal_after_total};

        noi_lineEdits = { ui->lineEdit_VAV_terminal_63, ui->lineEdit_VAV_terminal_125, ui->lineEdit_VAV_terminal_250,ui->lineEdit_VAV_terminal_500,
                          ui->lineEdit_VAV_terminal_1k, ui->lineEdit_VAV_terminal_2k, ui->lineEdit_VAV_terminal_4k, ui->lineEdit_VAV_terminal_8k,
                          ui->lineEdit_VAV_terminal_total};

        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "阀门/圆形调风门")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_circular_damper);

        noi_after_cal_lineEdits = { ui->lineEdit_circular_damper_after_63, ui->lineEdit_circular_damper_after_125, ui->lineEdit_circular_damper_after_250,ui->lineEdit_circular_damper_after_500,
                                        ui->lineEdit_circular_damper_after_1k, ui->lineEdit_circular_damper_after_2k, ui->lineEdit_circular_damper_after_4k, ui->lineEdit_circular_damper_after_8k,
                                        ui->lineEdit_circular_damper_after_total};

        noi_lineEdits = { ui->lineEdit_circular_damper_63, ui->lineEdit_circular_damper_125, ui->lineEdit_circular_damper_250,ui->lineEdit_circular_damper_500,
                          ui->lineEdit_circular_damper_1k, ui->lineEdit_circular_damper_2k, ui->lineEdit_circular_damper_4k, ui->lineEdit_circular_damper_8k,
                          ui->lineEdit_circular_damper_total};


        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "阀门/方形调风门")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_rect_damper);

        noi_after_cal_lineEdits = { ui->lineEdit_rect_damper_after_63, ui->lineEdit_rect_damper_after_125, ui->lineEdit_rect_damper_after_250,ui->lineEdit_rect_damper_after_500,
                                        ui->lineEdit_rect_damper_after_1k, ui->lineEdit_rect_damper_after_2k, ui->lineEdit_rect_damper_after_4k, ui->lineEdit_rect_damper_after_8k,
                                        ui->lineEdit_rect_damper_after_total};

        noi_lineEdits = { ui->lineEdit_rect_damper_63, ui->lineEdit_rect_damper_125, ui->lineEdit_rect_damper_250,ui->lineEdit_rect_damper_500,
                          ui->lineEdit_rect_damper_1k, ui->lineEdit_rect_damper_2k, ui->lineEdit_rect_damper_4k, ui->lineEdit_rect_damper_8k,
                          ui->lineEdit_rect_damper_total};

        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "末端/布风器+散流器")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_air_diff);

        noi_after_cal_lineEdits = { ui->lineEdit_air_diff_after_63, ui->lineEdit_air_diff_after_125, ui->lineEdit_air_diff_after_250,ui->lineEdit_air_diff_after_500,
                                        ui->lineEdit_air_diff_after_1k, ui->lineEdit_air_diff_after_2k, ui->lineEdit_air_diff_after_4k, ui->lineEdit_air_diff_after_8k,
                                        ui->lineEdit_air_diff_after_total};

        terminal_atten_lineEdits = { ui->lineEdit_air_diff_atten_63, ui->lineEdit_air_diff_atten_125, ui->lineEdit_air_diff_atten_250,ui->lineEdit_air_diff_atten_500,
                                     ui->lineEdit_air_diff_atten_1k, ui->lineEdit_air_diff_atten_2k, ui->lineEdit_air_diff_atten_4k, ui->lineEdit_air_diff_atten_8k,
                                     ui->lineEdit_air_diff_atten_total};

        terminal_refl_lineEdits = { ui->lineEdit_air_diff_refl_63, ui->lineEdit_air_diff_refl_125, ui->lineEdit_air_diff_refl_250,ui->lineEdit_air_diff_refl_500,
                                     ui->lineEdit_air_diff_refl_1k, ui->lineEdit_air_diff_refl_2k, ui->lineEdit_air_diff_refl_4k, ui->lineEdit_air_diff_refl_8k,
                                     ui->lineEdit_air_diff_refl_total};

        terminal_noi_lineEdits = { ui->lineEdit_air_diff_noi_63, ui->lineEdit_air_diff_noi_125, ui->lineEdit_air_diff_noi_250,ui->lineEdit_air_diff_noi_500,
                                   ui->lineEdit_air_diff_noi_1k, ui->lineEdit_air_diff_noi_2k, ui->lineEdit_air_diff_noi_4k, ui->lineEdit_air_diff_noi_8k,
                                   ui->lineEdit_air_diff_noi_total};

        noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "末端/抽风头")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_pump);

        noi_after_cal_lineEdits = { ui->lineEdit_pump_after_63, ui->lineEdit_pump_after_125, ui->lineEdit_pump_after_250,ui->lineEdit_pump_after_500,
                                        ui->lineEdit_pump_after_1k, ui->lineEdit_pump_after_2k, ui->lineEdit_pump_after_4k, ui->lineEdit_pump_after_8k,
                                        ui->lineEdit_pump_after_total};

        terminal_atten_lineEdits = { ui->lineEdit_pump_atten_63, ui->lineEdit_pump_atten_125, ui->lineEdit_pump_atten_250,ui->lineEdit_pump_atten_500,
                                     ui->lineEdit_pump_atten_1k, ui->lineEdit_pump_atten_2k, ui->lineEdit_pump_atten_4k, ui->lineEdit_pump_atten_8k,
                                     ui->lineEdit_pump_atten_total};

        terminal_refl_lineEdits = { ui->lineEdit_pump_refl_63, ui->lineEdit_pump_refl_125, ui->lineEdit_pump_refl_250,ui->lineEdit_pump_refl_500,
                                     ui->lineEdit_pump_refl_1k, ui->lineEdit_pump_refl_2k, ui->lineEdit_pump_refl_4k, ui->lineEdit_pump_refl_8k,
                                     ui->lineEdit_pump_refl_total};

        terminal_noi_lineEdits = { ui->lineEdit_pump_noi_63, ui->lineEdit_pump_noi_125, ui->lineEdit_pump_noi_250,ui->lineEdit_pump_noi_500,
                                   ui->lineEdit_pump_noi_1k, ui->lineEdit_pump_noi_2k, ui->lineEdit_pump_noi_4k, ui->lineEdit_pump_noi_8k,
                                   ui->lineEdit_pump_noi_total};

        noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "末端/送风头")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_send);

        noi_after_cal_lineEdits = { ui->lineEdit_send_after_63, ui->lineEdit_send_after_125, ui->lineEdit_send_after_250,ui->lineEdit_send_after_500,
                                        ui->lineEdit_send_after_1k, ui->lineEdit_send_after_2k, ui->lineEdit_send_after_4k, ui->lineEdit_send_after_8k,
                                        ui->lineEdit_send_after_total};

        terminal_atten_lineEdits = { ui->lineEdit_send_atten_63, ui->lineEdit_send_atten_125, ui->lineEdit_send_atten_250,ui->lineEdit_send_atten_500,
                                     ui->lineEdit_send_atten_1k, ui->lineEdit_send_atten_2k, ui->lineEdit_send_atten_4k, ui->lineEdit_send_atten_8k,
                                     ui->lineEdit_send_atten_total};

        terminal_refl_lineEdits = { ui->lineEdit_send_refl_63, ui->lineEdit_send_refl_125, ui->lineEdit_send_refl_250,ui->lineEdit_send_refl_500,
                                     ui->lineEdit_send_refl_1k, ui->lineEdit_send_refl_2k, ui->lineEdit_send_refl_4k, ui->lineEdit_send_refl_8k,
                                     ui->lineEdit_send_refl_total};

        terminal_noi_lineEdits = { ui->lineEdit_send_noi_63, ui->lineEdit_send_noi_125, ui->lineEdit_send_noi_250,ui->lineEdit_send_noi_500,
                                   ui->lineEdit_send_noi_1k, ui->lineEdit_send_noi_2k, ui->lineEdit_send_noi_4k, ui->lineEdit_send_noi_8k,
                                   ui->lineEdit_send_noi_total};

        noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "末端/静压箱+格栅")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_staticBox_grille);

        noi_after_cal_lineEdits = { ui->lineEdit_staticBox_grille_after_63, ui->lineEdit_staticBox_grille_after_125, ui->lineEdit_staticBox_grille_after_250,ui->lineEdit_staticBox_grille_after_500,
                                        ui->lineEdit_staticBox_grille_after_1k, ui->lineEdit_staticBox_grille_after_2k, ui->lineEdit_staticBox_grille_after_4k, ui->lineEdit_staticBox_grille_after_8k,
                                        ui->lineEdit_staticBox_grille_after_total};

        terminal_atten_lineEdits = { ui->lineEdit_staticBox_grille_atten_63, ui->lineEdit_staticBox_grille_atten_125, ui->lineEdit_staticBox_grille_atten_250,ui->lineEdit_staticBox_grille_atten_500,
                                     ui->lineEdit_staticBox_grille_atten_1k, ui->lineEdit_staticBox_grille_atten_2k, ui->lineEdit_staticBox_grille_atten_4k, ui->lineEdit_staticBox_grille_atten_8k,
                                     ui->lineEdit_staticBox_grille_atten_total};

        terminal_refl_lineEdits = { ui->lineEdit_staticBox_grille_refl_63, ui->lineEdit_staticBox_grille_refl_125, ui->lineEdit_staticBox_grille_refl_250,ui->lineEdit_staticBox_grille_refl_500,
                                     ui->lineEdit_staticBox_grille_refl_1k, ui->lineEdit_staticBox_grille_refl_2k, ui->lineEdit_staticBox_grille_refl_4k, ui->lineEdit_staticBox_grille_refl_8k,
                                     ui->lineEdit_staticBox_grille_refl_total};

        terminal_noi_lineEdits = { ui->lineEdit_staticBox_grille_noi_63, ui->lineEdit_staticBox_grille_noi_125, ui->lineEdit_staticBox_grille_noi_250,ui->lineEdit_staticBox_grille_noi_500,
                                   ui->lineEdit_staticBox_grille_noi_1k, ui->lineEdit_staticBox_grille_noi_2k, ui->lineEdit_staticBox_grille_noi_4k, ui->lineEdit_staticBox_grille_noi_8k,
                                   ui->lineEdit_staticBox_grille_noi_total};

        noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "末端/置换通风末端")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_disp_vent_terminal);

        noi_after_cal_lineEdits = { ui->lineEdit_disp_vent_terminal_after_63, ui->lineEdit_disp_vent_terminal_after_125, ui->lineEdit_disp_vent_terminal_after_250,ui->lineEdit_disp_vent_terminal_after_500,
                                        ui->lineEdit_disp_vent_terminal_after_1k, ui->lineEdit_disp_vent_terminal_after_2k, ui->lineEdit_disp_vent_terminal_after_4k, ui->lineEdit_disp_vent_terminal_after_8k,
                                        ui->lineEdit_disp_vent_terminal_after_total};

        terminal_atten_lineEdits = { ui->lineEdit_disp_vent_terminal_atten_63, ui->lineEdit_disp_vent_terminal_atten_125, ui->lineEdit_disp_vent_terminal_atten_250,ui->lineEdit_disp_vent_terminal_atten_500,
                                     ui->lineEdit_disp_vent_terminal_atten_1k, ui->lineEdit_disp_vent_terminal_atten_2k, ui->lineEdit_disp_vent_terminal_atten_4k, ui->lineEdit_disp_vent_terminal_atten_8k,
                                     ui->lineEdit_disp_vent_terminal_atten_total};

        terminal_refl_lineEdits = { ui->lineEdit_disp_vent_terminal_refl_63, ui->lineEdit_disp_vent_terminal_refl_125, ui->lineEdit_disp_vent_terminal_refl_250,ui->lineEdit_disp_vent_terminal_refl_500,
                                     ui->lineEdit_disp_vent_terminal_refl_1k, ui->lineEdit_disp_vent_terminal_refl_2k, ui->lineEdit_disp_vent_terminal_refl_4k, ui->lineEdit_disp_vent_terminal_refl_8k,
                                     ui->lineEdit_disp_vent_terminal_refl_total};

        terminal_noi_lineEdits = { ui->lineEdit_disp_vent_terminal_noi_63, ui->lineEdit_disp_vent_terminal_noi_125, ui->lineEdit_disp_vent_terminal_noi_250,ui->lineEdit_disp_vent_terminal_noi_500,
                                   ui->lineEdit_disp_vent_terminal_noi_1k, ui->lineEdit_disp_vent_terminal_noi_2k, ui->lineEdit_disp_vent_terminal_noi_4k, ui->lineEdit_disp_vent_terminal_noi_8k,
                                   ui->lineEdit_disp_vent_terminal_noi_total};

        noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "末端/其他通风末端")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_other_send_terminal);

        noi_after_cal_lineEdits = { ui->lineEdit_other_send_terminal_after_63, ui->lineEdit_other_send_terminal_after_125, ui->lineEdit_other_send_terminal_after_250,ui->lineEdit_other_send_terminal_after_500,
                                        ui->lineEdit_other_send_terminal_after_1k, ui->lineEdit_other_send_terminal_after_2k, ui->lineEdit_other_send_terminal_after_4k, ui->lineEdit_other_send_terminal_after_8k,
                                        ui->lineEdit_other_send_terminal_after_total};

        terminal_atten_lineEdits = { ui->lineEdit_other_send_terminal_atten_63, ui->lineEdit_other_send_terminal_atten_125, ui->lineEdit_other_send_terminal_atten_250,ui->lineEdit_other_send_terminal_atten_500,
                                     ui->lineEdit_other_send_terminal_atten_1k, ui->lineEdit_other_send_terminal_atten_2k, ui->lineEdit_other_send_terminal_atten_4k, ui->lineEdit_other_send_terminal_atten_8k,
                                     ui->lineEdit_other_send_terminal_atten_total};

        terminal_refl_lineEdits = { ui->lineEdit_other_send_terminal_refl_63, ui->lineEdit_other_send_terminal_refl_125, ui->lineEdit_other_send_terminal_refl_250,ui->lineEdit_other_send_terminal_refl_500,
                                     ui->lineEdit_other_send_terminal_refl_1k, ui->lineEdit_other_send_terminal_refl_2k, ui->lineEdit_other_send_terminal_refl_4k, ui->lineEdit_other_send_terminal_refl_8k,
                                     ui->lineEdit_other_send_terminal_refl_total};

        terminal_noi_lineEdits = { ui->lineEdit_other_send_terminal_noi_63, ui->lineEdit_other_send_terminal_noi_125, ui->lineEdit_other_send_terminal_noi_250,ui->lineEdit_other_send_terminal_noi_500,
                                   ui->lineEdit_other_send_terminal_noi_1k, ui->lineEdit_other_send_terminal_noi_2k, ui->lineEdit_other_send_terminal_noi_4k, ui->lineEdit_other_send_terminal_noi_8k,
                                   ui->lineEdit_other_send_terminal_noi_total};

        noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "分支/静压箱")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_static_box);

        noi_after_cal_lineEdits = { ui->lineEdit_static_box_after_63, ui->lineEdit_static_box_after_125, ui->lineEdit_static_box_after_250,ui->lineEdit_static_box_after_500,
                                        ui->lineEdit_static_box_after_1k, ui->lineEdit_static_box_after_2k, ui->lineEdit_static_box_after_4k, ui->lineEdit_static_box_after_8k,
                                        ui->lineEdit_static_box_after_total};

        atten_lineEdits = { ui->lineEdit_static_box_63, ui->lineEdit_static_box_125, ui->lineEdit_static_box_250,ui->lineEdit_static_box_500,
                            ui->lineEdit_static_box_1k, ui->lineEdit_static_box_2k, ui->lineEdit_static_box_4k, ui->lineEdit_static_box_8k,
                            ui->lineEdit_static_box_total};

        noi_lineEdits.clear();
        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
    }
    else if(arg1 == "分支/风道多分支")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_duct_with_multi_ranc);

        noi_after_cal_lineEdits = { ui->lineEdit_duct_with_multi_ranc_after_63, ui->lineEdit_duct_with_multi_ranc_after_125, ui->lineEdit_duct_with_multi_ranc_after_250,ui->lineEdit_duct_with_multi_ranc_after_500,
                                        ui->lineEdit_duct_with_multi_ranc_after_1k, ui->lineEdit_duct_with_multi_ranc_after_2k, ui->lineEdit_duct_with_multi_ranc_after_4k, ui->lineEdit_duct_with_multi_ranc_after_8k,
                                        ui->lineEdit_duct_with_multi_ranc_after_total};

        atten_lineEdits = { ui->lineEdit_duct_with_multi_ranc_63, ui->lineEdit_duct_with_multi_ranc_125, ui->lineEdit_duct_with_multi_ranc_250,ui->lineEdit_duct_with_multi_ranc_500,
                            ui->lineEdit_duct_with_multi_ranc_1k, ui->lineEdit_duct_with_multi_ranc_2k, ui->lineEdit_duct_with_multi_ranc_4k, ui->lineEdit_duct_with_multi_ranc_8k,
                            ui->lineEdit_duct_with_multi_ranc_total};

        noi_lineEdits.clear();
        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
    }
    else if(arg1 == "分支/三通")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_tee);

        noi_after_cal_lineEdits = { ui->lineEdit_tee_after_63, ui->lineEdit_tee_after_125, ui->lineEdit_tee_after_250,ui->lineEdit_tee_after_500,
                                        ui->lineEdit_tee_after_1k, ui->lineEdit_tee_after_2k, ui->lineEdit_tee_after_4k, ui->lineEdit_tee_after_8k,
                                        ui->lineEdit_tee_after_total};

        atten_lineEdits = { ui->lineEdit_tee_63, ui->lineEdit_tee_125, ui->lineEdit_tee_250,ui->lineEdit_tee_500,
                            ui->lineEdit_tee_1k, ui->lineEdit_tee_2k, ui->lineEdit_tee_4k, ui->lineEdit_tee_8k,
                            ui->lineEdit_tee_total};

        noi_lineEdits.clear();
        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
    }
    else if(arg1 == "直管")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_pipe);

        noi_after_cal_lineEdits = { ui->lineEdit_pipe_after_63, ui->lineEdit_pipe_after_125, ui->lineEdit_pipe_after_250,ui->lineEdit_pipe_after_500,
                                        ui->lineEdit_pipe_after_1k, ui->lineEdit_pipe_after_2k, ui->lineEdit_pipe_after_4k, ui->lineEdit_pipe_after_8k,
                                        ui->lineEdit_pipe_after_total};

        each_atten_lineEdits = { ui->lineEdit_pipe_63, ui->lineEdit_pipe_125, ui->lineEdit_pipe_250,ui->lineEdit_pipe_500,
                                        ui->lineEdit_pipe_1k, ui->lineEdit_pipe_2k, ui->lineEdit_pipe_4k, ui->lineEdit_pipe_8k,
                                        ui->lineEdit_pipe_total};

        sum_atten_lineEdits = { ui->lineEdit_pipe_sum_63, ui->lineEdit_pipe_sum_125, ui->lineEdit_pipe_sum_250,ui->lineEdit_pipe_sum_500,
                                ui->lineEdit_pipe_sum_1k, ui->lineEdit_pipe_sum_2k, ui->lineEdit_pipe_sum_4k, ui->lineEdit_pipe_sum_8k,
                                ui->lineEdit_pipe_sum_total};

        noi_lineEdits.clear();
        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "弯头")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_elbow);

        noi_after_cal_lineEdits = { ui->lineEdit_elbow_after_63, ui->lineEdit_elbow_after_125, ui->lineEdit_elbow_after_250,ui->lineEdit_elbow_after_500,
                                        ui->lineEdit_elbow_after_1k, ui->lineEdit_elbow_after_2k, ui->lineEdit_elbow_after_4k, ui->lineEdit_elbow_after_8k,
                                        ui->lineEdit_elbow_after_total};

        each_atten_lineEdits = { ui->lineEdit_elbow_63, ui->lineEdit_elbow_125, ui->lineEdit_elbow_250,ui->lineEdit_elbow_500,
                                        ui->lineEdit_elbow_1k, ui->lineEdit_elbow_2k, ui->lineEdit_elbow_4k, ui->lineEdit_elbow_8k,
                                        ui->lineEdit_elbow_total};

        sum_atten_lineEdits = { ui->lineEdit_elbow_sum_63, ui->lineEdit_elbow_sum_125, ui->lineEdit_elbow_sum_250,ui->lineEdit_elbow_sum_500,
                                ui->lineEdit_elbow_sum_1k, ui->lineEdit_elbow_sum_2k, ui->lineEdit_elbow_sum_4k, ui->lineEdit_elbow_sum_8k,
                                ui->lineEdit_elbow_sum_total};

        noi_lineEdits.clear();
        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        atten_lineEdits.clear();
    }
    else if(arg1 == "变径")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_reducer);

        noi_after_cal_lineEdits = { ui->lineEdit_reducer_after_63, ui->lineEdit_reducer_after_125, ui->lineEdit_reducer_after_250,ui->lineEdit_reducer_after_500,
                                        ui->lineEdit_reducer_after_1k, ui->lineEdit_reducer_after_2k, ui->lineEdit_reducer_after_4k, ui->lineEdit_reducer_after_8k,
                                        ui->lineEdit_reducer_after_total};

        atten_lineEdits = { ui->lineEdit_reducer_63, ui->lineEdit_reducer_125, ui->lineEdit_reducer_250,ui->lineEdit_reducer_500,
                                        ui->lineEdit_reducer_1k, ui->lineEdit_reducer_2k, ui->lineEdit_reducer_4k, ui->lineEdit_reducer_8k,
                                        ui->lineEdit_reducer_total};

        noi_lineEdits.clear();
        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
    }
    else if(arg1 == "消音器")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_silencer);

        noi_after_cal_lineEdits = { ui->lineEdit_silencer_after_63, ui->lineEdit_silencer_after_125, ui->lineEdit_silencer_after_250,ui->lineEdit_silencer_after_500,
                                        ui->lineEdit_silencer_after_1k, ui->lineEdit_silencer_after_2k, ui->lineEdit_silencer_after_4k, ui->lineEdit_silencer_after_8k, nullptr};

        atten_lineEdits = { ui->lineEdit_silencer_63, ui->lineEdit_silencer_125, ui->lineEdit_silencer_250,ui->lineEdit_silencer_500,
                                        ui->lineEdit_silencer_1k, ui->lineEdit_silencer_2k, ui->lineEdit_silencer_4k, ui->lineEdit_silencer_8k, nullptr};

        noi_lineEdits.clear();
        terminal_atten_lineEdits.clear();
        terminal_refl_lineEdits.clear();
        terminal_noi_lineEdits.clear();
        each_atten_lineEdits.clear();
        sum_atten_lineEdits.clear();
    }
    // 递归清空切换后页面中的 QLineEdit 和 QComboBox
    clearPage(ui->stackedWidget_table->currentWidget());
}


void RoomCalTable::on_comboBox_sound_type_room_currentTextChanged(const QString &arg1)
{
    ui->stackedWidget_title->setCurrentWidget(ui->page_unit);
    ui->comboBox_sound_type->setCurrentText(arg1);
    if(arg1 == "声源噪音")
    {
        ui->comboBox_unit_name->clear();
        ui->comboBox_unit_name->addItem("风机");
        ui->comboBox_unit_name->addItem("风机盘管");
        ui->comboBox_unit_name->addItem("空调器");
    }
    else if(arg1 == "气流噪音")
    {
        ui->comboBox_unit_name->clear();
        ui->comboBox_unit_name->addItem("阀门/变风量末端");
        ui->comboBox_unit_name->addItem("阀门/圆形调风门");
        ui->comboBox_unit_name->addItem("阀门/方形调风门");
    }
    else if(arg1 == "噪音衰减+气流噪音")
    {
        ui->comboBox_unit_name->clear();
        ui->comboBox_unit_name->addItem("末端/布风器+散流器");
        ui->comboBox_unit_name->addItem("末端/抽风头");
        ui->comboBox_unit_name->addItem("末端/送风头");
    }
    else if(arg1 == "噪音衰减+气流噪音")
    {
        ui->comboBox_unit_name->clear();
        ui->comboBox_unit_name->addItem("末端/布风器+散流器");
        ui->comboBox_unit_name->addItem("末端/抽风头");
        ui->comboBox_unit_name->addItem("末端/送风头");
        ui->comboBox_unit_name->addItem("末端/静压箱+格栅");
        ui->comboBox_unit_name->addItem("末端/置换通风末端");
        ui->comboBox_unit_name->addItem("末端/其他通风末端");
    }
    else if(arg1 == "噪音衰减")
    {
        ui->comboBox_unit_name->clear();
        ui->comboBox_unit_name->addItem("分支/静压箱");
        ui->comboBox_unit_name->addItem("分支/风道多分支");
        ui->comboBox_unit_name->addItem("分支/三通");
        ui->comboBox_unit_name->addItem("直管");
        ui->comboBox_unit_name->addItem("弯头");
        ui->comboBox_unit_name->addItem("变径");
        ui->comboBox_unit_name->addItem("消音器");
    }
    else if(arg1 == "声压级计算")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_room_cal);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
        ui->comboBox_room_type->setCurrentIndex(0);
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_less425);
    }
}

void RoomCalTable::setIsCollapsed(bool isCollapsed)
{
    this->isCollapsed = isCollapsed;
}

void RoomCalTable::setCollapsed()
{
    if(!isCollapsed)
    {
        ui->stackedWidget_table->setVisible(false);
        this->setMinimumSize(QSize(this->width(), ui->widget_title->height()));
        this->setMaximumSize(QSize(this->width(), ui->widget_title->height()));
        ui->widget_title->setStyleSheet(QString("#widget_title{ border:2px solid black;}QLabel{font-family: '黑体';font-size: 15px;}QLineEdit,QComboBox{font-family: '黑体';font-size: 14px;}QComboBox QAbstractItemView::item { height: 30px; }"));
    }
    else
    {
        ui->stackedWidget_table->setVisible(true);
        // 清除最小和最大大小限制
        this->setMinimumSize(QSize(0, 0));
        this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        // 获取当前显示的页面的部件
        QWidget *currentWidget = ui->stackedWidget_table->currentWidget();

        if (currentWidget) {
                int tableHeight = currentWidget->height();
                int titleHeight = ui->widget_title->height();

                // 设置窗口的高度
                this->setMinimumSize(QSize(this->width(), tableHeight + titleHeight));
                this->setMaximumSize(QSize(this->width(), tableHeight + titleHeight));
        }
        ui->widget_title->setStyleSheet(QString("#widget_title{ border:2px solid black;border-bottom:1px solid black;}QLabel{font-family: '黑体';font-size: 15px;}QLineEdit,QComboBox{font-family: '黑体';font-size: 14px;}QComboBox QAbstractItemView::item { height: 30px; }"));
    }
    isCollapsed = !isCollapsed;
}

void RoomCalTable::on_pushButton_number_clicked()
{
    setCollapsed();
}

bool RoomCalTable::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel && qobject_cast<QComboBox*>(obj)) {
        // 如果事件是鼠标滚轮事件，并且发生在 QComboBox 上，阻止事件传递
        return true;
    }

    // 其他事件按默认处理方式进行
    return QObject::eventFilter(obj, event);
}

void RoomCalTable::setSerialNum(int num)
{
    ui->pushButton_number->setText(QString::number(num));
}

void RoomCalTable::updateComboBoxItems() {
//    // 清除当前所有选项
//    ui->comboBox_noi_src_num->clear();

//    if(ui->stackedWidget_title->currentWidget() == ui->page_unit)
//    {
//        for(QString number : systemListMap[systemName][ui->comboBox_unit_name->currentText()])
//        {
//            ui->comboBox_noi_src_num->addItem(number);
//        }
//    }
}


void RoomCalTable::on_comboBox_room_type_currentTextChanged(const QString &arg1)
{
    if(arg1 == "体积小于425m³房间（点噪音源）")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_less425);
    }
    else if(arg1 == "体积大于425m³房间（点噪音源）")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_more425);
    }
    else if(arg1 == "无家具空房间（点噪音源）")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_noFurniture);
    }
    else if(arg1 == "室外开敞处所（点噪音源）")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_open);
    }
    else if(arg1 == "条缝风口房间（线噪音源）")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_gap_tuyere);
    }
    else if(arg1 == "雨降风口房间（面噪音源）")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_rain);
    }
}

void RoomCalTable::connectLineEditsToCalSlot(const QVector<QLineEdit*>& lineEdits) {
    for (QLineEdit* lineEdit : lineEdits) {
        // 确保 lineEdit 非空后再连接
        if (lineEdit) {
            connect(lineEdit, &QLineEdit::textChanged, this, &RoomCalTable::calVariations);
        } else {
            // 处理潜在的空 lineEdit（可选）
            // 您可能需要记录警告或提供备用行为
        }
    }
}

void RoomCalTable::disconnectLineEditsToCalSlot(const QVector<QLineEdit*>& lineEdits)
{
    for (QLineEdit* lineEdit : lineEdits) {
        if (lineEdit) {
            disconnect(lineEdit, &QLineEdit::textChanged, this, &RoomCalTable::calVariations);
        }
    }
}

void RoomCalTable::calVariations()
{
    if(!noi_lineEdits.empty())
    {
        for(int i = 0; i < 8; i++)
        {
            if(!(noi_lineEdits[i] && noi_lineEdits[i]->text() != ""))
            {
                variations.clear();
                return;
            }
            variations[i] = noi_lineEdits[i]->text();
        }
    }
    else if(!atten_lineEdits.empty())
    {
        for(int i = 0; i < 8; i++)
        {
            if(!(atten_lineEdits[i] && atten_lineEdits[i]->text() != ""))
            {
                variations.clear();
                return;
            }
            variations[i] = atten_lineEdits[i]->text();
        }
    }
    else if(!terminal_atten_lineEdits.empty() && !terminal_refl_lineEdits.empty() && !terminal_noi_lineEdits.empty())
    {
        for(int i = 0; i < 8; i++)
        {
            if(!(terminal_atten_lineEdits[i] && terminal_atten_lineEdits[i]->text() != ""
                 && terminal_refl_lineEdits[i] && terminal_refl_lineEdits[i]->text() != ""
                 && terminal_noi_lineEdits[i] && terminal_noi_lineEdits[i]->text() != ""))
            {
                variations.clear();
                return;
            }

            double atten = terminal_atten_lineEdits[i]->text().toDouble();
            double refl = terminal_atten_lineEdits[i]->text().toDouble();
            double noi = terminal_atten_lineEdits[i]->text().toDouble();
            double last_noi = noise_before_cal[i].toDouble();

            double sup_noi = 10 * log10(pow(10,noi / 10) + pow(10, last_noi / 10));

            variations[i] = QString::number(sup_noi - atten - refl ,'f',1);
        }
    }
    else if(!each_atten_lineEdits.empty() && !sum_atten_lineEdits.empty())
    {
        for(int i = 0; i < 8; i++)
        {
            if(!(each_atten_lineEdits[i] && each_atten_lineEdits[i]->text() != ""
                 && sum_atten_lineEdits[i] && sum_atten_lineEdits[i]->text() != ""))
            {
                variations.clear();
                return;
            }

            if(ui->lineEdit_pipe_length && ui->lineEdit_pipe_length->text() != "")
            {
                double each_atten = each_atten_lineEdits[i]->text().toDouble();
                sum_atten_lineEdits[i]->setText(QString::number(each_atten * ui->lineEdit_pipe_length->text().toInt(),'f',1));
                variations[i] = QString::number(each_atten * ui->lineEdit_pipe_length->text().toInt(),'f',1);
            }
            else if(ui->lineEdit_elbow_count && ui->lineEdit_elbow_count->text() != "")
            {
                 double each_atten = each_atten_lineEdits[i]->text().toDouble();
                 sum_atten_lineEdits[i]->setText(QString::number(each_atten * ui->lineEdit_elbow_count->text().toInt(),'f',1));
                 variations[i] = QString::number(each_atten * ui->lineEdit_elbow_count->text().toInt(),'f',1);
            }
            else
            {
                return;
            }
        }
    }

    double temp = 0;

    for(int i = 0; i < 8; i++)
    {
        if(variations[i].isEmpty())
        {
            return;
        }
        noi_after_cal_lineEdits[i]->setText(variations[i]);
        temp += pow(10, variations[i].toDouble() / 10);
    }
    double total = 10 * log10(temp);
    noi_after_cal_lineEdits[8]->setText(QString::number(total,'f' ,1));
}
