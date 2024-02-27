#include "roomcaltable.h"
#include "ui_roomcaltable.h"
#include <QContextMenuEvent>
#include "globle_var.h"
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout> // 添加此行
#include <QDebug>

RoomCalTable::RoomCalTable(QString systemName, QWidget *parent,QString m_type) :
    QWidget(parent),
    systemName(systemName),
    isCollapsed(false),
    ui(new Ui::RoomCalTable)
{
    ui->setupUi(this);
    connect(ui->comboBox_noi_src_num, &ClickableComboBox::clicked, this, &RoomCalTable::updateComboBoxItems);

    this->isValid = true;
    this->type = m_type;
    if(type == "")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_unit);
        ui->comboBox_sound_type->setCurrentText("声源噪音");
        ui->comboBox_unit_name->setCurrentIndex(-1);
        ui->stackedWidget_info->setCurrentWidget(ui->page_noi_src_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_noi_src_table);
        this->type = "声源噪音";
    }
    else if(type == "声压级计算")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_room_cal);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
        ui->comboBox_room_type->setCurrentIndex(-1);
        ui->stackedWidget_info->setCurrentWidget(ui->page_room_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_table);
    }
    else if (type == "声源噪音" || type == "气流噪音" || type == "噪音衰减+气流噪音" || type == "噪音衰减")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_unit);
        ui->comboBox_sound_type->setCurrentText(type);
        if(type == "声源噪音")
        {
            ui->comboBox_unit_name->setCurrentText("风机");
            ui->stackedWidget_info->setCurrentWidget(ui->page_noi_src_info);
            ui->stackedWidget_table->setCurrentWidget(ui->page_noi_src_table);
        }
        else if(type == "气流噪音")
        {
            ui->comboBox_unit_name->setCurrentText("阀门/变风量末端");
            ui->stackedWidget_info->setCurrentWidget(ui->page_vav_terminal_info);
            ui->stackedWidget_table->setCurrentWidget(ui->page_damper_table);
        }
        else if(type == "噪音衰减+气流噪音")
        {
            ui->comboBox_unit_name->setCurrentText("末端/布风器+散流器");
            ui->stackedWidget_info->setCurrentWidget(ui->page_atten_info);
            ui->stackedWidget_atten_noi_model->setCurrentWidget(ui->page_air_diff_model);
            ui->stackedWidget_table->setCurrentWidget(ui->page_atten_noi_table);
        }
        else if(type == "噪音衰减")
        {
            ui->comboBox_unit_name->setCurrentText("分支/静压箱");
            ui->stackedWidget_info->setCurrentWidget(ui->page_branch_info);
            ui->stackedWidget_table->setCurrentWidget(ui->page_branch_table);
        }
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
        // 获取所有子对象
        QList<QObject *> childObjects = currentWidget->children();

        // 遍历所有子对象
        for (QObject *childObject : childObjects) {
            // 判断是否是 QWidget 类型
            if (QWidget *childWidget = qobject_cast<QWidget *>(childObject)) {
                int widgetHeight = childWidget->height();

                // 设置 QStackedWidget 的高度
                ui->stackedWidget_table->setFixedHeight(widgetHeight);

                // 设置窗口的高度
                this->setMinimumSize(QSize(this->width(), 75 + widgetHeight));
                this->setMaximumSize(QSize(this->width(), 75 + widgetHeight));

                // 假设只需要获取第一个符合条件的子部件，可以注释下面这行来继续查找其他子部件
                break;
            }
        }
    }


    auto qComboBoxGroup= this->findChildren<QComboBox*>();
    for(auto each:qComboBoxGroup)
    {
       each->installEventFilter(this);
    }
    if(ui->stackedWidget_title->currentWidget() == ui->page_unit)
        this->currentTable = ui->comboBox_unit_name->currentText();
    else
        this->currentTable = ui->comboBox_room_type->currentText();
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
        // 获取所有子对象
        QList<QObject *> childObjects = currentWidget->children();

        // 遍历所有子对象
        for (QObject *childObject : childObjects) {
            // 判断是否是 QWidget 类型
            if (QWidget *childWidget = qobject_cast<QWidget *>(childObject)) {
                int widgetHeight = childWidget->height();

                // 设置 QStackedWidget 的高度
                ui->stackedWidget_table->setFixedHeight(widgetHeight);

                // 设置窗口的高度
                this->setMinimumSize(QSize(this->width(), 75 + widgetHeight));
                this->setMaximumSize(QSize(this->width(), 75 + widgetHeight));

                // 假设只需要获取第一个符合条件的子部件，可以注释下面这行来继续查找其他子部件
                break;
            }
        }
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
        ui->comboBox_unit_name->addItem("末端/静压+格栅");
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
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_table);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
    }
}


