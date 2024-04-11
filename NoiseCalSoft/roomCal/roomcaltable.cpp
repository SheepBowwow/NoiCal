#include "roomcaltable.h"
#include "ui_roomcaltable.h"
#include <QContextMenuEvent>
#include <QRegularExpression>
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
#include <utility> // For std::pair
#define Pi 3.14159265358979323846

RoomCalTable::RoomCalTable(QString systemName, QWidget *parent,QString currentTableType) :
    QWidget(parent),
    systemName(systemName),
    isCollapsed(false),
    ui(new Ui::RoomCalTable)
{
    ui->setupUi(this);
    //connect(&ComponentManager::getInstance(), &ComponentManager::componentsUpdate, this, &RoomCalTable::updateModelComboBoxItems);
    initTerminalDataSourceConn();

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

void RoomCalTable::clearPage(QWidget *widget, bool isPageChanged)
{
    // 静态正则表达式对象，以避免重复创建和编译
    static QRegularExpression regexDataSourceOrFanType("data_source|fan_type|comboBox_silencer_type");
    static QRegularExpression regexNoiseLocateOrAngle("noise_locate|angle|terminal_type|pipe_type|elbow_type|reducer_type");

    for (QObject *child : widget->children()) {
        if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(child)) {
            if(lineEdit->text() != "-")
                lineEdit->clear();
        } else if (QComboBox *comboBox = qobject_cast<QComboBox*>(child)) {
            // 检查comboBox的名称是否匹配"data_source"
            QRegularExpressionMatch matchDataSource = regexDataSourceOrFanType.match(comboBox->objectName());
            // 检查comboBox的名称是否匹配"noise_locate"或"fan_type"
            QRegularExpressionMatch matchNoiseLocateOrFanType = regexNoiseLocateOrAngle.match(comboBox->objectName());

            if (matchDataSource.hasMatch()) {
                if(isPageChanged)
                    comboBox->setCurrentIndex(-1);
            } else if (matchNoiseLocateOrFanType.hasMatch()) {
                // 如果是"noise_locate"或"fan_type"，则设置为-1
                comboBox->setCurrentIndex(-1);
            } else {
                // 如果不是上述任一情况，则清空选择
                comboBox->clear();
            }
        } else if (QWidget *childWidget = qobject_cast<QWidget*>(child)) {
            clearPage(childWidget, isPageChanged); // 递归处理子部件
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
    clearPage(ui->stackedWidget_table->currentWidget(), true);
    if(updateModelComboBox == false)
    {
        for (QLineEdit* lineEdit : qAsConst(noi_after_cal_lineEdits))
        {
            if (lineEdit)
            {
                disconnect(lineEdit, &QLineEdit::textChanged, this, &RoomCalTable::set_Noise_after_cal_Vector);
            }
        }
        clearPageControlVectors();
        noi_after_cal_lineEdits.clear();
    }
    //不管是否为更新comboBox，都要更新component列表
    this->currentAllComponentList.clear();
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
                this->currentAllComponentList.append(component);
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
                SystemManager::getInstance().getNoiseComponentsInSystem(systemName, noise_src_component::AIRCONDITION_SEND);
        for(auto& uuid : list_send)
        {
            auto component = ComponentManager::getInstance().findComponent(uuid);
            auto aircondition = qSharedPointerDynamicCast<Aircondition>(component);
            if(aircondition)
            {
                auto airconditionInCalTable = QSharedPointer<Aircondition_in_calTable>::create(*aircondition); // 假设存在这样的复制构造函数

                airconditionInCalTable->send_or_exhaust = "送风机"; // 设置属性为"送风机"

                currentAllComponentList.append(airconditionInCalTable);
            }
        }

        QList<QString> list_exhaust =
                SystemManager::getInstance().getNoiseComponentsInSystem(systemName,noise_src_component::AIRCONDITION_EXHAUST);
        for(auto& uuid : list_exhaust)
        {
            auto component = ComponentManager::getInstance().findComponent(uuid);
            auto aircondition = qSharedPointerDynamicCast<Aircondition>(component);
            if(aircondition)
            {
                auto airconditionInCalTable = QSharedPointer<Aircondition_in_calTable>::create(*aircondition); // 假设存在这样的复制构造函数

                airconditionInCalTable->send_or_exhaust = "排风机"; // 设置属性为"送风机"

                currentAllComponentList.append(airconditionInCalTable);
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

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::VAV_TERMINAL);

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

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::CIRCULAR_DAMPER);

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


        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::RECT_DAMPER);

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


        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::AIRDIFF);

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

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::PUMPSEND);

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

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::PUMPSEND);
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

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::STATICBOX_GRILLE);
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

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::DISP_VENT_TERMINAL);

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

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::OTHER_SEND_TERMINAL);

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

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::STATICBOX);

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

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::MULTI_RANC);

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


        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::TEE);

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


        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::PIPE);

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

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::ELBOW);

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

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::REDUCER);
        ui->stackedWidget_table->setCurrentWidget(ui->page_reducer);
    }
    else if(arg1 == "消音器")
    {
        noi_after_cal_lineEdits = { ui->lineEdit_silencer_after_63, ui->lineEdit_silencer_after_125, ui->lineEdit_silencer_after_250,ui->lineEdit_silencer_after_500,
                                        ui->lineEdit_silencer_after_1k, ui->lineEdit_silencer_after_2k, ui->lineEdit_silencer_after_4k, ui->lineEdit_silencer_after_8k, ui->lineEdit_silencer_after_total};

        atten_lineEdits = { ui->lineEdit_silencer_63, ui->lineEdit_silencer_125, ui->lineEdit_silencer_250,ui->lineEdit_silencer_500,
                                        ui->lineEdit_silencer_1k, ui->lineEdit_silencer_2k, ui->lineEdit_silencer_4k, ui->lineEdit_silencer_8k, ui->lineEdit_silencer_total};

        this->currentAllComponentList =
                ComponentManager::getInstance().getComponentsByType(component_type_name::SILENCER);


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
    auto it = std::find_if(currentAllComponentList.begin(),currentAllComponentList.end(), [&](QSharedPointer<ComponentBase> component){
            return uuid == component->UUID;
    });

    //uuid等于当前的说明当前的部件被修改或删除了
    if(uuid == currentComponent->UUID && (isTypeSame && it == currentAllComponentList.end()))
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
void RoomCalTable::clearPageControlVectors()
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

std::pair<int, int> RoomCalTable::splitDimension(const QString &size)
{
    // 使用正则表达式来匹配 "数字x数字" 的格式
    QRegExp rx("(\\d+)x(\\d+)");
    if (rx.indexIn(size) != -1) {
        // 如果匹配成功，则从捕获的组中获取数字
        int firstNumber = rx.cap(1).toInt();
        int secondNumber = rx.cap(2).toInt();
        return std::make_pair(firstNumber, secondNumber);
    } else {
        // 如果输入格式不正确，返回一对-1表示错误
        return std::make_pair(-1, -1);
    }
}

void RoomCalTable::initTerminalDataSourceConn()
{
    connect(ui->comboBox_air_diff_atten_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::air_diff_data_source_changed);
    connect(ui->comboBox_air_diff_noi_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::air_diff_data_source_changed);
    connect(ui->comboBox_air_diff_refl_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::air_diff_data_source_changed);

    connect(ui->comboBox_pump_atten_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::pump_data_source_changed);
    connect(ui->comboBox_pump_noi_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::pump_data_source_changed);
    connect(ui->comboBox_pump_refl_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::pump_data_source_changed);

    connect(ui->comboBox_send_atten_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::send_data_source_changed);
    connect(ui->comboBox_send_noi_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::send_data_source_changed);
    connect(ui->comboBox_send_refl_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::send_data_source_changed);

    connect(ui->comboBox_staticBox_grille_atten_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::staticBox_grille_data_source_changed);
    connect(ui->comboBox_staticBox_grille_noi_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::staticBox_grille_data_source_changed);
    connect(ui->comboBox_staticBox_grille_refl_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::staticBox_grille_data_source_changed);

    connect(ui->comboBox_disp_vent_terminal_atten_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::disp_vent_terminal_data_source_changed);
    connect(ui->comboBox_disp_vent_terminal_noi_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::disp_vent_terminal_data_source_changed);
    connect(ui->comboBox_disp_vent_terminal_refl_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::disp_vent_terminal_data_source_changed);

    connect(ui->comboBox_other_send_terminal_atten_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::other_send_terminal_data_source_changed);
    connect(ui->comboBox_other_send_terminal_noi_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::other_send_terminal_data_source_changed);
    connect(ui->comboBox_other_send_terminal_refl_data_source, &QComboBox::currentTextChanged, this, &RoomCalTable::other_send_terminal_data_source_changed);
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

/**********风机**********/
#pragma region "fan"{
void RoomCalTable::on_comboBox_fan_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    currentComponentListByDataSource.clear();
    for(auto& component : currentAllComponentList)
    {
        // 尝试将component转换为Silencer类型的智能指针
        if(auto fan = qSharedPointerDynamicCast<Fan>(component))
        {
            if(fan->data_source == arg1)
                currentComponentListByDataSource.append(component); // 直接添加component
        }
    }

    for(auto& component : currentComponentListByDataSource)
    {
        if(auto fan = qSharedPointerDynamicCast<Fan>(component))
            ui->comboBox_fan_number->addItem(fan->number);
    }
    ui->comboBox_fan_number->setCurrentIndex(-1);
}

//风机编号选择
void RoomCalTable::on_comboBox_fan_number_currentTextChanged(const QString &arg1)
{
    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto fan = qSharedPointerDynamicCast<Fan>(component);
        if(fan && fan->number == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto fan = qSharedPointerDynamicCast<Fan>(*it))
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
    if(index == -1)
        return;
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
#pragma endregion}
/**********风机**********/


/**********风机盘管**********/
#pragma region "fanCoil"{
void RoomCalTable::on_comboBox_fanCoil_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    currentComponentListByDataSource.clear();
    for(auto& component : currentAllComponentList)
    {
        // 尝试将component转换为Silencer类型的智能指针
        if(auto fanCoil = qSharedPointerDynamicCast<FanCoil>(component))
        {
            if(fanCoil->data_source == arg1)
                currentComponentListByDataSource.append(component); // 直接添加component
        }
    }

    for(auto& component : currentComponentListByDataSource)
    {
        if(auto fanCoil = qSharedPointerDynamicCast<FanCoil>(component))
            ui->comboBox_fanCoil_model->addItem(fanCoil->model);
    }
    ui->comboBox_fanCoil_model->setCurrentIndex(-1);
}

//风机盘管型号选择
void RoomCalTable::on_comboBox_fanCoil_model_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_fanCoil_air_volume->clear();
    ui->lineEdit_fanCoil_static_press->clear();
    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto fanCoil = dynamic_cast<FanCoil*>(component.data());
        if(fanCoil && fanCoil->model == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto fanCoil = qSharedPointerDynamicCast<FanCoil>(*it))
        {
            currentComponent = (*it);
            ui->lineEdit_fanCoil_air_volume->setText(fanCoil->air_volume);
            ui->lineEdit_fanCoil_static_press->setText(fanCoil->static_pressure);
        }
    }
    on_comboBox_fanCoil_noise_locate_currentIndexChanged(ui->comboBox_fanCoil_noise_locate->currentIndex());
}

