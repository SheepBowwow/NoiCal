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
#include "roomDefineForm/systemmanager.h"
#include "Component/ComponentManager.h"
#include "calFunction/cal_function.h"

RoomCalTable::RoomCalTable(QString systemName, QWidget *parent,QString currentTableType) :
    QWidget(parent),
    systemName(systemName),
    isCollapsed(false),
    ui(new Ui::RoomCalTable)
{
    ui->setupUi(this);
    //connect(ui->comboBox_noi_src_num, &ClickableComboBox::clicked, this, &RoomCalTable::updateComboBoxItems);
    connect(&ComponentManager::getInstance(), &ComponentManager::componentsUpdate, this, &RoomCalTable::updateModelComboBoxItems);

    //初始化定时器
    debounceTimer.setInterval(100);  // 设置防抖时间为100毫秒
    debounceTimer.setSingleShot(true);  // 确保定时器只触发一次
    connect(&debounceTimer, &QTimer::timeout, this, &RoomCalTable::onDebouncedChange);
    this->isValid = true;
    this->currentTableType = currentTableType;

    variations.reserve(8);
    for (int i = 0; i < 8; ++i) {
        variations.push_back(QString());
    }

    noise_after_cal.reserve(9);
    noise_before_cal.reserve(9);
    for (int i = 0; i < 9; ++i) {
        noise_after_cal.push_back(QString());
        noise_before_cal.push_back(QString());
    }

    if(currentTableType == "" || currentTableType == "声源噪音")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_unit);
        ui->comboBox_sound_type->setCurrentText("声源噪音");
        ui->comboBox_unit_name->setCurrentIndex(0);
        ui->stackedWidget_table->setCurrentWidget(ui->page_fan);
        this->currentTableType = "声源噪音";
        //QList<QString> uuids =
                SystemManager::getInstance().getNoiseComponentsInSystem(systemName,noise_src_component::FAN);
//        ui->comboBox_fan_number->clear();
//        for(QString number: systemListMap[systemName]["风机"])
//        {
//            ui->comboBox_fan_number->addItem(number);
//        }
//        noi_after_cal_lineEdits = { ui->lineEdit_fan_63, ui->lineEdit_fan_125, ui->lineEdit_fan_250,ui->lineEdit_fan_500,
//                                        ui->lineEdit_fan_1k, ui->lineEdit_fan_2k, ui->lineEdit_fan_4k, ui->lineEdit_fan_8k,
//                                        ui->lineEdit_fan_total };
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

    on_comboBox_unit_name_currentTextChanged(ui->comboBox_unit_name->currentText());

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
    on_stackedWidget_table_currentChanged(ui->stackedWidget_table->currentIndex());

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

void RoomCalTable::sendTableChangedSignal()
{
    debounceTimer.stop();  // 先停止定时器，以取消之前的等待
    debounceTimer.start();  // 重新启动定时器
}

void RoomCalTable::onDebouncedChange()
{
    emit tableChanged();
}