void RoomCalTable::on_comboBox_unit_name_currentTextChanged(const QString &arg1)
{
    // 获取当前页面索引
    int currentIndex = ui->stackedWidget_info->currentIndex();

    // 获取当前页面
    QWidget *currentPage = ui->stackedWidget_info->widget(currentIndex);

    // 递归清空当前页面中的 QLineEdit 和 QComboBox
    clearPage(currentPage);

    this->currentTable = ui->comboBox_unit_name->currentText();

    if(arg1 == "风机" || arg1 == "空调器" || arg1 == "风机盘管")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_noi_src_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_noi_src_table);

        ui->comboBox_noi_src_num->clear();
        for(QString number: systemListMap[systemName][arg1])
        {
            ui->comboBox_noi_src_num->addItem(number);
        }
        noi_after_cal_lineEdits = { ui->lineEdit_noi_src_63, ui->lineEdit_noi_src_125, ui->lineEdit_noi_src_250,ui->lineEdit_noi_src_500,
                                        ui->lineEdit_noi_src_1k, ui->lineEdit_noi_src_2k, ui->lineEdit_noi_src_4k, ui->lineEdit_noi_src_8k,
                                        ui->lineEdit_noi_src_total};
    }
    else if(arg1 == "阀门/变风量末端")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_vav_terminal_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_damper_table);

        noi_after_cal_lineEdits = { ui->lineEdit_damper_after_63, ui->lineEdit_damper_after_125, ui->lineEdit_damper_after_250,ui->lineEdit_damper_after_500,
                                        ui->lineEdit_damper_after_1k, ui->lineEdit_damper_after_2k, ui->lineEdit_damper_after_4k, ui->lineEdit_damper_after_8k,
                                        ui->lineEdit_damper_after_total};
    }
    else if(arg1 == "阀门/圆形调风门")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_circle_damper_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_damper_table);

        noi_after_cal_lineEdits = { ui->lineEdit_damper_after_63, ui->lineEdit_damper_after_125, ui->lineEdit_damper_after_250,ui->lineEdit_damper_after_500,
                                        ui->lineEdit_damper_after_1k, ui->lineEdit_damper_after_2k, ui->lineEdit_damper_after_4k, ui->lineEdit_damper_after_8k,
                                        ui->lineEdit_damper_after_total};
    }
    else if(arg1 == "阀门/方形调风门")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_rect_damper_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_damper_table);

        noi_after_cal_lineEdits = { ui->lineEdit_damper_after_63, ui->lineEdit_damper_after_125, ui->lineEdit_damper_after_250,ui->lineEdit_damper_after_500,
                                        ui->lineEdit_damper_after_1k, ui->lineEdit_damper_after_2k, ui->lineEdit_damper_after_4k, ui->lineEdit_damper_after_8k,
                                        ui->lineEdit_damper_after_total};
    }
    else if(arg1 == "末端/其他通风末端" || arg1 == "末端/抽风头" || arg1 == "末端/静压箱+格栅" || arg1 == "末端/送风头"
             || arg1 == "末端/置换通风末端" || arg1 == "末端/布风器+散流器")
    {

        ui->stackedWidget_info->setCurrentWidget(ui->page_atten_info);
        if(arg1 == "末端/其他通风末端")
        {
            ui->stackedWidget_atten_noi_model->setCurrentWidget(ui->page_other_send_terminal_model);
        }
        else if(arg1 == "末端/抽风头")
        {
            ui->stackedWidget_atten_noi_model->setCurrentWidget(ui->page_pump_model);
        }
        else if(arg1 == "末端/静压箱+格栅")
        {
            ui->stackedWidget_atten_noi_model->setCurrentWidget(ui->page_static_grille_model);
        }
        else if(arg1 == "末端/送风头")
        {
            ui->stackedWidget_atten_noi_model->setCurrentWidget(ui->page_send_model);
        }
        else if(arg1 == "末端/置换通风末端")
        {
            ui->stackedWidget_atten_noi_model->setCurrentWidget(ui->page_disp_vent_terminal_model);
        }
        else if(arg1 == "末端/布风器+散流器")
        {
            ui->stackedWidget_atten_noi_model->setCurrentWidget(ui->page_air_diff_model);
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_atten_noi_table);

        noi_after_cal_lineEdits = { ui->lineEdit_end_after_63, ui->lineEdit_end_after_125, ui->lineEdit_end_after_250,ui->lineEdit_end_after_500,
                                        ui->lineEdit_end_after_1k, ui->lineEdit_end_after_2k, ui->lineEdit_end_after_4k, ui->lineEdit_end_after_8k,
                                        ui->lineEdit_end_after_total};
    }
    else if(arg1 == "分支/静压箱" || arg1 == "分支/三通" || arg1 == "分支/风道多分支")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_branch_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_branch_table);

        noi_after_cal_lineEdits = { ui->lineEdit_branch_after_63, ui->lineEdit_branch_after_125, ui->lineEdit_branch_after_250,ui->lineEdit_branch_after_500,
                                        ui->lineEdit_branch_after_1k, ui->lineEdit_branch_after_2k, ui->lineEdit_branch_after_4k, ui->lineEdit_branch_after_8k,
                                        ui->lineEdit_branch_after_total};
    }
    else if(arg1 == "直管")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_pipe_elbow_info);
        ui->stackedWidget_pipe_elbow->setCurrentWidget(ui->page_pipe);
        ui->stackedWidget_table->setCurrentWidget(ui->page_pipe_table);

        noi_after_cal_lineEdits = { ui->lineEdit_pipe_after_63, ui->lineEdit_pipe_after_125, ui->lineEdit_pipe_after_250,ui->lineEdit_pipe_after_500,
                                        ui->lineEdit_pipe_after_1k, ui->lineEdit_pipe_after_2k, ui->lineEdit_pipe_after_4k, ui->lineEdit_pipe_after_8k,
                                        ui->lineEdit_pipe_after_total};
    }
    else if(arg1 == "弯头")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_pipe_elbow_info);
        ui->stackedWidget_pipe_elbow->setCurrentWidget(ui->page_elbow);
        ui->stackedWidget_table->setCurrentWidget(ui->page_elbow_table);

        noi_after_cal_lineEdits = { ui->lineEdit_elbow_after_63, ui->lineEdit_elbow_after_125, ui->lineEdit_elbow_after_250,ui->lineEdit_elbow_after_500,
                                        ui->lineEdit_elbow_after_1k, ui->lineEdit_elbow_after_2k, ui->lineEdit_elbow_after_4k, ui->lineEdit_elbow_after_8k,
                                        ui->lineEdit_elbow_after_total};
    }
    else if(arg1 == "变径")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_reducer_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_reducer_table);

        noi_after_cal_lineEdits = { ui->lineEdit_reducer_after_63, ui->lineEdit_reducer_after_125, ui->lineEdit_reducer_after_250,ui->lineEdit_reducer_after_500,
                                        ui->lineEdit_reducer_after_1k, ui->lineEdit_reducer_after_2k, ui->lineEdit_reducer_after_4k, ui->lineEdit_reducer_after_8k,
                                        ui->lineEdit_reducer_after_total};
    }
    else if(arg1 == "消音器")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_silencer_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_silencer_table);

        noi_after_cal_lineEdits = { ui->lineEdit_silencer_after_63, ui->lineEdit_silencer_after_125, ui->lineEdit_silencer_after_250,ui->lineEdit_silencer_after_500,
                                        ui->lineEdit_silencer_after_1k, ui->lineEdit_silencer_after_2k, ui->lineEdit_silencer_after_4k, ui->lineEdit_silencer_after_8k, nullptr};
    }
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
        ui->comboBox_unit_name->addItem("末端/静压+格栅");
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
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_table);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
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
        ui->stackedWidget_info->setVisible(false);
        ui->stackedWidget_table->setVisible(false);
        this->setMinimumSize(QSize(this->width(), ui->widget_title->height()));
        this->setMaximumSize(QSize(this->width(), ui->widget_title->height()));
        ui->widget_title->setStyleSheet(QString("#widget_title{ border:2px solid black;}QLabel{font-family: '黑体';font-size: 15px;}QLineEdit,QComboBox{font-family: '黑体';font-size: 14px;}QComboBox QAbstractItemView::item { height: 30px; }"));
    }
    else
    {
        ui->stackedWidget_info->setVisible(true);
        ui->stackedWidget_table->setVisible(true);
        // 清除最小和最大大小限制
        this->setMinimumSize(QSize(0, 0));
        this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        // 获取当前显示的页面的部件
        QWidget *currentWidget = ui->stackedWidget_table->currentWidget();

        if (currentWidget) {
            // 获取所有子对象
            QList<QObject *> childObjects = currentWidget->children();

            // 遍历所有子对象
            for (QObject *childObject : childObjects) {
                // 判断是否是 QWidget 类型
                if (QWidget *childWidget = qobject_cast<QWidget *>(childObject)) {
                    int widgetHeight = childWidget->height();

                    // 设置 QStackedWidget 的高度
                    ui->stackedWidget_table->setFixedHeight(widgetHeight);

                    // 设置窗口的高度
                    this->setMinimumSize(QSize(this->width(), 75 + widgetHeight));
                    this->setMaximumSize(QSize(this->width(), 75 + widgetHeight));

                    // 假设只需要获取第一个符合条件的子部件，可以注释下面这行来继续查找其他子部件
                    break;
                }
            }
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
    // 清除当前所有选项
    ui->comboBox_noi_src_num->clear();

    if(ui->stackedWidget_title->currentWidget() == ui->page_unit)
    {
        for(QString number : systemListMap[systemName][ui->comboBox_unit_name->currentText()])
        {
            ui->comboBox_noi_src_num->addItem(number);
        }
    }
}

void RoomCalTable::on_comboBox_room_type_currentTextChanged(const QString &arg1)
{
    if(arg1 == "室外开敞处所（点噪音源）")
    {
        ui->label_room_input_power->setText("进入开敞处所噪音声功率(dB)");
        ui->label_power_amend->setText(" 开敞处所声压与功率修正(dB) ");
    }
    else
    {
        ui->label_room_input_power->setText("进入房间噪音声功率(dB)");
        ui->label_power_amend->setText(" 房间声压与声功率修正(dB) ");
    }
}


void RoomCalTable::on_comboBox_noi_src_num_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_noi_src_num->currentText() == "")
        return;

    ComponentBase* component = componentManager.getComponentByNumber(arg1);
    if(ui->comboBox_unit_name->currentText() == "风机")
    {
        Fan_noise* fan = static_cast<Fan_noise*>(component);
        ui->lineEdit_noi_src_brand->setText(fan->brand);
        ui->lineEdit_noi_src_model->setText(fan->model);
        ui->lineEdit_noi_src_air_volume->setText(fan->air_volume);
        ui->lineEdit_noi_src_static_press->setText(fan->brand);
    }
    else if(ui->comboBox_unit_name->currentText() == "风机盘管")
    {
        FanCoil_noise* fanCoil = static_cast<FanCoil_noise*>(component);
        ui->lineEdit_noi_src_brand->setText(fanCoil->brand);
        ui->lineEdit_noi_src_model->setText(fanCoil->model);
        ui->lineEdit_noi_src_air_volume->setText(fanCoil->air_volume);
        ui->lineEdit_noi_src_static_press->setText(fanCoil->brand);
    }
    else if(ui->comboBox_unit_name->currentText() == "空调器")
    {
        Aircondition_noise* aircondition = static_cast<Aircondition_noise*>(component);
        ui->lineEdit_noi_src_brand->setText(aircondition->brand);
        ui->lineEdit_noi_src_model->setText(aircondition->model);
        ui->lineEdit_noi_src_air_volume->setText(aircondition->air_volume);
        ui->lineEdit_noi_src_static_press->setText(aircondition->brand);
    }
}