void RoomCalTable::on_comboBox_fanCoil_noise_locate_currentIndexChanged(int index)
{
    if(index == -1)
        return;
    if(auto fanCoil = qSharedPointerDynamicCast<FanCoil>(currentComponent))
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
#pragma endregion}
/**********风机盘管**********/

/**********空调器**********/
#pragma region "aircondition"{
void RoomCalTable::on_comboBox_aircondition_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    currentComponentListByDataSource.clear();
    for(auto& component : currentAllComponentList)
    {
        // 尝试将component转换为Silencer类型的智能指针
        if(auto airconditionIntable = qSharedPointerDynamicCast<Aircondition_in_calTable>(component))
        {
            if(airconditionIntable->data_source == arg1)
                currentComponentListByDataSource.append(component); // 直接添加component
        }
    }

    ui->comboBox_aircondition_number->setCurrentIndex(-1);
}

//空调器风机类型选择
void RoomCalTable::on_comboBox_aircondition_fan_type_currentIndexChanged(int index)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);

    // 断开信号和槽的连接
    disconnect(ui->comboBox_aircondition_number, SIGNAL(currentTextChanged(QString)),
               this, SLOT(on_comboBox_aircondition_number_currentTextChanged(QString)));
    //送风机
    if(index == 0)
    {
        for(auto& component : this->currentComponentListByDataSource)
        {
            if(auto airconditionIntable = qSharedPointerDynamicCast<Aircondition_in_calTable>(component))
            {
                if(airconditionIntable->send_or_exhaust == "送风机")
                    ui->comboBox_aircondition_number->addItem(airconditionIntable->send_number);
            }
        }
    }
    else if(index == 1)
    {
        for(auto& component : this->currentComponentListByDataSource)
        {
            if(auto airconditionIntable = qSharedPointerDynamicCast<Aircondition_in_calTable>(component))
            {
                if(airconditionIntable->send_or_exhaust == "排风机")
                    ui->comboBox_aircondition_number->addItem(airconditionIntable->exhaust_number);
            }
        }
    }
    ui->comboBox_aircondition_number->setCurrentIndex(-1);
    // 断开信号和槽的连接
    connect(ui->comboBox_aircondition_number, SIGNAL(currentTextChanged(QString)),
               this, SLOT(on_comboBox_aircondition_number_currentTextChanged(QString)));
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
        auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
            auto airconditionIntable = qSharedPointerDynamicCast<Aircondition_in_calTable>(component);
            if(airconditionIntable)
            {
                return airconditionIntable->send_or_exhaust == "送风机" && airconditionIntable->send_number == arg1;
            }
            return false;
        });
        if(it != currentComponentListByDataSource.end())
        {
            if(auto airconditionIntable = qSharedPointerDynamicCast<Aircondition_in_calTable>(*it))
            {
                currentComponent = (*it);
                ui->lineEdit_aircondition_air_volume->setText(airconditionIntable->send_air_volume);
                ui->lineEdit_aircondition_model->setText(airconditionIntable->model);
                ui->lineEdit_aircondition_static_press->setText(airconditionIntable->send_static_pressure);
            }
        }
    }
    else if(fan_type_index == 1)
    {
        auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
            auto airconditionIntable = qSharedPointerDynamicCast<Aircondition_in_calTable>(component);
            if(airconditionIntable)
            {
                return airconditionIntable->send_or_exhaust == "排风机" && airconditionIntable->exhaust_number == arg1;
            }
            return false;
        });
        if(it != currentComponentListByDataSource.end())
        {
            if(auto airconditionIntable = qSharedPointerDynamicCast<Aircondition_in_calTable>(*it))
            {
                currentComponent = (*it);
                ui->lineEdit_aircondition_air_volume->setText(airconditionIntable->exhaust_air_volume);
                ui->lineEdit_aircondition_model->setText(airconditionIntable->model);
                ui->lineEdit_aircondition_static_press->setText(airconditionIntable->exhaust_static_pressure);
            }
        }
    }

    on_comboBox_aircondition_noise_locate_currentIndexChanged(ui->comboBox_aircondition_noise_locate->currentIndex());
}

void RoomCalTable::on_comboBox_aircondition_noise_locate_currentIndexChanged(int index)
{
    if(index == -1)
        return;


    if(currentComponent)
    {
        auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
            auto airconditionIntable = qSharedPointerDynamicCast<Aircondition_in_calTable>(component);
            if(airconditionIntable)
            {
                return airconditionIntable->UUID == currentComponent->UUID;
            }
            return false;
        });
        if(it == currentComponentListByDataSource.end())
        {
            return;
        }
    }

    if(auto airconditionIntable = qSharedPointerDynamicCast<Aircondition_in_calTable>(currentComponent))
    {
        if(index == 0)
        {
            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                noi_after_cal_lineEdits[i]->setText(
                            airconditionIntable->send_or_exhaust == "送风机" ?
                                airconditionIntable->noi_send_in[i] : airconditionIntable->noi_exhaust_in[i]);
            }
        }
        else if(index == 1)
        {
            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                noi_after_cal_lineEdits[i]->setText(
                            airconditionIntable->send_or_exhaust == "送风机" ?
                                airconditionIntable->noi_send_out[i] : airconditionIntable->noi_exhaust_out[i]);
            }
        }
    }

}
#pragma endregion}
/**********空调器**********/

/**********变风量末端**********/
#pragma region "VAV_terminal"{
void RoomCalTable::on_comboBox_VAV_terminal_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    currentComponentListByDataSource.clear();
    for(auto& component : currentAllComponentList)
    {
        // 尝试将component转换为Silencer类型的智能指针
        if(auto vav = qSharedPointerDynamicCast<VAV_terminal>(component))
        {
            if(vav->data_source == arg1)
                currentComponentListByDataSource.append(component); // 直接添加component
        }
    }

    // 断开信号和槽的连接
    disconnect(ui->comboBox_VAV_terminal_number, SIGNAL(currentTextChanged(QString)),
               this, SLOT(on_comboBox_VAV_terminal_number_currentTextChanged(QString)));

    for(auto& component : currentComponentListByDataSource)
    {
        if(auto vav = qSharedPointerDynamicCast<VAV_terminal>(component))
            ui->comboBox_VAV_terminal_number->addItem(vav->number);
    }
    ui->comboBox_VAV_terminal_number->setCurrentIndex(-1);

    // 重新连接信号和槽
    connect(ui->comboBox_VAV_terminal_number, SIGNAL(currentTextChanged(QString)),
            this, SLOT(on_comboBox_VAV_terminal_number_currentTextChanged(QString)));
}

void RoomCalTable::on_comboBox_VAV_terminal_number_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_VAV_terminal_number->currentIndex() == -1)
        return;
    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto vav = qSharedPointerDynamicCast<VAV_terminal>(component);
        if(vav && vav->number == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto vav = qSharedPointerDynamicCast<VAV_terminal>(*it))
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
#pragma endregion}
/**********变风量末端**********/

/**********圆形调风门**********/
#pragma region "circular_damper"{
void RoomCalTable::circular_damper_noise_cal()
{
    if(ui->comboBox_circular_damper_angle->currentText().isEmpty() ||
            ui->lineEdit_circular_damper_air_volume->text().isEmpty() ||
            ui->lineEdit_circular_damper_diameter->text().isEmpty())
        return;

    array<double, 9> noi =  calDamperNoise(Circle,ui->comboBox_circular_damper_angle->currentText().remove("°").toInt(),
                                           ui->lineEdit_circular_damper_air_volume->text().toDouble(),
                                           ui->lineEdit_circular_damper_diameter->text().toDouble());

    for(int i = 0; i < noi.size(); i++)
    {
        noi_lineEdits[i]->setText(QString::number(noi[i],'f', 1));
    }
}