void RoomCalTable::set_Noise_after_cal_Vector()
{
    if(noi_after_cal_lineEdits.isEmpty())
        return;

    for(QLineEdit* lineEdit : noi_after_cal_lineEdits)
    {
        if(!lineEdit || lineEdit->text() == "")
            return;
    }

    for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
    {
        noise_after_cal[i] = noi_after_cal_lineEdits[i]->text();
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

    for (QLineEdit* lineEdit : qAsConst(currentConnectedLineEdits))
    {
        if (lineEdit)
        {
            disconnect(lineEdit, &QLineEdit::textChanged, this, &RoomCalTable::sendTableChangedSignal);
        }
    }
    currentConnectedLineEdits.clear();

    QVector<QVector<QLineEdit*>*> allLineEdits = {
        &noi_lineEdits,
        &terminal_atten_lineEdits,
        &terminal_refl_lineEdits,
        &terminal_noi_lineEdits,
        &each_atten_lineEdits,
        &sum_atten_lineEdits,
        &atten_lineEdits
    };

    for (QVector<QLineEdit*>* container : qAsConst(allLineEdits))
    {
        if (!container->isEmpty())
        {  // 检查容器是否非空
            for (QLineEdit* lineEdit : *container)
            {
                connect(lineEdit, &QLineEdit::textChanged, this, &RoomCalTable::sendTableChangedSignal);
                currentConnectedLineEdits.push_back(lineEdit);
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
    // 递归查找 QLineEdit、QComboBox 和 QCheckBox
    for (QObject *child : widget->children()) {
        if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(child)) {
            // 找到 QLineEdit，清空文本
            lineEdit->clear();
        } else if (QComboBox *comboBox = qobject_cast<QComboBox*>(child)) {
            // 找到 QComboBox，清空选择
            comboBox->setCurrentIndex(-1); // 或者使用 comboBox->clear();
        } else if (QCheckBox *checkBox = qobject_cast<QCheckBox*>(child)) {
            // 找到 QCheckBox，设置为未选中
            checkBox->setChecked(false);
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

        ui->comboBox_fan_number->clear();
        for(QString number: systemListMap[systemName]["风机"])
        {
            ui->comboBox_fan_number->addItem(number);
        }
        noi_after_cal_lineEdits = { ui->lineEdit_fan_63, ui->lineEdit_fan_125, ui->lineEdit_fan_250,ui->lineEdit_fan_500,
                                        ui->lineEdit_fan_1k, ui->lineEdit_fan_2k, ui->lineEdit_fan_4k, ui->lineEdit_fan_8k,
                                        ui->lineEdit_fan_total };
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
    clearPage(ui->stackedWidget_table->currentWidget());
    if(updateModelComboBox == false)
    {
        for (QLineEdit* lineEdit : qAsConst(noi_after_cal_lineEdits))
        {
            if (lineEdit)
            {
                disconnect(lineEdit, &QLineEdit::textChanged, this, &RoomCalTable::set_Noise_after_cal_Vector);
            }
        }
        clearLineEditsVectors();
        noi_after_cal_lineEdits.clear();
    }
    //不管是否为更新comboBox，都要更新component列表
    this->currentAirconditionList_exhaust.clear();
    this->currentComponentList.clear();
    this->currentComponent = QSharedPointer<ComponentBase>();
    if(arg1 == "风机")
    {
        ui->comboBox_fan_number->clear();

        QList<QString> list =
                SystemManager::getInstance().getNoiseComponentsInSystem(systemName,noise_src_component::FAN);
        for(auto& uuid : list)
        {
            auto component = ComponentManager::getInstance().findComponent(uuid);
            if(auto fan = dynamic_cast<Fan*>(component.data()))
            {
                ui->comboBox_fan_number->addItem(fan->number);
                this->currentComponentList.append(component);
            }
        }

        noi_after_cal_lineEdits = { ui->lineEdit_fan_63, ui->lineEdit_fan_125, ui->lineEdit_fan_250,ui->lineEdit_fan_500,
                                        ui->lineEdit_fan_1k, ui->lineEdit_fan_2k, ui->lineEdit_fan_4k, ui->lineEdit_fan_8k,
                                        ui->lineEdit_fan_total };

        ui->stackedWidget_table->setCurrentWidget(ui->page_fan);
    }
    else if(arg1 == "空调器")
    {
        ui->comboBox_aircondition_number->clear();
        ui->comboBox_aircondition_fan_type->setCurrentIndex(-1);

        noi_after_cal_lineEdits = { ui->lineEdit_aircondition_63, ui->lineEdit_aircondition_125, ui->lineEdit_aircondition_250,ui->lineEdit_aircondition_500,
                                        ui->lineEdit_aircondition_1k, ui->lineEdit_aircondition_2k, ui->lineEdit_aircondition_4k, ui->lineEdit_aircondition_8k,
                                        ui->lineEdit_aircondition_total };

        QList<QString> list_send =
                SystemManager::getInstance().getNoiseComponentsInSystem(systemName,noise_src_component::AIRCONDITION_SEND);
        for(auto& uuid : list_send)
        {
            auto component = ComponentManager::getInstance().findComponent(uuid);
            if(auto aircondition = dynamic_cast<Aircondition*>(component.data()))
            {
                this->currentComponentList.append(component);
            }
        }
        QList<QString> list_exhaust =
                SystemManager::getInstance().getNoiseComponentsInSystem(systemName,noise_src_component::AIRCONDITION_EXHAUST);
        for(auto& uuid : list_exhaust)
        {
            auto component = ComponentManager::getInstance().findComponent(uuid);
            if(auto aircondition = dynamic_cast<Aircondition*>(component.data()))
            {
                this->currentAirconditionList_exhaust.append(component);
            }
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_aircondition);
    }
    else if(arg1 == "风机盘管")
    {
        ui->comboBox_fanCoil_model->clear();


        QList<QString> list =
                SystemManager::getInstance().getNoiseComponentsInSystem(systemName,noise_src_component::FANCOIL);
        for(auto& uuid : list)
        {
            auto component = ComponentManager::getInstance().findComponent(uuid);
            if(auto fanCoil = dynamic_cast<FanCoil*>(component.data()))
            {
                ui->comboBox_fanCoil_model->addItem(fanCoil->model);
                this->currentComponentList.append(component);
            }
        }

        noi_after_cal_lineEdits = { ui->lineEdit_fanCoil_63, ui->lineEdit_fanCoil_125, ui->lineEdit_fanCoil_250,ui->lineEdit_fanCoil_500,
                                        ui->lineEdit_fanCoil_1k, ui->lineEdit_fanCoil_2k, ui->lineEdit_fanCoil_4k, ui->lineEdit_fanCoil_8k,
                                        ui->lineEdit_fanCoil_total };

        ui->stackedWidget_table->setCurrentWidget(ui->page_fanCoil);
    }
    else if(arg1 == "阀门/变风量末端")
    {
        ui->comboBox_VAV_terminal_number->clear();
        noi_after_cal_lineEdits = { ui->lineEdit_VAV_terminal_after_63, ui->lineEdit_VAV_terminal_after_125, ui->lineEdit_VAV_terminal_after_250,ui->lineEdit_VAV_terminal_after_500,
                                        ui->lineEdit_VAV_terminal_after_1k, ui->lineEdit_VAV_terminal_after_2k, ui->lineEdit_VAV_terminal_after_4k, ui->lineEdit_VAV_terminal_after_8k,
                                        ui->lineEdit_VAV_terminal_after_total};

        noi_lineEdits = { ui->lineEdit_VAV_terminal_63, ui->lineEdit_VAV_terminal_125, ui->lineEdit_VAV_terminal_250,ui->lineEdit_VAV_terminal_500,
                          ui->lineEdit_VAV_terminal_1k, ui->lineEdit_VAV_terminal_2k, ui->lineEdit_VAV_terminal_4k, ui->lineEdit_VAV_terminal_8k,
                          ui->lineEdit_VAV_terminal_total};


        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::VAV_TERMINAL);

        for(auto& component : currentComponentList)
        {
            if(auto vav_terminal = dynamic_cast<VAV_terminal*>(component.data()))
            {
                ui->comboBox_VAV_terminal_number->addItem(vav_terminal->number);
            }
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_VAV_terminal);

    }
    else if(arg1 == "阀门/圆形调风门")
    {
        noi_after_cal_lineEdits = { ui->lineEdit_circular_damper_after_63, ui->lineEdit_circular_damper_after_125, ui->lineEdit_circular_damper_after_250,ui->lineEdit_circular_damper_after_500,
                                        ui->lineEdit_circular_damper_after_1k, ui->lineEdit_circular_damper_after_2k, ui->lineEdit_circular_damper_after_4k, ui->lineEdit_circular_damper_after_8k,
                                        ui->lineEdit_circular_damper_after_total};

        noi_lineEdits = { ui->lineEdit_circular_damper_63, ui->lineEdit_circular_damper_125, ui->lineEdit_circular_damper_250,ui->lineEdit_circular_damper_500,
                          ui->lineEdit_circular_damper_1k, ui->lineEdit_circular_damper_2k, ui->lineEdit_circular_damper_4k, ui->lineEdit_circular_damper_8k,
                          ui->lineEdit_circular_damper_total};


        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::CIRCULAR_DAMPER);

        for(auto& component : currentComponentList)
        {
            if(auto circular_damper = dynamic_cast<Circular_damper*>(component.data()))
            {
                ui->comboBox_circular_damper_model->addItem(circular_damper->model);
            }
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_circular_damper);
    }
    else if(arg1 == "阀门/方形调风门")
    {
        noi_after_cal_lineEdits = { ui->lineEdit_rect_damper_after_63, ui->lineEdit_rect_damper_after_125, ui->lineEdit_rect_damper_after_250,ui->lineEdit_rect_damper_after_500,
                                        ui->lineEdit_rect_damper_after_1k, ui->lineEdit_rect_damper_after_2k, ui->lineEdit_rect_damper_after_4k, ui->lineEdit_rect_damper_after_8k,
                                        ui->lineEdit_rect_damper_after_total};

        noi_lineEdits = { ui->lineEdit_rect_damper_63, ui->lineEdit_rect_damper_125, ui->lineEdit_rect_damper_250,ui->lineEdit_rect_damper_500,
                          ui->lineEdit_rect_damper_1k, ui->lineEdit_rect_damper_2k, ui->lineEdit_rect_damper_4k, ui->lineEdit_rect_damper_8k,
                          ui->lineEdit_rect_damper_total};


        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::RECT_DAMPER);

        for(auto& component : currentComponentList)
        {
            if(auto rect_damper = dynamic_cast<Rect_damper*>(component.data()))
            {
                ui->comboBox_rect_damper_model->addItem(rect_damper->model);
            }
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_rect_damper);
    }
    else if(arg1 == "末端/布风器+散流器")
    {
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


        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::AIRDIFF);

        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto air_diff = dynamic_cast<AirDiff*>(component.data()))
            {
                set.insert(air_diff->air_distributor_model);
            }
        }

        for(QString air_distributor_model : set)
        {
            ui->comboBox_air_distributor_model->addItem(air_distributor_model);
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_air_diff);
    }
    else if(arg1 == "末端/抽风头")
    {
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

        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::PUMPSEND);

        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto pump = dynamic_cast<PumpSend*>(component.data()))
            {
                if(pump->type_pump_or_send == pump_send_type::PUMP)
                    set.insert(pump->model);
            }
        }

        for(QString pump_model : set)
        {
            ui->comboBox_pump_model->addItem(pump_model);
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_pump);
    }
    else if(arg1 == "末端/送风头")
    {
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

        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::PUMPSEND);
        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto send = dynamic_cast<PumpSend*>(component.data()))
            {
                if(send->type_pump_or_send == pump_send_type::SEND)
                    set.insert(send->model);
            }
        }

        for(QString send_model : set)
        {
            ui->comboBox_send_model->addItem(send_model);
        }
        ui->stackedWidget_table->setCurrentWidget(ui->page_send);
    }
    else if(arg1 == "末端/静压箱+格栅")
    {
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

        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::STATICBOX_GRILLE);
        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto staticBox_grille = dynamic_cast<StaticBox_grille*>(component.data()))
            {
                set.insert(staticBox_grille->staticBox_model);
            }
        }

        for(QString staticBox_model : set)
        {
            ui->comboBox_staticBox_model->addItem(staticBox_model);
        }
        ui->stackedWidget_table->setCurrentWidget(ui->page_staticBox_grille);
    }
    else if(arg1 == "末端/置换通风末端")
    {
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

        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::DISP_VENT_TERMINAL);
        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto disp_vent = dynamic_cast<Disp_vent_terminal*>(component.data()))
            {
                    set.insert(disp_vent->model);
            }
        }

        for(QString disp_vent_model : set)
        {
            ui->comboBox_disp_vent_terminal_model->addItem(disp_vent_model);
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_disp_vent_terminal);
    }
    else if(arg1 == "末端/其他通风末端")
    {
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

        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::OTHER_SEND_TERMINAL);
        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto other_send = dynamic_cast<Other_send_terminal*>(component.data()))
            {
                    set.insert(other_send->model);
            }
        }

        for(QString other_send_model : set)
        {
            ui->comboBox_other_send_terminal_model->addItem(other_send_model);
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_other_send_terminal);
    }
    else if(arg1 == "分支/静压箱")
    {
        noi_after_cal_lineEdits = { ui->lineEdit_static_box_after_63, ui->lineEdit_static_box_after_125, ui->lineEdit_static_box_after_250,ui->lineEdit_static_box_after_500,
                                        ui->lineEdit_static_box_after_1k, ui->lineEdit_static_box_after_2k, ui->lineEdit_static_box_after_4k, ui->lineEdit_static_box_after_8k,
                                        ui->lineEdit_static_box_after_total};

        atten_lineEdits = { ui->lineEdit_static_box_63, ui->lineEdit_static_box_125, ui->lineEdit_static_box_250,ui->lineEdit_static_box_500,
                            ui->lineEdit_static_box_1k, ui->lineEdit_static_box_2k, ui->lineEdit_static_box_4k, ui->lineEdit_static_box_8k,
                            ui->lineEdit_static_box_total};

        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::STATICBOX);
        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto static_box = dynamic_cast<Static_box*>(component.data()))
            {
                    set.insert(static_box->model);
            }
        }

        for(QString static_box_model : set)
        {
            ui->comboBox_static_box_model->addItem(static_box_model);
        }
        ui->stackedWidget_table->setCurrentWidget(ui->page_static_box);
    }
    else if(arg1 == "分支/风道多分支")
    {
        noi_after_cal_lineEdits = { ui->lineEdit_multi_ranc_after_63, ui->lineEdit_multi_ranc_after_125, ui->lineEdit_multi_ranc_after_250,ui->lineEdit_multi_ranc_after_500,
                                        ui->lineEdit_multi_ranc_after_1k, ui->lineEdit_multi_ranc_after_2k, ui->lineEdit_multi_ranc_after_4k, ui->lineEdit_multi_ranc_after_8k,
                                        ui->lineEdit_multi_ranc_after_total};

        atten_lineEdits = { ui->lineEdit_multi_ranc_63, ui->lineEdit_multi_ranc_125, ui->lineEdit_multi_ranc_250,ui->lineEdit_multi_ranc_500,
                            ui->lineEdit_multi_ranc_1k, ui->lineEdit_multi_ranc_2k, ui->lineEdit_multi_ranc_4k, ui->lineEdit_multi_ranc_8k,
                            ui->lineEdit_multi_ranc_total};

        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::MULTI_RANC);
        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto multi_ranc = dynamic_cast<Multi_ranc*>(component.data()))
            {
                    set.insert(multi_ranc->model);
            }
        }

        for(QString multi_ranc_model : set)
        {
            ui->comboBox_multi_ranc_model->addItem(multi_ranc_model);
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_multi_ranc);
    }
    else if(arg1 == "分支/三通")
    {
        noi_after_cal_lineEdits = { ui->lineEdit_tee_after_63, ui->lineEdit_tee_after_125, ui->lineEdit_tee_after_250,ui->lineEdit_tee_after_500,
                                        ui->lineEdit_tee_after_1k, ui->lineEdit_tee_after_2k, ui->lineEdit_tee_after_4k, ui->lineEdit_tee_after_8k,
                                        ui->lineEdit_tee_after_total};

        atten_lineEdits = { ui->lineEdit_tee_63, ui->lineEdit_tee_125, ui->lineEdit_tee_250,ui->lineEdit_tee_500,
                            ui->lineEdit_tee_1k, ui->lineEdit_tee_2k, ui->lineEdit_tee_4k, ui->lineEdit_tee_8k,
                            ui->lineEdit_tee_total};


        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::TEE);
        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto tee = dynamic_cast<Tee*>(component.data()))
            {
                set.insert(tee->model);
            }
        }

        for(QString tee_model : set)
        {
            ui->comboBox_tee_model->addItem(tee_model);
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_tee);
    }
    else if(arg1 == "直管")
    {
        noi_after_cal_lineEdits = { ui->lineEdit_pipe_after_63, ui->lineEdit_pipe_after_125, ui->lineEdit_pipe_after_250,ui->lineEdit_pipe_after_500,
                                        ui->lineEdit_pipe_after_1k, ui->lineEdit_pipe_after_2k, ui->lineEdit_pipe_after_4k, ui->lineEdit_pipe_after_8k,
                                        ui->lineEdit_pipe_after_total};

        each_atten_lineEdits = { ui->lineEdit_pipe_63, ui->lineEdit_pipe_125, ui->lineEdit_pipe_250,ui->lineEdit_pipe_500,
                                        ui->lineEdit_pipe_1k, ui->lineEdit_pipe_2k, ui->lineEdit_pipe_4k, ui->lineEdit_pipe_8k,
                                        ui->lineEdit_pipe_total};

        sum_atten_lineEdits = { ui->lineEdit_pipe_sum_63, ui->lineEdit_pipe_sum_125, ui->lineEdit_pipe_sum_250,ui->lineEdit_pipe_sum_500,
                                ui->lineEdit_pipe_sum_1k, ui->lineEdit_pipe_sum_2k, ui->lineEdit_pipe_sum_4k, ui->lineEdit_pipe_sum_8k,
                                ui->lineEdit_pipe_sum_total};


        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::PIPE);
        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto pipe = dynamic_cast<Pipe*>(component.data()))
            {
                set.insert(pipe->model);
            }
        }

        for(QString pipe_model : set)
        {
            ui->comboBox_pipe_model->addItem(pipe_model);
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_pipe);
    }
    else if(arg1 == "弯头")
    {
        noi_after_cal_lineEdits = { ui->lineEdit_elbow_after_63, ui->lineEdit_elbow_after_125, ui->lineEdit_elbow_after_250,ui->lineEdit_elbow_after_500,
                                        ui->lineEdit_elbow_after_1k, ui->lineEdit_elbow_after_2k, ui->lineEdit_elbow_after_4k, ui->lineEdit_elbow_after_8k,
                                        ui->lineEdit_elbow_after_total};

        each_atten_lineEdits = { ui->lineEdit_elbow_63, ui->lineEdit_elbow_125, ui->lineEdit_elbow_250,ui->lineEdit_elbow_500,
                                        ui->lineEdit_elbow_1k, ui->lineEdit_elbow_2k, ui->lineEdit_elbow_4k, ui->lineEdit_elbow_8k,
                                        ui->lineEdit_elbow_total};

        sum_atten_lineEdits = { ui->lineEdit_elbow_sum_63, ui->lineEdit_elbow_sum_125, ui->lineEdit_elbow_sum_250,ui->lineEdit_elbow_sum_500,
                                ui->lineEdit_elbow_sum_1k, ui->lineEdit_elbow_sum_2k, ui->lineEdit_elbow_sum_4k, ui->lineEdit_elbow_sum_8k,
                                ui->lineEdit_elbow_sum_total};

        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::ELBOW);
        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto elbow = dynamic_cast<Elbow*>(component.data()))
            {
                set.insert(elbow->model);
            }
        }

        for(QString elbow_model : set)
        {
            ui->comboBox_elbow_model->addItem(elbow_model);
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_elbow);
    }
    else if(arg1 == "变径")
    {
        noi_after_cal_lineEdits = { ui->lineEdit_reducer_after_63, ui->lineEdit_reducer_after_125, ui->lineEdit_reducer_after_250,ui->lineEdit_reducer_after_500,
                                        ui->lineEdit_reducer_after_1k, ui->lineEdit_reducer_after_2k, ui->lineEdit_reducer_after_4k, ui->lineEdit_reducer_after_8k,
                                        ui->lineEdit_reducer_after_total};

        atten_lineEdits = { ui->lineEdit_reducer_63, ui->lineEdit_reducer_125, ui->lineEdit_reducer_250,ui->lineEdit_reducer_500,
                                        ui->lineEdit_reducer_1k, ui->lineEdit_reducer_2k, ui->lineEdit_reducer_4k, ui->lineEdit_reducer_8k,
                                        ui->lineEdit_reducer_total};

        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::REDUCER);
        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto reducer = dynamic_cast<Reducer*>(component.data()))
            {
                set.insert(reducer->model);
            }
        }

        for(QString reducer_model : set)
        {
            ui->comboBox_reducer_model->addItem(reducer_model);
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_reducer);
    }
    else if(arg1 == "消音器")
    {
        noi_after_cal_lineEdits = { ui->lineEdit_silencer_after_63, ui->lineEdit_silencer_after_125, ui->lineEdit_silencer_after_250,ui->lineEdit_silencer_after_500,
                                        ui->lineEdit_silencer_after_1k, ui->lineEdit_silencer_after_2k, ui->lineEdit_silencer_after_4k, ui->lineEdit_silencer_after_8k, nullptr};

        atten_lineEdits = { ui->lineEdit_silencer_63, ui->lineEdit_silencer_125, ui->lineEdit_silencer_250,ui->lineEdit_silencer_500,
                                        ui->lineEdit_silencer_1k, ui->lineEdit_silencer_2k, ui->lineEdit_silencer_4k, ui->lineEdit_silencer_8k, nullptr};

        this->currentComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::SILENCER);
        //用set去重
        QSet<QString> set;
        for(auto& component : currentComponentList)
        {
            if(auto silencer = dynamic_cast<Silencer*>(component.data()))
            {
                set.insert(silencer->model);
            }
        }

        for(QString silencer_model : set)
        {
            ui->comboBox_reducer_model->addItem(silencer_model);
        }

        ui->stackedWidget_table->setCurrentWidget(ui->page_silencer);
    }

    if(updateModelComboBox == false)
    {
        for (QLineEdit* lineEdit : qAsConst(noi_after_cal_lineEdits))
        {
            if (lineEdit)
            {
                connect(lineEdit, &QLineEdit::textChanged, this, &RoomCalTable::set_Noise_after_cal_Vector);
            }
        }
        clearPage(ui->stackedWidget_table->currentWidget());
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

/**
 * @brief RoomCalTable::updateComboBoxItems
 * 用于处理如果1~4部分录入，在这个界面更新, 如果修改的部件的uuid和当前的一样
 */
void RoomCalTable::updateModelComboBoxItems(const QString &uuid) {
    QSharedPointer<ComponentBase> changedComponent = ComponentManager::getInstance().findComponent(uuid);
    bool isTypeSame = (changedComponent->typeName() == currentComponent->typeName());
    auto it = std::find_if(currentComponentList.begin(),currentComponentList.end(), [&](QSharedPointer<ComponentBase> component){
            return uuid == component->UUID;
    });

    //uuid等于当前的说明当前的部件被修改或删除了
    if(uuid == currentComponent->UUID && (isTypeSame && it == currentComponentList.end()))
    {
        updateModelComboBox = true;
        on_comboBox_unit_name_currentTextChanged(ui->comboBox_unit_name->currentText());
        updateModelComboBox = false;
    }
}

/**
 * @brief RoomCalTable::clearQVectors
 * 清除所有lineEdit容器
 */
void RoomCalTable::clearLineEditsVectors()
{
    noi_lineEdits.clear();
    terminal_atten_lineEdits.clear();
    terminal_refl_lineEdits.clear();
    terminal_noi_lineEdits.clear();
    each_atten_lineEdits.clear();
    sum_atten_lineEdits.clear();
    atten_lineEdits.clear();
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

/**
 * @brief RoomCalTable::connectLineEditsToCalSlot
 * 将传入容器中的所有lineEdit连接计算变化量的函数
 * @param lineEdits
 */
void RoomCalTable::connectLineEditsToCalSlot(const QVector<QLineEdit*>& lineEdits) {
    for (QLineEdit* lineEdit : lineEdits) {
        // 确保 lineEdit 非空后再连接
        if (lineEdit) {
            connect(lineEdit, &QLineEdit::textChanged, this, &RoomCalTable::calVariations);
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
                variations = QVector<QString>(8,QString());
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
                variations = QVector<QString>(8,QString());
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
                variations = QVector<QString>(8,QString());
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
                variations = QVector<QString>(8,QString());
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

    std::array<QLineEdit*, 9> lineEdits{0};
    for(int i = 0; i < 8; i++)
    {
        if(variations[i].isEmpty())
        {
            return;
        }
        noi_after_cal_lineEdits[i]->setText(QString::number(variations[i].toDouble() + noise_before_cal[i].toDouble(),'f',1));
        lineEdits[i] = noi_after_cal_lineEdits[i];
    }
    noi_after_cal_lineEdits[8]->setText(QString::number(calNoiseTotalValue(lineEdits),'f' ,1)); //总值
}

//风机编号选择
void RoomCalTable::on_comboBox_fan_number_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_fan_air_volume->clear();
    ui->lineEdit_fan_model->clear();
    ui->lineEdit_fan_static_press->clear();
    auto it = std::find_if(this->currentComponentList.begin(),this->currentComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto fan = dynamic_cast<Fan*>(component.data());
        if(fan && fan->number == arg1)
            return true;
        return false;
    });

    if(it != currentComponentList.end())
    {
        if(auto fan = dynamic_cast<Fan*>((*it).data()))
        {
            currentComponent = (*it);
            ui->lineEdit_fan_air_volume->setText(fan->air_volume);
            ui->lineEdit_fan_model->setText(fan->model);
            ui->lineEdit_fan_static_press->setText(fan->static_pressure);
        }
    }
    on_comboBox_fan_noise_locate_currentIndexChanged(ui->comboBox_fan_noise_locate->currentIndex());
}

void RoomCalTable::on_comboBox_fan_noise_locate_currentIndexChanged(int index)
{
    if(auto fan = dynamic_cast<Fan*>(currentComponent.data()))
    {
        if(index == 0)
        {
            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                noi_after_cal_lineEdits[i]->setText(fan->noi_in[i]);
            }
        }
        else if(index == 1)
        {
            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                noi_after_cal_lineEdits[i]->setText(fan->noi_out[i]);
            }
        }
    }
}