void RoomCalTable::on_comboBox_noi_locate_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_noi_src_num->currentText() == "")
        return;

    ComponentBase* component = componentManager.getComponentByNumber(ui->comboBox_noi_src_num->currentText());
    Fan_noise* fan = nullptr;
    FanCoil_noise* fanCoil = nullptr;
    Aircondition_noise* aircondition = nullptr;


    QVector<QString> noises;
    if(arg1 == "进口")
    {
        if(ui->comboBox_unit_name->currentText() == "风机")
        {
            fan = static_cast<Fan_noise*>(component);
            noises = {fan->noi_in_63, fan->noi_in_125, fan->noi_in_250, fan->noi_in_500, fan->noi_in_1k, fan->noi_in_2k, fan->noi_in_4k,
                        fan->noi_in_8k,fan->noi_in_total};
        }
        else if(ui->comboBox_unit_name->currentText() == "风机盘管")
        {
            fanCoil = static_cast<FanCoil_noise*>(component);
            noises = {fanCoil->noi_in_63, fanCoil->noi_in_125, fanCoil->noi_in_250, fanCoil->noi_in_500, fanCoil->noi_in_1k, fanCoil->noi_in_2k, fanCoil->noi_in_4k,
                        fanCoil->noi_in_8k,fanCoil->noi_in_total};
        }
        else if(ui->comboBox_unit_name->currentText() == "空调器")
        {
            aircondition = static_cast<Aircondition_noise*>(component);
            noises = {aircondition->noi_send_in_125, aircondition->noi_send_in_125, aircondition->noi_send_in_250, aircondition->noi_send_in_500, aircondition->noi_send_in_1k, aircondition->noi_send_in_2k, aircondition->noi_send_in_4k,
                        aircondition->noi_send_in_8k,aircondition->noi_send_in_total};
        }
    }
    else if(arg1 == "出口")
    {
        if(ui->comboBox_unit_name->currentText() == "风机")
        {
            fan = static_cast<Fan_noise*>(component);
            noises = {fan->noi_out_63, fan->noi_out_125, fan->noi_out_250, fan->noi_out_500, fan->noi_out_1k, fan->noi_out_2k, fan->noi_out_4k,
                        fan->noi_out_8k,fan->noi_out_total};
        }
        else if(ui->comboBox_unit_name->currentText() == "风机盘管")
        {
            fanCoil = static_cast<FanCoil_noise*>(component);
            noises = {fanCoil->noi_out_63, fanCoil->noi_out_125, fanCoil->noi_out_250, fanCoil->noi_out_500, fanCoil->noi_out_1k, fanCoil->noi_out_2k, fanCoil->noi_out_4k,
                        fanCoil->noi_out_8k,fanCoil->noi_out_total};
        }
        else if(ui->comboBox_unit_name->currentText() == "空调器")
        {
            aircondition = static_cast<Aircondition_noise*>(component);
            noises = {aircondition->noi_send_out_125, aircondition->noi_send_out_125, aircondition->noi_send_out_250, aircondition->noi_send_out_500, aircondition->noi_send_out_1k, aircondition->noi_send_out_2k, aircondition->noi_send_out_4k,
                        aircondition->noi_send_out_8k,aircondition->noi_send_out_total};
        }
    }

    for(int i = 0; i < noises.size(); i++)
    {
        noi_after_cal_lineEdits[i]->setText(noises[i]);
        noise_after_cal[i] = noi_after_cal_lineEdits[i]->text();
    }
}

void RoomCalTable::on_comboBox_atten_noi_end_size_currentTextChanged(const QString &arg1)
{
    //stackedWidget_terminal_size
    if(arg1 == "圆形")
    {
        ui->stackedWidget_terminal_size->setCurrentWidget(ui->page_circle);
    }
    else if(arg1 == "方形")
    {
        ui->stackedWidget_terminal_size->setCurrentWidget(ui->page_rect);
    }
}


void RoomCalTable::on_comboBox_pipe_elbow_type_currentTextChanged(const QString &arg1)
{
    if(arg1 == "圆形")
    {
        ui->stackedWidget_pipe_size->setCurrentWidget(ui->page_pipe_circle);
    }
    else if(arg1 == "方形")
    {
        ui->stackedWidget_pipe_size->setCurrentWidget(ui->page_pipe_rect);
    }
}