void RoomCalTable::on_comboBox_circular_damper_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    if(arg1 != "经验公式")
    {
        ui->comboBox_circular_damper_model->setEnabled(true);
        ui->comboBox_circular_damper_model->setStyleSheet("QComboBox { border:none;"
                                                          "border-bottom:1px solid black;}");

        // 隐藏下拉箭头
        ui->comboBox_circular_damper_angle->setStyleSheet("QComboBox { combobox-popup: 0; border:none;"
                                                          "border-bottom:1px solid black;"
                                                          "font-family: '黑体';"
                                                          "font-size: 14px;}"
                                                          "QComboBox::drop-down { width:0px; }"); // CSS隐藏下拉箭头
        // 防止显示下拉列表
        ui->comboBox_circular_damper_angle->setEditable(true);
        ui->comboBox_circular_damper_angle->lineEdit()->setReadOnly(true);

        ui->lineEdit_circular_damper_diameter->setReadOnly(true);
        ui->lineEdit_circular_damper_air_volume->setReadOnly(true);

        disconnect(ui->comboBox_circular_damper_angle, &QComboBox::currentTextChanged, this, &RoomCalTable::circular_damper_noise_cal);
        disconnect(ui->lineEdit_circular_damper_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::circular_damper_noise_cal);
        disconnect(ui->lineEdit_circular_damper_diameter, &QLineEdit::textChanged, this, &RoomCalTable::circular_damper_noise_cal);

        clearPage(ui->stackedWidget_table->currentWidget(), false);
        currentComponentListByDataSource.clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto circluar_damper = qSharedPointerDynamicCast<Circular_damper>(component))
            {
                if(circluar_damper->data_source == arg1)
                    currentComponentListByDataSource.append(component); // 直接添加component
            }
        }

        // 断开信号和槽的连接
        disconnect(ui->comboBox_circular_damper_model, SIGNAL(currentTextChanged(QString)),
                   this, SLOT(on_comboBox_circular_damper_model_currentTextChanged(QString)));

        for(auto& component : currentComponentListByDataSource)
        {
            if(auto circluar_damper = qSharedPointerDynamicCast<Circular_damper>(component))
                ui->comboBox_circular_damper_model->addItem(circluar_damper->model);
        }
        ui->comboBox_circular_damper_model->setCurrentIndex(-1);

        // 重新连接信号和槽
        connect(ui->comboBox_circular_damper_model, SIGNAL(currentTextChanged(QString)),
                this, SLOT(on_comboBox_circular_damper_model_currentTextChanged(QString)));
    }
    else
    {
        ui->comboBox_circular_damper_model->setCurrentText("-");
        ui->comboBox_circular_damper_model->setEnabled(false);
        ui->comboBox_circular_damper_model->setStyleSheet("QComboBox { border:1px solid black ;background-color: rgb(240,240,240);"
                                                          "font-family: '黑体';"
                                                          "font-size: 14px;}");

        // 隐藏下拉箭头
        ui->comboBox_circular_damper_angle->setStyleSheet("QComboBox{border:none; "
                                                          "border-bottom:1px solid black;}"
                                                          "QLineEdit{font-family: '黑体';"
                                                          "font-size: 14px;}"); // CSS隐藏下拉箭头
        // 防止显示下拉列表
        ui->comboBox_circular_damper_angle->setMaxVisibleItems(10);
        ui->comboBox_circular_damper_angle->setEditable(false);
        //ui->comboBox_circular_damper_angle->lineEdit()->setReadOnly(false);

        ui->lineEdit_circular_damper_diameter->setReadOnly(false);
        ui->lineEdit_circular_damper_air_volume->setReadOnly(false);

        connect(ui->comboBox_circular_damper_angle, &QComboBox::currentTextChanged, this, &RoomCalTable::circular_damper_noise_cal);
        connect(ui->lineEdit_circular_damper_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::circular_damper_noise_cal);
        connect(ui->lineEdit_circular_damper_diameter, &QLineEdit::textChanged, this, &RoomCalTable::circular_damper_noise_cal);
    }
}

void RoomCalTable::on_comboBox_circular_damper_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_circular_damper_model->currentIndex() == -1)
        return;

    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto damper = qSharedPointerDynamicCast<Circular_damper>(component);
        if(damper && damper->model == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto damper = qSharedPointerDynamicCast<Circular_damper>(*it))
        {
            currentComponent = (*it);
            ui->lineEdit_circular_damper_air_volume->setText(damper->air_volume);
            ui->lineEdit_circular_damper_diameter->setText(damper->diameter);
            ui->comboBox_circular_damper_angle->setCurrentText(damper->angle);

            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                noi_lineEdits[i]->setText(damper->noi[i]);
            }
        }
    }
}

#pragma endregion}
/**********圆形调风门**********/

/**********方形调风门**********/
#pragma region "rect_damper"{
void RoomCalTable::rect_damper_noise_cal()
{
    if(ui->comboBox_rect_damper_angle->currentText().isEmpty() ||
            ui->lineEdit_rect_damper_air_volume->text().isEmpty() ||
            ui->lineEdit_rect_damper_size->text().isEmpty())
        return;

    auto dimensions = splitDimension(ui->lineEdit_rect_damper_size->text());
    double length, width;
    if (dimensions.first != -1) {
        length = dimensions.first;
        width = dimensions.second;
    }

    array<double, 9> noi =  calDamperNoise(Rect,ui->comboBox_rect_damper_angle->currentText().remove("°").toInt(),
                                           ui->lineEdit_rect_damper_air_volume->text().toDouble(),
                                           length,
                                           width);

    for(int i = 0; i < noi.size(); i++)
    {
        noi_lineEdits[i]->setText(QString::number(noi[i],'f', 1));
    }
}

void RoomCalTable::on_comboBox_rect_damper_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    if(arg1 != "经验公式")
    {
        ui->comboBox_rect_damper_model->setEnabled(true);
        ui->comboBox_rect_damper_model->setStyleSheet("QComboBox { border:none;"
                                                          "border-bottom:1px solid black;}");

        // 隐藏下拉箭头
        ui->comboBox_rect_damper_angle->setStyleSheet("QComboBox { combobox-popup: 0; border:none;"
                                                          "border-bottom:1px solid black;"
                                                          "font-family: '黑体';"
                                                          "font-size: 14px;}"
                                                          "QComboBox::drop-down { width:0px; }"); // CSS隐藏下拉箭头
        // 防止显示下拉列表
        ui->comboBox_rect_damper_angle->setEditable(true);
        ui->comboBox_rect_damper_angle->lineEdit()->setReadOnly(true);

        ui->lineEdit_rect_damper_size->setValidator(nullptr);
        ui->lineEdit_rect_damper_size->setReadOnly(true);
        ui->lineEdit_rect_damper_air_volume->setReadOnly(true);

        disconnect(ui->comboBox_rect_damper_angle, &QComboBox::currentTextChanged, this, &RoomCalTable::rect_damper_noise_cal);
        disconnect(ui->lineEdit_rect_damper_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::rect_damper_noise_cal);
        disconnect(ui->lineEdit_rect_damper_size, &QLineEdit::textChanged, this, &RoomCalTable::rect_damper_noise_cal);

        currentComponentListByDataSource.clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto rect_damper = qSharedPointerDynamicCast<Rect_damper>(component))
            {
                if(rect_damper->data_source == arg1)
                    currentComponentListByDataSource.append(component); // 直接添加component
            }
        }

        // 断开信号和槽的连接
        disconnect(ui->comboBox_rect_damper_model, SIGNAL(currentTextChanged(QString)),
                   this, SLOT(on_comboBox_rect_damper_model_currentTextChanged(QString)));

        for(auto& component : currentComponentListByDataSource)
        {
            if(auto rect_damper = qSharedPointerDynamicCast<Rect_damper>(component))
                ui->comboBox_rect_damper_model->addItem(rect_damper->model);
        }
        ui->comboBox_rect_damper_model->setCurrentIndex(-1);

        // 重新连接信号和槽
        connect(ui->comboBox_rect_damper_model, SIGNAL(currentTextChanged(QString)),
                this, SLOT(on_comboBox_rect_damper_model_currentTextChanged(QString)));
    }
    else
    {
        //给size LineEdit安装验证器
        ui->lineEdit_rect_damper_size->setValidator(new DimensionValidator(this));
        ui->comboBox_rect_damper_model->setCurrentText("-");
        ui->comboBox_rect_damper_model->setEnabled(false);
        ui->comboBox_rect_damper_model->setStyleSheet("QComboBox { border:1px solid black ;background-color: rgb(240,240,240);"
                                                          "font-family: '黑体';"
                                                          "font-size: 14px;}");

        // 隐藏下拉箭头
        ui->comboBox_rect_damper_angle->setStyleSheet("QComboBox{border:none; "
                                                          "border-bottom:1px solid black;}"
                                                          "QLineEdit{font-family: '黑体';"
                                                          "font-size: 14px;}"); // CSS隐藏下拉箭头
        // 防止显示下拉列表
        ui->comboBox_rect_damper_angle->setMaxVisibleItems(10);
        ui->comboBox_rect_damper_angle->setEditable(false);

        ui->lineEdit_rect_damper_size->setPlaceholderText("长 x 宽");
        ui->lineEdit_rect_damper_size->setReadOnly(false);
        ui->lineEdit_rect_damper_air_volume->setReadOnly(false);

        connect(ui->comboBox_rect_damper_angle, &QComboBox::currentTextChanged, this, &RoomCalTable::rect_damper_noise_cal);
        connect(ui->lineEdit_rect_damper_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::rect_damper_noise_cal);
        connect(ui->lineEdit_rect_damper_size, &QLineEdit::textChanged, this, &RoomCalTable::rect_damper_noise_cal);
    }
}