//风机盘管型号选择
void RoomCalTable::on_comboBox_fanCoil_model_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_fanCoil_air_volume->clear();
    ui->lineEdit_fanCoil_static_press->clear();
    auto it = std::find_if(this->currentComponentList.begin(),this->currentComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto fanCoil = dynamic_cast<FanCoil*>(component.data());
        if(fanCoil && fanCoil->model == arg1)
            return true;
        return false;
    });

    if(it != currentComponentList.end())
    {
        if(auto fanCoil = dynamic_cast<FanCoil*>((*it).data()))
        {
            currentComponent = (*it);
            ui->lineEdit_fanCoil_air_volume->setText(fanCoil->air_volume);
            ui->lineEdit_fanCoil_static_press->setText(fanCoil->static_pressure);
        }
    }
    on_comboBox_fanCoil_locate_currentIndexChanged(ui->comboBox_fanCoil_locate->currentIndex());
}

void RoomCalTable::on_comboBox_fanCoil_locate_currentIndexChanged(int index)
{
    if(auto fanCoil = dynamic_cast<FanCoil*>(currentComponent.data()))
    {
        if(index == 0)
        {
            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                noi_after_cal_lineEdits[i]->setText(fanCoil->noi_in[i]);
            }
        }
        else if(index == 1)
        {
            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                noi_after_cal_lineEdits[i]->setText(fanCoil->noi_out[i]);
            }
        }
    }
}

