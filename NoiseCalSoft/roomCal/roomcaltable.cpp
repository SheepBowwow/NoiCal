#include "roomcaltable.h"
#include "ui_roomcaltable.h"
#include <QContextMenuEvent>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout> // 添加此行
#include <QDebug>

RoomCalTable::RoomCalTable(QWidget *parent,QString type) :
    QWidget(parent),
    ui(new Ui::RoomCalTable)
{
    ui->setupUi(this);

    if(type == "")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_unit);
        ui->comboBox_sound_type->setCurrentText("声源噪音");
        ui->comboBox_unit_name->setCurrentIndex(-1);
        ui->stackedWidget_info->setCurrentWidget(ui->page_noi_src_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_noi_src_table);
    }
    else if(type == "声压级计算")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_room_cal);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
        ui->comboBox_room_type->setCurrentIndex(-1);
        ui->stackedWidget_info->setCurrentWidget(ui->page_room_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_table);
    }
    else
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

    if(arg1 == "风机" || arg1 == "风机" || arg1 == "风机盘管")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_noi_src_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_noi_src_table);
    }
    else if(arg1 == "阀门/变风量末端")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_vav_terminal_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_damper_table);
    }
    else if(arg1 == "阀门/圆形调风门")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_circle_damper_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_damper_table);
    }
    else if(arg1 == "阀门/方形调风门")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_rect_damper_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_damper_table);
    }
    else if(arg1 == "分支/静压箱" || arg1 == "分支/静压箱" || arg1 == "分支/静压箱")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_branch_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_branch_table);
    }
    else if(arg1 == "直管")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_pipe_elbow_info);
        ui->stackedWidget_pipe_elbow->setCurrentWidget(ui->page_pipe);
        ui->stackedWidget_table->setCurrentWidget(ui->page_pipe_table);
    }
    else if(arg1 == "弯头")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_pipe_elbow_info);
        ui->stackedWidget_pipe_elbow->setCurrentWidget(ui->page_elbow);
        ui->stackedWidget_table->setCurrentWidget(ui->page_elbow_table);
    }
    else if(arg1 == "变径")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_reducer_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_elbow_table);
    }
    else if(arg1 == "消音器")
    {
        ui->stackedWidget_info->setCurrentWidget(ui->page_silencer_info);
        ui->stackedWidget_table->setCurrentWidget(ui->page_silencer_table);
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