void RoomCalTable::on_comboBox_rect_damper_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_rect_damper_model->currentIndex() == -1)
        return;

    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto damper = qSharedPointerDynamicCast<Rect_damper>(component);
        if(damper && damper->model == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto damper = qSharedPointerDynamicCast<Rect_damper>(*it))
        {
            currentComponent = (*it);
            ui->lineEdit_rect_damper_air_volume->setText(damper->air_volume);
            ui->lineEdit_rect_damper_size->setText(damper->size);
            ui->comboBox_rect_damper_angle->setCurrentText(damper->angle);

            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                noi_lineEdits[i]->setText(damper->noi[i]);
            }
        }
    }
}

#pragma endregion}
/**********方形调风门**********/

/**********布风器+散流器**********/
#pragma region "rect_damper"{
//选择完布风器型号,添加散流器型号
void RoomCalTable::air_diff_data_source_changed()
{
    if(ui->comboBox_air_diff_atten_data_source->currentText().isEmpty() ||
            ui->comboBox_air_diff_noi_data_source->currentText().isEmpty() ||
            ui->comboBox_air_diff_refl_data_source->currentText().isEmpty())
        return;

    clearPage(ui->stackedWidget_table->currentWidget(), false);
    currentComponentListByDataSource.clear();
    for(auto& component : currentAllComponentList)
    {
        // 尝试将component转换为Silencer类型的智能指针
        if(auto airdiff = qSharedPointerDynamicCast<AirDiff>(component))
        {
            if(airdiff->noi_data_source == ui->comboBox_air_diff_noi_data_source->currentText()
                    && airdiff->atten_data_source == ui->comboBox_air_diff_atten_data_source->currentText()
                    && airdiff->refl_data_source == ui->comboBox_air_diff_refl_data_source->currentText())
                currentComponentListByDataSource.append(component); // 直接添加component
        }
    }

    qDebug() << ui->comboBox_air_diff_noi_data_source->currentText();
    qDebug() << ui->comboBox_air_diff_atten_data_source->currentText();
    qDebug() << ui->comboBox_air_diff_refl_data_source->currentText();

    for(auto& component : currentComponentListByDataSource)
    {
        if(auto airdiff = qSharedPointerDynamicCast<AirDiff>(component))
            ui->comboBox_air_distributor_model->addItem(airdiff->air_distributor_model);
    }
    ui->comboBox_air_distributor_model->setCurrentIndex(-1);
}

void RoomCalTable::on_comboBox_air_distributor_model_currentTextChanged(const QString &arg1)
{
    ui->comboBox_diffuser_model->clear();
    ui->comboBox_diffuser_model->blockSignals(true);
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto airdiff = qSharedPointerDynamicCast<AirDiff>(component))
        {
            if(airdiff->air_distributor_model == arg1)
                ui->comboBox_diffuser_model->addItem(airdiff->diffuser_model);
        }
    }
    ui->comboBox_diffuser_model->setCurrentIndex(-1);
    ui->comboBox_diffuser_model->blockSignals(false);
}

void RoomCalTable::on_comboBox_diffuser_model_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    ui->lineEdit_air_diff_size->clear();
    ui->comboBox_air_diff_terminal_type->clear();
    // 在添加item之前，暂时阻止comboBox的信号
    ui->comboBox_air_diff_terminal_type->blockSignals(true);
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto airdiff = qSharedPointerDynamicCast<AirDiff>(component))
        {
            if(airdiff->air_distributor_model == ui->comboBox_air_distributor_model->currentText() && airdiff->diffuser_model == arg1)
            {
                ui->comboBox_air_diff_terminal_type->addItem(airdiff->terminal_shape);
            }
        }
    }
    ui->comboBox_air_diff_terminal_type->setCurrentIndex(-1);
    // 添加item之后，恢复comboBox的信号
    ui->comboBox_air_diff_terminal_type->blockSignals(false);
}

//选择完末端类型
void RoomCalTable::on_comboBox_air_diff_terminal_type_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty() ||
            ui->comboBox_air_distributor_model->currentText().isEmpty()
            || ui->comboBox_diffuser_model->currentText().isEmpty())
        return;
    ui->lineEdit_air_diff_size->clear();
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto airdiff = qSharedPointerDynamicCast<AirDiff>(component))
        {
            if(airdiff->air_distributor_model == ui->comboBox_air_distributor_model->currentText()
                    && airdiff->diffuser_model == ui->comboBox_diffuser_model->currentText() && airdiff->terminal_shape == arg1)
            {
                ui->lineEdit_air_diff_size->setText(airdiff->terminal_size);
                for(int i = 0; i < 8; i++)
                {
                    terminal_noi_lineEdits[i]->setText(airdiff->noi[i]);
                    terminal_atten_lineEdits[i]->setText(airdiff->atten[i]);
                    terminal_refl_lineEdits[i]->setText(airdiff->refl[i]);
                }
                terminal_noi_lineEdits[8]->setText(airdiff->noi[8]);
            }
        }
    }
}

#pragma endregion}
/**********布风器+散流器**********/

/**********抽风头**********/
#pragma region "pump"{
void RoomCalTable::pump_data_source_changed()
{
    if(ui->comboBox_pump_atten_data_source->currentText().isEmpty() ||
            ui->comboBox_pump_noi_data_source->currentText().isEmpty() ||
            ui->comboBox_pump_refl_data_source->currentText().isEmpty())
        return;

    clearPage(ui->stackedWidget_table->currentWidget(), false);
    currentComponentListByDataSource.clear();
    for(auto& component : currentAllComponentList)
    {
        // 尝试将component转换为Silencer类型的智能指针
        if(auto pump = qSharedPointerDynamicCast<PumpSend>(component))
        {
            if(pump->noi_data_source == ui->comboBox_pump_noi_data_source->currentText()
                    && pump->atten_data_source == ui->comboBox_pump_atten_data_source->currentText()
                    && pump->refl_data_source == ui->comboBox_pump_refl_data_source->currentText())
                currentComponentListByDataSource.append(component); // 直接添加component
        }
    }

    for(auto& component : currentComponentListByDataSource)
    {
        if(auto pump = qSharedPointerDynamicCast<PumpSend>(component))
            ui->comboBox_pump_model->addItem(pump->model);
    }
    ui->comboBox_pump_model->setCurrentIndex(-1);
}


void RoomCalTable::on_comboBox_pump_model_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    ui->lineEdit_pump_size->clear();
    ui->comboBox_pump_terminal_type->clear();
    // 在添加item之前，暂时阻止comboBox的信号
    ui->comboBox_pump_terminal_type->blockSignals(true);
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto pump = qSharedPointerDynamicCast<PumpSend>(component))
        {
            if(pump->model == arg1)
            {
                ui->comboBox_pump_terminal_type->addItem(pump->terminal_shape);
            }
        }
    }
    ui->comboBox_pump_terminal_type->setCurrentIndex(-1);
    // 添加item之后，恢复comboBox的信号
    ui->comboBox_pump_terminal_type->blockSignals(false);
}

void RoomCalTable::on_comboBox_pump_terminal_type_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty() || ui->comboBox_pump_model->currentText().isEmpty())
        return;
    ui->lineEdit_pump_size->clear();
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto pump = qSharedPointerDynamicCast<PumpSend>(component))
        {
            if(pump->model == ui->comboBox_pump_model->currentText()
                    && pump->terminal_shape == arg1)
            {
                ui->lineEdit_pump_size->setText(pump->terminal_size);
                for(int i = 0; i < 8; i++)
                {
                    terminal_noi_lineEdits[i]->setText(pump->noi[i]);
                    terminal_atten_lineEdits[i]->setText(pump->atten[i]);
                    terminal_refl_lineEdits[i]->setText(pump->refl[i]);
                }
                terminal_noi_lineEdits[8]->setText(pump->noi[8]);
            }
        }
    }
}

#pragma endregion}
/**********抽风头**********/

/**********送风头**********/
#pragma region "send"{
void RoomCalTable::send_data_source_changed()
{
    if(ui->comboBox_send_atten_data_source->currentText().isEmpty() ||
            ui->comboBox_send_noi_data_source->currentText().isEmpty() ||
            ui->comboBox_send_refl_data_source->currentText().isEmpty())
        return;

    clearPage(ui->stackedWidget_table->currentWidget(), false);
    currentComponentListByDataSource.clear();
    ui->comboBox_send_model->blockSignals(true);
    for(auto& component : currentAllComponentList)
    {
        // 尝试将component转换为Silencer类型的智能指针
        if(auto send = qSharedPointerDynamicCast<PumpSend>(component))
        {
            if(send->noi_data_source == ui->comboBox_send_noi_data_source->currentText()
                    && send->atten_data_source == ui->comboBox_send_atten_data_source->currentText()
                    && send->refl_data_source == ui->comboBox_send_refl_data_source->currentText())
                currentComponentListByDataSource.append(component); // 直接添加component
        }
    }

    for(auto& component : currentComponentListByDataSource)
    {
        if(auto send = qSharedPointerDynamicCast<PumpSend>(component))
            ui->comboBox_send_model->addItem(send->model);
    }
    ui->comboBox_send_model->setCurrentIndex(-1);
    ui->comboBox_send_model->blockSignals(false);
}