//空调器风机类型选择
void RoomCalTable::on_comboBox_aircondition_fan_type_currentIndexChanged(int index)
{
    ui->comboBox_aircondition_number->clear();

    //送风机
    if(index == 0)
    {
        for(auto& component : this->currentComponentList)
        {
            if(auto aircondition = dynamic_cast<Aircondition*>(component.data()))
            {
                ui->comboBox_aircondition_number->addItem(aircondition->send_number);
            }
        }
    }
    else if(index == 1)
    {
        for(auto& component : this->currentAirconditionList_exhaust)
        {
            if(auto aircondition = dynamic_cast<Aircondition*>(component.data()))
            {
                ui->comboBox_aircondition_number->addItem(aircondition->exhaust_number);
            }
        }
    }

    on_comboBox_aircondition_noise_locate_currentIndexChanged(ui->comboBox_aircondition_noise_locate->currentIndex());
}

//空调器编号选择
void RoomCalTable::on_comboBox_aircondition_number_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_aircondition_air_volume->clear();
    ui->lineEdit_aircondition_model->clear();
    ui->lineEdit_aircondition_static_press->clear();

    int fan_type_index = ui->comboBox_aircondition_fan_type->currentIndex();

    if(fan_type_index == 0)
    {
        auto it = std::find_if(this->currentComponentList.begin(),this->currentComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
            auto aircondition = dynamic_cast<Aircondition*>(component.data());
            if(aircondition)
            {
                return aircondition->send_number == arg1;
            }
            return false;
        });
        if(it != currentComponentList.end())
        {
            if(auto aircondition = dynamic_cast<Aircondition*>((*it).data()))
            {
                currentComponent = (*it);
                ui->lineEdit_aircondition_air_volume->setText(aircondition->send_air_volume);
                ui->lineEdit_aircondition_model->setText(aircondition->model);
                ui->lineEdit_aircondition_static_press->setText(aircondition->send_static_pressure);
            }
        }
    }
    else if(fan_type_index == 1)
    {
        auto it = std::find_if(this->currentAirconditionList_exhaust.begin(),this->currentAirconditionList_exhaust.end(), [&](const QSharedPointer<ComponentBase>& component){
            auto aircondition = dynamic_cast<Aircondition*>(component.data());
            if(aircondition)
            {
                return aircondition->exhaust_number == arg1;
            }
            return false;
        });
        if(it != currentAirconditionList_exhaust.end())
        {
            if(auto aircondition = dynamic_cast<Aircondition*>((*it).data()))
            {
                currentComponent = (*it);
                ui->lineEdit_aircondition_air_volume->setText(aircondition->exhaust_air_volume);
                ui->lineEdit_aircondition_model->setText(aircondition->model);
                ui->lineEdit_aircondition_static_press->setText(aircondition->exhaust_static_pressure);
            }
        }
    }


    on_comboBox_aircondition_noise_locate_currentIndexChanged(ui->comboBox_aircondition_noise_locate->currentIndex());
}

void RoomCalTable::on_comboBox_aircondition_noise_locate_currentIndexChanged(int index)
{
    for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
    {
        noi_after_cal_lineEdits[i]->clear();
    }
    int fan_type_index = ui->comboBox_aircondition_fan_type->currentIndex();
    if(currentComponent)
    {
        if(fan_type_index == 0)
        {
            auto it = std::find_if(this->currentComponentList.begin(),this->currentComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
                auto aircondition = dynamic_cast<Aircondition*>(component.data());
                if(aircondition)
                {
                    return aircondition->UUID == currentComponent->UUID;
                }
                return false;
            });
            if(it == currentComponentList.end())
            {
                return;
            }
        }
        else
        {
            auto it = std::find_if(this->currentAirconditionList_exhaust.begin(),this->currentAirconditionList_exhaust.end(), [&](const QSharedPointer<ComponentBase>& component){
                auto aircondition = dynamic_cast<Aircondition*>(component.data());
                if(aircondition)
                {
                    return aircondition->UUID == currentComponent->UUID;
                }
                return false;
            });
            if(it == currentAirconditionList_exhaust.end())
            {
                return;
            }
        }
    }
    if(auto aircondition = dynamic_cast<Aircondition*>(currentComponent.data()))
    {
        if(index == 0)
        {

            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                noi_after_cal_lineEdits[i]->setText(
                            fan_type_index == 0 ? aircondition->noi_send_in[i] : aircondition->noi_exhaust_in[i]);
            }
        }
        else if(index == 1)
        {
            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                noi_after_cal_lineEdits[i]->setText(
                            fan_type_index == 0 ? aircondition->noi_send_out[i] : aircondition->noi_exhaust_out[i]);
            }
        }
    }

}