void RoomCalTable::on_comboBox_send_model_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    ui->lineEdit_send_size->clear();
    ui->comboBox_send_terminal_type->clear();
    // 在添加item之前，暂时阻止comboBox的信号
    ui->comboBox_send_terminal_type->blockSignals(true);
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto send = qSharedPointerDynamicCast<PumpSend>(component))
        {
            if(send->model == arg1)
            {
                ui->comboBox_send_terminal_type->addItem(send->terminal_shape);
            }
        }
    }
    ui->comboBox_send_terminal_type->setCurrentIndex(-1);
    // 添加item之后，恢复comboBox的信号
    ui->comboBox_send_terminal_type->blockSignals(false);
}

void RoomCalTable::on_comboBox_send_terminal_type_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty() || ui->comboBox_send_model->currentText().isEmpty())
        return;
    ui->lineEdit_send_size->clear();
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto send = qSharedPointerDynamicCast<PumpSend>(component))
        {
            if(send->model == ui->comboBox_send_model->currentText()
                    && send->terminal_shape == arg1)
            {
                ui->lineEdit_send_size->setText(send->terminal_size);
                for(int i = 0; i < 8; i++)
                {
                    terminal_noi_lineEdits[i]->setText(send->noi[i]);
                    terminal_atten_lineEdits[i]->setText(send->atten[i]);
                    terminal_refl_lineEdits[i]->setText(send->refl[i]);
                }
                terminal_noi_lineEdits[8]->setText(send->noi[8]);
            }
        }
    }
}
#pragma endregion}
/**********送风头**********/


/**********静压箱+格栅**********/
#pragma region "staticBox_grille"{
void RoomCalTable::staticBox_grille_data_source_changed()
{
    if(ui->comboBox_staticBox_grille_atten_data_source->currentText().isEmpty() ||
            ui->comboBox_staticBox_grille_noi_data_source->currentText().isEmpty() ||
            ui->comboBox_staticBox_grille_refl_data_source->currentText().isEmpty())
        return;

    clearPage(ui->stackedWidget_table->currentWidget(), false);
    currentComponentListByDataSource.clear();
    for(auto& component : currentAllComponentList)
    {
        // 尝试将component转换为Silencer类型的智能指针
        if(auto staticBox_grille = qSharedPointerDynamicCast<StaticBox_grille>(component))
        {
            if(staticBox_grille->noi_data_source == ui->comboBox_staticBox_grille_noi_data_source->currentText()
                    && staticBox_grille->atten_data_source == ui->comboBox_staticBox_grille_atten_data_source->currentText()
                    && staticBox_grille->refl_data_source == ui->comboBox_staticBox_grille_refl_data_source->currentText())
                currentComponentListByDataSource.append(component); // 直接添加component
        }
    }

    ui->comboBox_staticBox_model->blockSignals(true);
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto staticBox_grille = qSharedPointerDynamicCast<StaticBox_grille>(component))
            ui->comboBox_staticBox_model->addItem(staticBox_grille->staticBox_model);
    }
    ui->comboBox_staticBox_model->setCurrentIndex(-1);
    ui->comboBox_staticBox_model->blockSignals(false);
}

void RoomCalTable::on_comboBox_staticBox_model_currentTextChanged(const QString &arg1)
{
    ui->comboBox_grille_model->clear();
    ui->comboBox_grille_model->blockSignals(true);
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto staticBox_grille = qSharedPointerDynamicCast<StaticBox_grille>(component))
        {
            if(staticBox_grille->staticBox_model == arg1)
                ui->comboBox_grille_model->addItem(staticBox_grille->grille_model);
        }
    }
    ui->comboBox_grille_model->setCurrentIndex(-1);
    ui->comboBox_grille_model->blockSignals(false);
}

void RoomCalTable::on_comboBox_grille_model_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    ui->lineEdit_staticBox_grille_size->clear();
    ui->comboBox_staticBox_grille_terminal_type->clear();
    // 在添加item之前，暂时阻止comboBox的信号
    ui->comboBox_staticBox_grille_terminal_type->blockSignals(true);
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto staticBox_grille = qSharedPointerDynamicCast<StaticBox_grille>(component))
        {
            if(staticBox_grille->staticBox_model == ui->comboBox_air_distributor_model->currentText()
                    && staticBox_grille->grille_model == arg1)
            {
                ui->comboBox_staticBox_grille_terminal_type->addItem(staticBox_grille->terminal_shape);
            }
        }
    }
    ui->comboBox_staticBox_grille_terminal_type->setCurrentIndex(-1);
    // 添加item之后，恢复comboBox的信号
    ui->comboBox_staticBox_grille_terminal_type->blockSignals(false);
}

void RoomCalTable::on_comboBox_staticBox_grille_terminal_type_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty() ||
            ui->comboBox_staticBox_model->currentText().isEmpty()
            || ui->comboBox_grille_model->currentText().isEmpty())
        return;
    ui->lineEdit_staticBox_grille_size->clear();
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto staticBox_grille = qSharedPointerDynamicCast<StaticBox_grille>(component))
        {
            if(staticBox_grille->staticBox_model == ui->comboBox_staticBox_model->currentText()
                    && staticBox_grille->grille_model == ui->comboBox_grille_model->currentText() && staticBox_grille->terminal_shape == arg1)
            {
                ui->lineEdit_staticBox_grille_size->setText(staticBox_grille->terminal_size);
                for(int i = 0; i < 8; i++)
                {
                    terminal_noi_lineEdits[i]->setText(staticBox_grille->noi[i]);
                    terminal_atten_lineEdits[i]->setText(staticBox_grille->atten[i]);
                    terminal_refl_lineEdits[i]->setText(staticBox_grille->refl[i]);
                }
                terminal_noi_lineEdits[8]->setText(staticBox_grille->noi[8]);
            }
        }
    }
}
#pragma endregion}
/**********静压箱+格栅**********/


/**********置换通风末端**********/
#pragma region "staticBox_grille"{
void RoomCalTable::disp_vent_terminal_data_source_changed()
{
    if(ui->comboBox_disp_vent_terminal_atten_data_source->currentText().isEmpty() ||
            ui->comboBox_disp_vent_terminal_noi_data_source->currentText().isEmpty() ||
            ui->comboBox_disp_vent_terminal_refl_data_source->currentText().isEmpty())
        return;

    clearPage(ui->stackedWidget_table->currentWidget(), false);
    currentComponentListByDataSource.clear();
    for(auto& component : currentAllComponentList)
    {
        // 尝试将component转换为Silencer类型的智能指针
        if(auto disp_vent_terminal = qSharedPointerDynamicCast<Disp_vent_terminal>(component))
        {
            if(disp_vent_terminal->noi_data_source == ui->comboBox_disp_vent_terminal_noi_data_source->currentText()
                    && disp_vent_terminal->atten_data_source == ui->comboBox_disp_vent_terminal_atten_data_source->currentText()
                    && disp_vent_terminal->refl_data_source == ui->comboBox_disp_vent_terminal_refl_data_source->currentText())
                currentComponentListByDataSource.append(component); // 直接添加component
        }
    }

    for(auto& component : currentComponentListByDataSource)
    {
        if(auto disp_vent_terminal = qSharedPointerDynamicCast<Disp_vent_terminal>(component))
            ui->comboBox_disp_vent_terminal_model->addItem(disp_vent_terminal->model);
    }
    ui->comboBox_disp_vent_terminal_model->setCurrentIndex(-1);
}

void RoomCalTable::on_comboBox_disp_vent_terminal_model_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    ui->lineEdit_disp_vent_terminal_size->clear();
    ui->comboBox_disp_vent_terminal_type->clear();
    // 在添加item之前，暂时阻止comboBox的信号
    ui->comboBox_disp_vent_terminal_type->blockSignals(true);
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto disp_vent_terminal = qSharedPointerDynamicCast<Disp_vent_terminal>(component))
        {
            if(disp_vent_terminal->model == arg1)
            {
                ui->comboBox_disp_vent_terminal_type->addItem(disp_vent_terminal->terminal_shape);
            }
        }
    }
    ui->comboBox_disp_vent_terminal_type->setCurrentIndex(-1);
    // 添加item之后，恢复comboBox的信号
    ui->comboBox_disp_vent_terminal_type->blockSignals(false);
}

void RoomCalTable::on_comboBox_disp_vent_terminal_type_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty() || ui->comboBox_disp_vent_terminal_model->currentText().isEmpty())
        return;
    ui->lineEdit_disp_vent_terminal_size->clear();
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto disp_vent_terminal = qSharedPointerDynamicCast<Disp_vent_terminal>(component))
        {
            if(disp_vent_terminal->model == ui->comboBox_disp_vent_terminal_model->currentText()
                    && disp_vent_terminal->terminal_shape == arg1)
            {
                ui->lineEdit_disp_vent_terminal_size->setText(disp_vent_terminal->terminal_size);
                for(int i = 0; i < 8; i++)
                {
                    terminal_noi_lineEdits[i]->setText(disp_vent_terminal->noi[i]);
                    terminal_atten_lineEdits[i]->setText(disp_vent_terminal->atten[i]);
                    terminal_refl_lineEdits[i]->setText(disp_vent_terminal->refl[i]);
                }
                terminal_noi_lineEdits[8]->setText(disp_vent_terminal->noi[8]);
            }
        }
    }
}


#pragma endregion}
/**********置换通风末端**********/