void RoomCalTable::on_comboBox_VAV_terminal_number_currentTextChanged(const QString &arg1)
{
    for(int i = 0; i < noi_lineEdits.size(); i++)
    {
        noi_lineEdits[i]->clear();
    }
    ui->lineEdit_VAV_terminal_model->clear();
    ui->lineEdit_VAV_terminal_air_volume->clear();
    ui->lineEdit_VAV_terminal_angle->clear();
    auto it = std::find_if(this->currentComponentList.begin(),this->currentComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto vav = dynamic_cast<VAV_terminal*>(component.data());
        if(vav && vav->number == arg1)
            return true;
        return false;
    });

    if(it != currentComponentList.end())
    {
        if(auto vav = dynamic_cast<VAV_terminal*>((*it).data()))
        {
            currentComponent = (*it);
            ui->lineEdit_VAV_terminal_air_volume->setText(vav->air_volume);
            ui->lineEdit_VAV_terminal_model->setText(vav->model);
            ui->lineEdit_VAV_terminal_angle->setText(vav->angle);

            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                noi_lineEdits[i]->setText(vav->noi[i]);
            }
        }
    }
}

//圆形调风门直径选择后
void RoomCalTable::on_comboBox_circular_damper_diameter_currentTextChanged(const QString &arg1)
{
    if(this->currentComponentList.isEmpty())
        return;

    // 断开信号与槽的连接
    disconnect(ui->comboBox_circular_damper_angle, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_comboBox_circular_damper_angle_currentTextChanged(const QString &)));


    ui->comboBox_circular_damper_angle->clear();

    QSet<QString> set;

    for(auto& component : currentComponentList)
    {
        const Circular_damper* circular_damper = dynamic_cast<const Circular_damper*>(component.data());
        if(circular_damper->model == ui->comboBox_circular_damper_diameter->currentText())
        {
            set.insert(circular_damper->angle);
        }
    }

    // 转换为数字列表，同时移除度数符号
    QList<int> numbers;
    for(QString model : set) {
        bool ok;
        int number = model.remove("°").toInt(&ok);
        if(ok) { // 确保转换成功
            numbers.append(number);
        }
    }

    // 对数字列表进行排序
    std::sort(numbers.begin(), numbers.end());

    for(int number : numbers)
    {
        // 将数字转换回QString并添加度数符号，然后添加到comboBox
        ui->comboBox_circular_damper_angle->addItem(QString::number(number) + "°");
    }

    ui->comboBox_circular_damper_angle->setCurrentIndex(-1);

    connect(ui->comboBox_circular_damper_angle, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_comboBox_circular_damper_angle_currentTextChanged(const QString &)));
}