/**********其他送风末端**********/
#pragma region "other_send_terminal"{
void RoomCalTable::other_send_terminal_data_source_changed()
{
    if(ui->comboBox_other_send_terminal_atten_data_source->currentText().isEmpty() ||
            ui->comboBox_other_send_terminal_noi_data_source->currentText().isEmpty() ||
            ui->comboBox_other_send_terminal_refl_data_source->currentText().isEmpty())
        return;

    clearPage(ui->stackedWidget_table->currentWidget(), false);
    currentComponentListByDataSource.clear();
    for(auto& component : currentAllComponentList)
    {
        // 尝试将component转换为Silencer类型的智能指针
        if(auto other_send_terminal = qSharedPointerDynamicCast<Other_send_terminal>(component))
        {
            if(other_send_terminal->noi_data_source == ui->comboBox_other_send_terminal_noi_data_source->currentText()
                    && other_send_terminal->atten_data_source == ui->comboBox_other_send_terminal_atten_data_source->currentText()
                    && other_send_terminal->refl_data_source == ui->comboBox_other_send_terminal_refl_data_source->currentText())
                currentComponentListByDataSource.append(component); // 直接添加component
        }
    }

    for(auto& component : currentComponentListByDataSource)
    {
        if(auto other_send_terminal = qSharedPointerDynamicCast<Other_send_terminal>(component))
            ui->comboBox_other_send_terminal_model->addItem(other_send_terminal->model);
    }
    ui->comboBox_other_send_terminal_model->setCurrentIndex(-1);
}

void RoomCalTable::on_comboBox_other_send_terminal_model_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    ui->lineEdit_other_send_terminal_size->clear();
    ui->comboBox_other_send_terminal_type->clear();
    // 在添加item之前，暂时阻止comboBox的信号
    ui->comboBox_other_send_terminal_type->blockSignals(true);
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto other_send_terminal = qSharedPointerDynamicCast<Other_send_terminal>(component))
        {
            if(other_send_terminal->model == arg1)
            {
                ui->comboBox_other_send_terminal_type->addItem(other_send_terminal->terminal_shape);
            }
        }
    }
    ui->comboBox_other_send_terminal_type->setCurrentIndex(-1);
    // 添加item之后，恢复comboBox的信号
    ui->comboBox_other_send_terminal_type->blockSignals(false);
}

void RoomCalTable::on_comboBox_other_send_terminal_type_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty() || ui->comboBox_other_send_terminal_model->currentText().isEmpty())
        return;
    ui->lineEdit_other_send_terminal_size->clear();
    for(auto& component : currentComponentListByDataSource)
    {
        if(auto other_send_terminal = qSharedPointerDynamicCast<Other_send_terminal>(component))
        {
            if(other_send_terminal->model == ui->comboBox_other_send_terminal_model->currentText()
                    && other_send_terminal->terminal_shape == arg1)
            {
                ui->lineEdit_other_send_terminal_size->setText(other_send_terminal->terminal_size);
                for(int i = 0; i < 8; i++)
                {
                    terminal_noi_lineEdits[i]->setText(other_send_terminal->noi[i]);
                    terminal_atten_lineEdits[i]->setText(other_send_terminal->atten[i]);
                    terminal_refl_lineEdits[i]->setText(other_send_terminal->refl[i]);
                }
                terminal_noi_lineEdits[8]->setText(other_send_terminal->noi[8]);
            }
        }
    }
}

#pragma endregion}


/**********静压箱**********/
#pragma region "static_box"{
void RoomCalTable::static_box_atten_cal()
{
    if( ui->lineEdit_static_box_total_air_volume->text().isEmpty() ||
            ui->lineEdit_static_box_branch_air_volume->text().isEmpty())
        return;

    array<double, 8> atten =  calBranchNoise(ui->lineEdit_static_box_total_air_volume->text().toDouble(),
                                           ui->lineEdit_static_box_branch_air_volume->text().toDouble());

    for(int i = 0; i < atten.size(); i++)
    {
        atten_lineEdits[i]->setText(QString::number(atten[i],'f', 1));
    }
}

void RoomCalTable::on_comboBox_static_box_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    if(arg1 != "经验公式")
    {
        ui->comboBox_static_box_model->setEnabled(true);
        ui->comboBox_static_box_model->setStyleSheet("QComboBox { border:none;"
                                                          "border-bottom:1px solid black;}");

        ui->lineEdit_static_box_total_air_volume->setReadOnly(true);
        ui->lineEdit_static_box_branch_air_volume->setReadOnly(true);

        disconnect(ui->lineEdit_static_box_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::static_box_atten_cal);
        disconnect(ui->lineEdit_static_box_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::static_box_atten_cal);

        clearPage(ui->stackedWidget_table->currentWidget(), false);
        currentComponentListByDataSource.clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto static_box = qSharedPointerDynamicCast<Static_box>(component))
            {
                if(static_box->data_source == arg1)
                    currentComponentListByDataSource.append(component); // 直接添加component
            }
        }

        ui->comboBox_static_box_model->blockSignals(true);

        for(auto& component : currentComponentListByDataSource)
        {
            if(auto static_box = qSharedPointerDynamicCast<Static_box>(component))
                ui->comboBox_static_box_model->addItem(static_box->model);
        }
        ui->comboBox_static_box_model->setCurrentIndex(-1);

        ui->comboBox_static_box_model->blockSignals(false);
    }
    else
    {
        ui->comboBox_static_box_model->setCurrentText("-");
        ui->comboBox_static_box_model->setEnabled(false);
        ui->comboBox_static_box_model->setStyleSheet("QComboBox { border:1px solid black ;background-color: rgb(240,240,240);"
                                                          "font-family: '黑体';"
                                                          "font-size: 14px;}");

        ui->lineEdit_static_box_total_air_volume->setReadOnly(false);
        ui->lineEdit_static_box_branch_air_volume->setReadOnly(false);

        connect(ui->lineEdit_static_box_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::static_box_atten_cal);
        connect(ui->lineEdit_static_box_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::static_box_atten_cal);
    }

}

void RoomCalTable::on_comboBox_static_box_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_static_box_model->currentIndex() == -1)
        return;

    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto static_box = qSharedPointerDynamicCast<Static_box>(component);
        if(static_box && static_box->model == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto static_box = qSharedPointerDynamicCast<Static_box>(*it))
        {
            currentComponent = (*it);
            ui->lineEdit_static_box_total_air_volume->setText(static_box->q);
            ui->lineEdit_static_box_branch_air_volume->setText(static_box->q1);

            for(int i = 0; i < static_box->atten.size(); i++)
            {
                atten_lineEdits[i]->setText(static_box->atten[i]);
            }
        }
    }
}

#pragma endregion}
/**********静压箱**********/

/**********风道多分支**********/
#pragma region "multi_ranc"{
void RoomCalTable::multi_ranc_atten_cal()
{
    if( ui->lineEdit_multi_ranc_total_air_volume->text().isEmpty() ||
            ui->lineEdit_multi_ranc_branch_air_volume->text().isEmpty())
        return;

    array<double, 8> atten =  calBranchNoise(ui->lineEdit_multi_ranc_total_air_volume->text().toDouble(),
                                           ui->lineEdit_multi_ranc_branch_air_volume->text().toDouble());

    for(int i = 0; i < atten.size(); i++)
    {
        atten_lineEdits[i]->setText(QString::number(atten[i],'f', 1));
    }
}

void RoomCalTable::on_comboBox_multi_ranc_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    if(arg1 != "经验公式")
    {
        ui->comboBox_multi_ranc_model->setEnabled(true);
        ui->comboBox_multi_ranc_model->setStyleSheet("QComboBox { border:none;"
                                                          "border-bottom:1px solid black;}");

        ui->lineEdit_multi_ranc_total_air_volume->setReadOnly(true);
        ui->lineEdit_multi_ranc_branch_air_volume->setReadOnly(true);

        disconnect(ui->lineEdit_multi_ranc_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::multi_ranc_atten_cal);
        disconnect(ui->lineEdit_multi_ranc_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::multi_ranc_atten_cal);

        clearPage(ui->stackedWidget_table->currentWidget(), false);
        currentComponentListByDataSource.clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto multi_ranc = qSharedPointerDynamicCast<Multi_ranc>(component))
            {
                if(multi_ranc->data_source == arg1)
                    currentComponentListByDataSource.append(component); // 直接添加component
            }
        }

        ui->comboBox_multi_ranc_model->blockSignals(true);

        for(auto& component : currentComponentListByDataSource)
        {
            if(auto multi_ranc = qSharedPointerDynamicCast<Multi_ranc>(component))
                ui->comboBox_multi_ranc_model->addItem(multi_ranc->model);
        }
        ui->comboBox_multi_ranc_model->setCurrentIndex(-1);

        ui->comboBox_multi_ranc_model->blockSignals(false);
    }
    else
    {
        ui->comboBox_multi_ranc_model->setCurrentText("-");
        ui->comboBox_multi_ranc_model->setEnabled(false);
        ui->comboBox_multi_ranc_model->setStyleSheet("QComboBox { border:1px solid black ;background-color: rgb(240,240,240);"
                                                          "font-family: '黑体';"
                                                          "font-size: 14px;}");

        ui->lineEdit_multi_ranc_total_air_volume->setReadOnly(false);
        ui->lineEdit_multi_ranc_branch_air_volume->setReadOnly(false);

        connect(ui->lineEdit_multi_ranc_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::multi_ranc_atten_cal);
        connect(ui->lineEdit_multi_ranc_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::multi_ranc_atten_cal);
    }
}

void RoomCalTable::on_comboBox_multi_ranc_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_multi_ranc_model->currentIndex() == -1)
        return;

    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto multi_ranc = qSharedPointerDynamicCast<Multi_ranc>(component);
        if(multi_ranc && multi_ranc->model == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto multi_ranc = qSharedPointerDynamicCast<Multi_ranc>(*it))
        {
            currentComponent = (*it);
            ui->lineEdit_multi_ranc_total_air_volume->setText(multi_ranc->q);
            ui->lineEdit_multi_ranc_branch_air_volume->setText(multi_ranc->q1);

            for(int i = 0; i < multi_ranc->atten.size(); i++)
            {
                atten_lineEdits[i]->setText(multi_ranc->atten[i]);
            }
        }
    }
}


#pragma endregion}
/**********风道多分支**********/

/**********三通**********/
#pragma region "tee"{
void RoomCalTable::tee_atten_cal()
{
    if( ui->lineEdit_tee_total_air_volume->text().isEmpty() ||
            ui->lineEdit_tee_branch_air_volume->text().isEmpty())
        return;

    array<double, 8> atten =  calBranchNoise(ui->lineEdit_tee_total_air_volume->text().toDouble(),
                                           ui->lineEdit_tee_branch_air_volume->text().toDouble());

    for(int i = 0; i < atten.size(); i++)
    {
        atten_lineEdits[i]->setText(QString::number(atten[i],'f', 1));
    }
}

void RoomCalTable::on_comboBox_tee_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    if(arg1 != "经验公式")
    {
        ui->comboBox_tee_model->setEnabled(true);
        ui->comboBox_tee_model->setStyleSheet("QComboBox { border:none;"
                                                          "border-bottom:1px solid black;}");

        ui->lineEdit_tee_total_air_volume->setReadOnly(true);
        ui->lineEdit_tee_branch_air_volume->setReadOnly(true);

        disconnect(ui->lineEdit_tee_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::tee_atten_cal);
        disconnect(ui->lineEdit_tee_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::tee_atten_cal);

        clearPage(ui->stackedWidget_table->currentWidget(), false);
        currentComponentListByDataSource.clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto tee = qSharedPointerDynamicCast<Tee>(component))
            {
                if(tee->data_source == arg1)
                    currentComponentListByDataSource.append(component); // 直接添加component
            }
        }

        ui->comboBox_tee_model->blockSignals(true);

        for(auto& component : currentComponentListByDataSource)
        {
            if(auto tee = qSharedPointerDynamicCast<Tee>(component))
                ui->comboBox_tee_model->addItem(tee->model);
        }
        ui->comboBox_tee_model->setCurrentIndex(-1);

        ui->comboBox_tee_model->blockSignals(false);
    }
    else
    {
        ui->comboBox_tee_model->setCurrentText("-");
        ui->comboBox_tee_model->setEnabled(false);
        ui->comboBox_tee_model->setStyleSheet("QComboBox { border:1px solid black ;background-color: rgb(240,240,240);"
                                                          "font-family: '黑体';"
                                                          "font-size: 14px;}");

        ui->lineEdit_tee_total_air_volume->setReadOnly(false);
        ui->lineEdit_tee_branch_air_volume->setReadOnly(false);

        connect(ui->lineEdit_tee_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::tee_atten_cal);
        connect(ui->lineEdit_tee_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::tee_atten_cal);
    }
}

void RoomCalTable::on_comboBox_tee_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_tee_model->currentIndex() == -1)
        return;

    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto tee = qSharedPointerDynamicCast<Tee>(component);
        if(tee && tee->model == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto tee = qSharedPointerDynamicCast<Tee>(*it))
        {
            currentComponent = (*it);
            ui->lineEdit_tee_total_air_volume->setText(tee->q);
            ui->lineEdit_tee_branch_air_volume->setText(tee->q1);

            for(int i = 0; i < tee->atten.size(); i++)
            {
                atten_lineEdits[i]->setText(tee->atten[i]);
            }
        }
    }
}
#pragma endregion}
/**********三通**********/

/**********直管**********/
#pragma region "pipe"{
void RoomCalTable::pipe_atten_cal()
{
    if( ui->lineEdit_pipe_size->text().isEmpty() ||
            ui->comboBox_pipe_type->currentIndex() == -1)
        return;

    array<double, 8> atten;
    if(ui->comboBox_pipe_type->currentIndex() == 0)
    {
        atten = caPipeNoise(Circle, ui->lineEdit_pipe_size->text().toDouble());
    }
    else{
        auto dimensions = splitDimension(ui->lineEdit_rect_damper_size->text());
        double length, width;
        if (dimensions.first != -1) {
            length = dimensions.first;
            width = dimensions.second;
        }
        atten =  caPipeNoise(Rect, length, width);
    }


    for(int i = 0; i < atten.size(); i++)
    {
        each_atten_lineEdits[i]->setText(QString::number(atten[i],'f', 2));
    }
}

void RoomCalTable::on_comboBox_pipe_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    if(arg1 != "经验公式")
    {
        ui->comboBox_pipe_model->setEnabled(true);
        ui->comboBox_pipe_model->setStyleSheet("QComboBox { border:none;"
                                                          "border-bottom:1px solid black;}");

        ui->lineEdit_pipe_size->setReadOnly(true);

        disconnect(ui->lineEdit_pipe_size, &QLineEdit::textChanged, this, &RoomCalTable::pipe_atten_cal);

        currentComponentListByDataSource.clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto pipe = qSharedPointerDynamicCast<Pipe>(component))
            {
                if(pipe->data_source == arg1)
                    currentComponentListByDataSource.append(component); // 直接添加component
            }
        }

        ui->comboBox_pipe_model->blockSignals(true);

        for(auto& component : currentComponentListByDataSource)
        {
            if(auto pipe = qSharedPointerDynamicCast<Pipe>(component))
                ui->comboBox_pipe_model->addItem(pipe->model);
        }
        ui->comboBox_pipe_model->setCurrentIndex(-1);

        ui->comboBox_pipe_model->blockSignals(false);
    }
    else
    {
        ui->comboBox_pipe_model->setCurrentText("-");
        ui->comboBox_pipe_model->setEnabled(false);
        ui->comboBox_pipe_model->setStyleSheet("QComboBox { border:1px solid black ;background-color: rgb(240,240,240);"
                                                          "font-family: '黑体';"
                                                          "font-size: 14px;}");

        ui->lineEdit_pipe_size->setReadOnly(false);

        connect(ui->lineEdit_pipe_size, &QLineEdit::textChanged, this, &RoomCalTable::pipe_atten_cal);
    }
}

void RoomCalTable::on_lineEdit_pipe_size_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    if(ui->comboBox_pipe_type->currentIndex() == 0)
    {
        double perimeter;
        perimeter = 2* Pi * ( arg1.toDouble() / 2 );
        ui->lineEdit_pipe_perimeter->setText(QString::number(perimeter, 'f', 1));
    }
    else
    {
        auto dimensions = splitDimension(ui->lineEdit_rect_damper_size->text());
        double length, width;
        if (dimensions.first != -1) {
            length = dimensions.first;
            width = dimensions.second;
        }
        ui->lineEdit_pipe_perimeter->setText(QString::number(length * 2 + width * 2, 'f', 1));
    }
}

void RoomCalTable::on_lineEdit_pipe_length_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;
    for(int i = 0; i < 8; i++)
    {
        if(each_atten_lineEdits[i]->text().isEmpty())
            return;
    }

    for(int i = 0; i < 8; i++)
    {
        sum_atten_lineEdits[i]->setText(QString::number(each_atten_lineEdits[i]->text().toDouble() * arg1.toDouble(), 'f' ,2));
    }
}

void RoomCalTable::on_comboBox_pipe_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_pipe_model->currentIndex() == -1)
        return;

    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto pipe = qSharedPointerDynamicCast<Pipe>(component);
        if(pipe && pipe->model == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto pipe = qSharedPointerDynamicCast<Pipe>(*it))
        {
            currentComponent = (*it);
            for(int i = 0; i < pipe->atten.size(); i++)
            {
                each_atten_lineEdits[i]->setText(pipe->atten[i]);
            }
            ui->comboBox_pipe_type->setCurrentText(pipe->pipe_shape);
            ui->lineEdit_pipe_size->setText(pipe->size);
        }
    }
}
#pragma endregion}
/**********直管**********/

/**********弯头**********/
#pragma region "elbow"{
void RoomCalTable::elbow_atten_cal()
{
    if( ui->lineEdit_elbow_size->text().isEmpty() ||
            ui->comboBox_elbow_type->currentIndex() == -1)
        return;

    array<double, 8> atten;
    if(ui->comboBox_elbow_type->currentIndex() == 0)
    {
        atten = calElbowNoise(Circle, ui->lineEdit_elbow_size->text().toDouble());
    }
    else if(ui->comboBox_elbow_type->currentIndex() == 1){
        atten = calElbowNoise(RectangleUnlined, ui->lineEdit_elbow_size->text().toDouble());
    }
    else if(ui->comboBox_elbow_type->currentIndex() == 2){
        atten = calElbowNoise(RectangleLined, ui->lineEdit_elbow_size->text().toDouble());
    }

    for(int i = 0; i < atten.size(); i++)
    {
        each_atten_lineEdits[i]->setText(QString::number(atten[i],'f', 2));
    }
}