//圆形调风门阀门开度选择后
void RoomCalTable::on_comboBox_circular_damper_angle_currentTextChanged(const QString &arg1)
{
    if(this->currentComponentList.isEmpty())
        return;

    // 断开信号与槽的连接
    disconnect(ui->comboBox_circular_damper_air_volume, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_comboBox_circular_damper_air_volume_currentTextChanged(const QString &)));

    ui->comboBox_circular_damper_air_volume->clear();

    QList<int> numbers;

    for(auto& component : currentComponentList)
    {
        const Circular_damper* circular_damper = dynamic_cast<const Circular_damper*>(component.data());
        if(circular_damper->model == ui->comboBox_circular_damper_diameter->currentText()
                && circular_damper->angle == ui->comboBox_circular_damper_angle->currentText())
        {
            bool ok;
            int number = circular_damper->air_volume.toInt(&ok);
            if(ok) { // 确保转换成功
                numbers.append(number);
            }
        }
    }

    // 对数字列表进行排序
    std::sort(numbers.begin(), numbers.end());

    for(int number : numbers)
    {
        ui->comboBox_circular_damper_angle->addItem(QString::number(number));
    }

    ui->comboBox_circular_damper_air_volume->setCurrentIndex(-1);

    connect(ui->comboBox_circular_damper_air_volume, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_comboBox_circular_damper_air_volume_currentTextChanged(const QString &)));
}

//选择圆形调风门风量,得出噪声量
void RoomCalTable::on_comboBox_circular_damper_air_volume_currentTextChanged(const QString &arg1)
{
    if(this->currentComponentList.isEmpty())
        return;

    QVector<QString> circular_damperNoise;

    for(auto& component : currentComponentList)
    {
        const Circular_damper* circular_damper = dynamic_cast<const Circular_damper*>(component.data());
        if(circular_damper->model == ui->comboBox_circular_damper_diameter->currentText()
                && circular_damper->angle == ui->comboBox_circular_damper_angle->currentText()
                && circular_damper->air_volume == ui->comboBox_circular_damper_air_volume->currentText())
        {

            for(int i = 0; i < 9; i++)
            {
                noi_lineEdits[i]->setText(circular_damper->noi[i]);
            }
        }
    }
}

//选择方形调风门尺寸
void RoomCalTable::on_comboBox_rect_damper_size_currentTextChanged(const QString &arg1)
{
    if(this->currentComponentList.isEmpty())
        return;

    // 断开信号与槽的连接
    disconnect(ui->comboBox_rect_damper_angle, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_comboBox_rect_damper_angle_currentTextChanged(const QString &)));


    ui->comboBox_rect_damper_angle->clear();

    QSet<QString> set;

    for(auto& component : currentComponentList)
    {
        const Rect_damper* rect_damper = dynamic_cast<const Rect_damper*>(component.data());
        if(rect_damper->model == ui->comboBox_rect_damper_size->currentText())
        {
            set.insert(rect_damper->angle);
        }
    }

    // 转换为数字列表，同时移除度数符号
    QList<int> numbers;
    for(QString model : set) {
        bool ok;
        int number = model.remove("°").toInt(&ok);
        if(ok) { // 确保转换成功
            numbers.append(number);
        }
    }

    // 对数字列表进行排序
    std::sort(numbers.begin(), numbers.end());

    for(int number : numbers)
    {
        // 将数字转换回QString并添加度数符号，然后添加到comboBox
        ui->comboBox_rect_damper_angle->addItem(QString::number(number) + "°");
    }

    ui->comboBox_rect_damper_angle->setCurrentIndex(-1);

    connect(ui->comboBox_rect_damper_angle, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_comboBox_rect_damper_angle_currentTextChanged(const QString &)));
}