void RoomCalTable::on_comboBox_elbow_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    if(arg1 != "经验公式")
    {
        ui->comboBox_elbow_model->setEnabled(true);
        ui->comboBox_elbow_model->setStyleSheet("QComboBox { border:none;"
                                                          "border-bottom:1px solid black;}");

        ui->lineEdit_elbow_size->setReadOnly(true);

        disconnect(ui->lineEdit_elbow_size, &QLineEdit::textChanged, this, &RoomCalTable::elbow_atten_cal);

        currentComponentListByDataSource.clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto elbow = qSharedPointerDynamicCast<Elbow>(component))
            {
                if(elbow->data_source == arg1)
                    currentComponentListByDataSource.append(component); // 直接添加component
            }
        }

        ui->comboBox_elbow_model->blockSignals(true);

        for(auto& component : currentComponentListByDataSource)
        {
            if(auto elbow = qSharedPointerDynamicCast<Elbow>(component))
                ui->comboBox_elbow_model->addItem(elbow->model);
        }
        ui->comboBox_elbow_model->setCurrentIndex(-1);

        ui->comboBox_elbow_model->blockSignals(false);
    }
    else
    {
        ui->comboBox_elbow_model->setCurrentText("-");
        ui->comboBox_elbow_model->setEnabled(false);
        ui->comboBox_elbow_model->setStyleSheet("QComboBox { border:1px solid black ;background-color: rgb(240,240,240);"
                                                          "font-family: '黑体';"
                                                          "font-size: 14px;}");

        ui->lineEdit_elbow_size->setReadOnly(false);

        connect(ui->lineEdit_elbow_size, &QLineEdit::textChanged, this, &RoomCalTable::elbow_atten_cal);
    }
}

void RoomCalTable::on_comboBox_elbow_type_currentTextChanged(const QString &arg1)
{
    elbow_atten_cal();
}

void RoomCalTable::on_lineEdit_elbow_count_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;
    for(int i = 0; i < 8; i++)
    {
        if(each_atten_lineEdits[i]->text().isEmpty())
            return;
    }

    for(int i = 0; i < 8; i++)
    {
        sum_atten_lineEdits[i]->setText(QString::number(each_atten_lineEdits[i]->text().toDouble() * arg1.toDouble(), 'f' ,2));
    }
}

void RoomCalTable::on_comboBox_elbow_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_elbow_model->currentIndex() == -1)
        return;

    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto elbow = qSharedPointerDynamicCast<Elbow>(component);
        if(elbow && elbow->model == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto elbow = qSharedPointerDynamicCast<Elbow>(*it))
        {
            currentComponent = (*it);
            for(int i = 0; i < elbow->atten.size(); i++)
            {
                each_atten_lineEdits[i]->setText(elbow->atten[i]);
            }
            ui->comboBox_elbow_type->setCurrentText(elbow->elbow_shape);
            ui->lineEdit_elbow_size->setText(elbow->size);
        }
    }
}



#pragma endregion}

/**********变径**********/
#pragma region "reducer"{
void RoomCalTable::reducer_atten_cal()
{
    if(ui->comboBox_reducer_type->currentIndex() == -1 ||
            ui->lineEdit_reducer_before_size->text().isEmpty() ||
            ui->lineEdit_reducer_after_size->text().isEmpty())
        return;

    array<double,8> atten;
    if(ui->comboBox_reducer_type->currentText() == "圆-圆")
    {
        atten = calReducerNoise(ui->comboBox_reducer_type->currentText(),
                                ui->lineEdit_reducer_before_size->text().toDouble(),
                                -1,
                                ui->lineEdit_reducer_after_size->text().toDouble(),
                                -1);
    }
    else if(ui->comboBox_reducer_type->currentText() == "方-方")
    {
        auto dimensions_before = splitDimension(ui->lineEdit_reducer_before_size->text());
        double length_before, width_before;
        if (dimensions_before.first != -1) {
            length_before = dimensions_before.first;
            width_before = dimensions_before.second;
        }

        auto dimensions_after = splitDimension(ui->lineEdit_reducer_after_size->text());
        double length_after, width_after;
        if (dimensions_after.first != -1) {
            length_after = dimensions_after.first;
            width_after = dimensions_after.second;
        }

        atten =  calReducerNoise(ui->comboBox_reducer_type->currentText(),
                                 length_before,
                                 width_before,
                                 length_after,
                                 width_after);
    }
    else if((ui->comboBox_reducer_type->currentText() == "方-圆"))
    {
        auto dimensions_before = splitDimension(ui->lineEdit_reducer_before_size->text());
        double length_before, width_before;
        if (dimensions_before.first != -1) {
            length_before = dimensions_before.first;
            width_before = dimensions_before.second;
        }
        atten =  calReducerNoise(ui->comboBox_reducer_type->currentText(),
                                 length_before,
                                 width_before,
                                 ui->lineEdit_reducer_after_size->text().toDouble(),
                                 -1);
    }
    else if((ui->comboBox_reducer_type->currentText() == "圆-方"))
    {
        auto dimensions_after = splitDimension(ui->lineEdit_reducer_after_size->text());
        double length_after, width_after;
        if (dimensions_after.first != -1) {
            length_after = dimensions_after.first;
            width_after = dimensions_after.second;
        }
        atten =  calReducerNoise(ui->comboBox_reducer_type->currentText(),
                                 ui->lineEdit_reducer_before_size->text().toDouble(),
                                 -1,
                                 length_after,
                                 width_after);
    }

    for(int i = 0; i < atten.size(); i++)
    {
        atten_lineEdits[i]->setText(QString::number(atten[i],'f', 1));
    }
}

void RoomCalTable::on_comboBox_reducer_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    if(arg1 != "经验公式")
    {
        ui->comboBox_reducer_model->setEnabled(true);
        ui->comboBox_reducer_model->setStyleSheet("QComboBox { border:none;"
                                                          "border-bottom:1px solid black;}");

        ui->lineEdit_reducer_before_size->setReadOnly(true);
        ui->lineEdit_reducer_after_size->setReadOnly(true);

        disconnect(ui->lineEdit_reducer_before_size, &QLineEdit::textChanged, this, &RoomCalTable::reducer_atten_cal);
        disconnect(ui->lineEdit_reducer_after_size, &QLineEdit::textChanged, this, &RoomCalTable::reducer_atten_cal);

        currentComponentListByDataSource.clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto reducer = qSharedPointerDynamicCast<Reducer>(component))
            {
                if(reducer->data_source == arg1)
                    currentComponentListByDataSource.append(component); // 直接添加component
            }
        }

        ui->comboBox_reducer_model->blockSignals(true);

        for(auto& component : currentComponentListByDataSource)
        {
            if(auto reducer = qSharedPointerDynamicCast<Reducer>(component))
                ui->comboBox_reducer_model->addItem(reducer->model);
        }
        ui->comboBox_reducer_model->setCurrentIndex(-1);

        ui->comboBox_reducer_model->blockSignals(false);
    }
    else
    {
        ui->comboBox_reducer_model->setCurrentText("-");
        ui->comboBox_reducer_model->setEnabled(false);
        ui->comboBox_reducer_model->setStyleSheet("QComboBox { border:1px solid black ;background-color: rgb(240,240,240);"
                                                          "font-family: '黑体';"
                                                          "font-size: 14px;}");

        ui->lineEdit_reducer_before_size->setReadOnly(false);
        ui->lineEdit_reducer_after_size->setReadOnly(false);

        connect(ui->lineEdit_reducer_before_size, &QLineEdit::textChanged, this, &RoomCalTable::reducer_atten_cal);
        connect(ui->lineEdit_reducer_after_size, &QLineEdit::textChanged, this, &RoomCalTable::reducer_atten_cal);
    }
}

void RoomCalTable::on_comboBox_reducer_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_reducer_model->currentIndex() == -1)
        return;

    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto reducer = qSharedPointerDynamicCast<Reducer>(component);
        if(reducer && reducer->model == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto reducer = qSharedPointerDynamicCast<Reducer>(*it))
        {
            currentComponent = (*it);
            for(int i = 0; i < reducer->atten.size(); i++)
            {
                atten_lineEdits[i]->setText(reducer->atten[i]);
            }
            ui->comboBox_reducer_type->setCurrentText(reducer->reducer_type);
            ui->lineEdit_reducer_before_size->setText(reducer->reducer_before_size);
            ui->lineEdit_reducer_after_size->setText(reducer->reducer_after_size);
        }
    }
}
#pragma endregion}
/**********变径**********/

/**********消音器**********/
#pragma region "reducer"{
void RoomCalTable::on_comboBox_silencer_data_source_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);
    currentComponentListByDataSource.clear();
    for(auto& component : currentAllComponentList)
    {
        // 尝试将component转换为Silencer类型的智能指针
        if(auto silencer = qSharedPointerDynamicCast<Silencer>(component))
        {
            if(silencer->data_source == arg1)
                currentComponentListByDataSource.append(component); // 直接添加component
        }
    }
}

void RoomCalTable::on_comboBox_silencer_type_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);

    ui->comboBox_silencer_model->blockSignals(true);

    for(auto& component : currentComponentListByDataSource)
    {
        if(auto silencer = qSharedPointerDynamicCast<Silencer>(component))
            if(silencer->silencer_type == ui->comboBox_silencer_type->currentText())
                ui->comboBox_silencer_model->addItem(silencer->model);
    }
    ui->comboBox_silencer_model->setCurrentIndex(-1);

    ui->comboBox_silencer_model->blockSignals(false);
}

void RoomCalTable::on_comboBox_silencer_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_silencer_model->currentIndex() == -1)
        return;
    auto it = std::find_if(this->currentComponentListByDataSource.begin(),this->currentComponentListByDataSource.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto silencer = qSharedPointerDynamicCast<Silencer>(component);
        if(silencer && silencer->model == arg1)
            return true;
        return false;
    });

    if(it != currentComponentListByDataSource.end())
    {
        if(auto silencer = qSharedPointerDynamicCast<Silencer>(*it))
        {
            currentComponent = (*it);

            for(int i = 0; i < silencer->atten.size(); i++)
            {
                atten_lineEdits[i]->setText(silencer->atten[i]);
            }
        }
    }

}
#pragma endregion}
/**********消音器**********/