//方形调风门阀门开度
void RoomCalTable::on_comboBox_rect_damper_angle_currentTextChanged(const QString &arg1)
{
    if(this->currentComponentList.isEmpty())
        return;

    // 断开信号与槽的连接
    disconnect(ui->comboBox_rect_damper_air_volume, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_comboBox_rect_damper_air_volume_currentTextChanged(const QString &)));

    ui->comboBox_rect_damper_air_volume->clear();

    QList<int> numbers;

    for(auto& component : currentComponentList)
    {
        const Rect_damper* rect_damper = dynamic_cast<const Rect_damper*>(component.data());
        if(rect_damper->model == ui->comboBox_rect_damper_size->currentText()
                && rect_damper->angle == ui->comboBox_rect_damper_angle->currentText())
        {
            bool ok;
            int number = rect_damper->air_volume.toInt(&ok);
            if(ok) { // 确保转换成功
                numbers.append(number);
            }
        }
    }

    // 对数字列表进行排序
    std::sort(numbers.begin(), numbers.end());

    for(int number : numbers)
    {
        ui->comboBox_rect_damper_angle->addItem(QString::number(number));
    }

    ui->comboBox_rect_damper_air_volume->setCurrentIndex(-1);

    connect(ui->comboBox_rect_damper_air_volume, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_comboBox_rect_damper_air_volume_currentTextChanged(const QString &)));
}

//方形调风门风量
void RoomCalTable::on_comboBox_rect_damper_air_volume_currentTextChanged(const QString &arg1)
{
    if(this->currentComponentList.isEmpty())
        return;

    QVector<QString> rect_damperNoise;

    for(auto& component : currentComponentList)
    {
        const Rect_damper* rect_damper = dynamic_cast<const Rect_damper*>(component.data());
        if(rect_damper->model == ui->comboBox_rect_damper_size->currentText()
                && rect_damper->angle == ui->comboBox_rect_damper_angle->currentText()
                && rect_damper->air_volume == ui->comboBox_rect_damper_air_volume->currentText())
        {

            for(int i = 0; i < 9; i++)
            {
                noi_lineEdits[i]->setText(rect_damper->noi[i]);
            }
        }
    }
}

//选择完布风器型号,添加散流器型号
void RoomCalTable::on_comboBox_air_distributor_model_currentTextChanged(const QString &arg1)
{
    if(this->currentComponentList.isEmpty())
        return;

    ui->comboBox_diffuser_model->clear();

    for(auto& component : currentComponentList)
    {
        const AirDiff* air_diff = dynamic_cast<const AirDiff*>(component.data());
        if(air_diff->air_distributor_model == ui->comboBox_air_distributor_model->currentText())
        {
            ui->comboBox_diffuser_model->addItem(air_diff->diffuser_model);
        }
    }

    ui->comboBox_diffuser_model->setCurrentIndex(-1);
}

//选择完末端类型
void RoomCalTable::on_comboBox_air_diff_terminal_type_currentTextChanged(const QString &arg1)
{
    if(this->currentComponentList.isEmpty())
        return;

    ui->comboBox_air_diff_size->clear();

    QStringList sizeStrings;

    for(auto& component : currentComponentList)
    {
        const AirDiff* air_diff = dynamic_cast<const AirDiff*>(component.data());
        if(air_diff->air_distributor_model == ui->comboBox_air_distributor_model->currentText()
                && air_diff->diffuser_model == ui->comboBox_diffuser_model->currentText())
        {
            sizeStrings.append(air_diff->terminal_size);
        }
    }

    // 转换为QSet去重
    QSet<QString> uniqueSet = QSet<QString>::fromList(sizeStrings);

    // 从QSet转回QStringList
    sizeStrings = QList<QString>::fromSet(uniqueSet);


    // 自定义比较函数
    auto compareSizes_rect = [](const QString &a, const QString &b) {
        QStringList partsA = a.split('X');
        QStringList partsB = b.split('X');
        if(partsA.size() == 2 && partsB.size() == 2) {
            int lengthA = partsA[0].toInt();
            int widthA = partsA[1].toInt();
            int lengthB = partsB[0].toInt();
            int widthB = partsB[1].toInt();

            if(lengthA == lengthB) {
                return widthA < widthB;
            }
            return lengthA < lengthB;
        }
        return false; // 如果格式不正确，默认不更改顺序
    };

    // 自定义比较函数
    auto compareSizes_circular = [](const QString &a, const QString &b) {
        int numberA = a.toInt();
        int numberB = b.toInt();

        return numberA < numberB; // 直接比较转换后的整数
    };

    if(arg1 == "方形")
    {
        // 使用自定义比较函数对尺寸字符串进行排序
        std::sort(sizeStrings.begin(), sizeStrings.end(), compareSizes_rect);
    }
    else if (arg1 == "圆形")
    {
        std::sort(sizeStrings.begin(), sizeStrings.end(), compareSizes_circular);
    }
    // 清空comboBox，防止重复添加
    ui->comboBox_air_diff_size->clear();

    // 添加排序后的尺寸到comboBox
    for (const QString &sizeString : sizeStrings) {
        ui->comboBox_air_diff_size->addItem(sizeString);
    }
}


void RoomCalTable::on_comboBox_air_diff_size_currentTextChanged(const QString &arg1)
{
    if(this->currentComponentList.isEmpty())
        return;

    for(auto& component : currentComponentList)
    {
        const AirDiff* air_diff = dynamic_cast<const AirDiff*>(component.data());
        if(air_diff->air_distributor_model == ui->comboBox_air_distributor_model->currentText()
                && air_diff->diffuser_model == ui->comboBox_diffuser_model->currentText()
                && air_diff->terminal_shape == ui->comboBox_air_diff_terminal_type->currentText()
                && air_diff->terminal_size == ui->comboBox_air_diff_size->currentText())
        {
            for(int i = 0; i < 8; i++)
            {
                terminal_atten_lineEdits[i]->setText(air_diff->atten[i]);
                terminal_refl_lineEdits[i]->setText(air_diff->refl[i]);
                terminal_noi_lineEdits[i]->setText(air_diff->noi[i]);
            }

            terminal_noi_lineEdits[8]->setText(air_diff->noi[8]);
        }
    }
}

