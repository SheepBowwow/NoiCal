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
#include "Component/ComponentManager.h"
#include "roomDefineForm/roomcalinfomanager.h"
#include "calFunction/cal_function.h"
#include "global_constant.h"
#include <utility> // For std::pair
#include <QJsonObject>
#define Pi 3.14159265358979323846

RoomCalTable::RoomCalTable(QString systemName, QWidget *parent, QString currentTableType) :
    QWidget(parent),
    systemName(systemName),
    isCollapsed(false),
    ui(new Ui::RoomCalTable)
{   
    ui->setupUi(this);
    registerSetTableInfoFuncMap();
    registerCreateTableInfoJsonFuncMap();
    initRoomCalSlotFuncConn();

    noi_after_cal_lineEdits.clear();
    noi_lineEdits.clear();       //噪音
    terminal_atten_lineEdits.clear();       //末端衰减
    terminal_refl_lineEdits.clear();       //末端反射衰减
    terminal_noi_lineEdits.clear();       //末端气流噪音
    each_atten_lineEdits.clear();       //1米直管或单个弯头衰减
    sum_atten_lineEdits.clear();       //衰减汇总
    atten_lineEdits.clear();       //衰减
    noi_input_room_lineEdits.clear();       //进入房间噪音声功率(dB)
    room_noi_revise_lineEdits.clear();       // 房间声压与声功率修正(dB)
    test_point_noi_lineEdits.clear();       //衰减
    A_weighted_noi_lineEdit = nullptr;                //A级计权噪音

    //connect(&ComponentManager::getInstance(), &ComponentManager::componentsUpdate, this, &RoomCalTable::updateModelComboBoxItems);

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

    if(currentTableType == "" || currentTableType == "噪音源")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_unit);
        ui->comboBox_sound_type->setCurrentText("噪音源");
        ui->comboBox_unit_name->setCurrentIndex(0);
        ui->stackedWidget_table->setCurrentWidget(ui->page_fan);
        this->currentTableType = "噪音源";
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
        on_comboBox_room_type_currentTextChanged(ui->comboBox_room_type->currentText());
    }
    else
    {
        this->isValid = false;
    }
    if(!(currentTableType == "声压级计算"))
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
        this->currentUnitType = ui->comboBox_unit_name->currentText().mid(ui->comboBox_unit_name->currentText().indexOf("/") + 1);
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

    for(QLineEdit*& lineEdit : noi_after_cal_lineEdits)
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

    QVector<QLineEdit*>* A_weighted = new QVector<QLineEdit*>(1, A_weighted_noi_lineEdit);
    if(!A_weighted_noi_lineEdit)
        A_weighted->clear();

    QVector<QVector<QLineEdit*>*> allLineEdits = {
        &noi_lineEdits,
        &terminal_atten_lineEdits,
        &terminal_refl_lineEdits,
        &terminal_noi_lineEdits,
        &each_atten_lineEdits,
        &sum_atten_lineEdits,
        &atten_lineEdits,
        &noi_input_room_lineEdits,
        &room_noi_revise_lineEdits,
        &test_point_noi_lineEdits,
        A_weighted
    };

    for (QVector<QLineEdit*>* container : qAsConst(allLineEdits))
    {
        if (!container->isEmpty())
        {  // 检查容器是否非空
            for (QLineEdit*& lineEdit : *container)
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
    this->currentTableType = arg1;
    ui->stackedWidget_title->setCurrentWidget(ui->page_unit);
    if(arg1 == "噪音源")
    {
        ui->comboBox_unit_name->clear();
        ui->comboBox_unit_name->addItem("风机");
        ui->comboBox_unit_name->addItem("风机盘管");
        ui->comboBox_unit_name->addItem("空调器(单风机)");
        ui->comboBox_unit_name->addItem("空调器(双风机)");
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
        on_comboBox_room_type_currentTextChanged(ui->comboBox_room_type->currentText());
    }
}

void RoomCalTable::on_comboBox_unit_name_currentTextChanged(const QString &arg1)
{
    int index = arg1.indexOf("/");
    this->currentUnitType = arg1.mid(index + 1);
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
        ui->comboBox_fan_number->blockSignals(true);
        ui->comboBox_fan_number->clear();

        QList<QString> list =
            RoomCalInfoManager::getInstance().getCompUUIDsFromSystemList(systemName, noise_src_component::FAN);
        for(auto& uuid : list)
        {
            auto component = ComponentManager::getInstance().findComponent(false, uuid);
            if(auto fan = dynamic_cast<Fan*>(component.data()))
            {
                this->currentAllComponentList.append(component);
                ui->comboBox_fan_number->addItem(fan->number);
            }
        }
        ui->comboBox_fan_number->setCurrentIndex(-1);
        noi_after_cal_lineEdits = { ui->lineEdit_fan_63, ui->lineEdit_fan_125, ui->lineEdit_fan_250,ui->lineEdit_fan_500,
                                   ui->lineEdit_fan_1k, ui->lineEdit_fan_2k, ui->lineEdit_fan_4k, ui->lineEdit_fan_8k,
                                   ui->lineEdit_fan_total };

        ui->stackedWidget_table->setCurrentWidget(ui->page_fan);
        ui->comboBox_fan_number->setCurrentIndex(-1);
        ui->comboBox_fan_number->blockSignals(false);
    }
    else if(arg1 == "空调器(单风机)" || arg1 == "空调器(双风机)")
    {
        ui->comboBox_aircondition_number->blockSignals(true);
        ui->comboBox_aircondition_number->clear();

        noi_after_cal_lineEdits = { ui->lineEdit_aircondition_63, ui->lineEdit_aircondition_125, ui->lineEdit_aircondition_250,ui->lineEdit_aircondition_500,
                                   ui->lineEdit_aircondition_1k, ui->lineEdit_aircondition_2k, ui->lineEdit_aircondition_4k, ui->lineEdit_aircondition_8k,
                                   ui->lineEdit_aircondition_total };

        if(arg1 == "空调器(单风机)"){
            QList<QString> list_single =
                RoomCalInfoManager::getInstance().getCompUUIDsFromSystemList(systemName, noise_src_component::AIRCONDITION_SINGLE);
            for(auto& uuid : list_single)
            {
                auto component = ComponentManager::getInstance().findComponent(false, uuid);
                if(auto aircondition = dynamic_cast<Aircondition*>(component.data()))
                {
                    this->currentAllComponentList.append(component);
                    if(ui->comboBox_aircondition_number->findText(aircondition->send_number) == -1)
                        ui->comboBox_aircondition_number->addItem(aircondition->send_number);
                }
            }
        }
        else if(arg1 == "空调器(双风机)") {
            QList<QString> list_double =
                RoomCalInfoManager::getInstance().getCompUUIDsFromSystemList(systemName,noise_src_component::AIRCONDITION_DOUBLE);
            for(auto& uuid : list_double)
            {
                auto component = ComponentManager::getInstance().findComponent(false, uuid);
                if(auto aircondition = dynamic_cast<Aircondition*>(component.data()))
                {
                    this->currentAllComponentList.append(component);
                    if(ui->comboBox_aircondition_number->findText(aircondition->send_number) == -1)
                        ui->comboBox_aircondition_number->addItem(aircondition->send_number);
                    if(ui->comboBox_aircondition_number->findText(aircondition->exhaust_number) == -1)
                        ui->comboBox_aircondition_number->addItem(aircondition->exhaust_number);
                }
            }
        }
        ui->comboBox_aircondition_number->setCurrentIndex(-1);
        ui->stackedWidget_table->setCurrentWidget(ui->page_aircondition);
        ui->comboBox_aircondition_number->blockSignals(false);
    }
    else if(arg1 == "风机盘管")
    {
        ui->comboBox_fanCoil_model->blockSignals(true);
        ui->comboBox_fanCoil_model->clear();
        QList<QString> list =
            RoomCalInfoManager::getInstance().getCompUUIDsFromSystemList(systemName, noise_src_component::FANCOIL);
        for(auto& uuid : list)
        {
            auto component = ComponentManager::getInstance().findComponent(false, uuid);
            if(auto fanCoil = dynamic_cast<FanCoil*>(component.data()))
            {
                this->currentAllComponentList.append(component);
                ui->comboBox_fanCoil_model->addItem(fanCoil->model);
            }
        }
        ui->comboBox_fanCoil_model->setCurrentIndex(-1);
        noi_after_cal_lineEdits = { ui->lineEdit_fanCoil_63, ui->lineEdit_fanCoil_125, ui->lineEdit_fanCoil_250,ui->lineEdit_fanCoil_500,
                                   ui->lineEdit_fanCoil_1k, ui->lineEdit_fanCoil_2k, ui->lineEdit_fanCoil_4k, ui->lineEdit_fanCoil_8k,
                                   ui->lineEdit_fanCoil_total };

        ui->stackedWidget_table->setCurrentWidget(ui->page_fanCoil);
        ui->comboBox_fanCoil_model->setCurrentIndex(-1);
        ui->comboBox_fanCoil_model->blockSignals(false);
    }
    else if(arg1 == "阀门/变风量末端")
    {
        ui->comboBox_VAV_terminal_model->blockSignals(true);
        ui->comboBox_VAV_terminal_model->clear();
        noi_after_cal_lineEdits = { ui->lineEdit_VAV_terminal_after_63, ui->lineEdit_VAV_terminal_after_125, ui->lineEdit_VAV_terminal_after_250,ui->lineEdit_VAV_terminal_after_500,
                                   ui->lineEdit_VAV_terminal_after_1k, ui->lineEdit_VAV_terminal_after_2k, ui->lineEdit_VAV_terminal_after_4k, ui->lineEdit_VAV_terminal_after_8k,
                                   ui->lineEdit_VAV_terminal_after_total};

        noi_lineEdits = { ui->lineEdit_VAV_terminal_63, ui->lineEdit_VAV_terminal_125, ui->lineEdit_VAV_terminal_250,ui->lineEdit_VAV_terminal_500,
                         ui->lineEdit_VAV_terminal_1k, ui->lineEdit_VAV_terminal_2k, ui->lineEdit_VAV_terminal_4k, ui->lineEdit_VAV_terminal_8k,
                         ui->lineEdit_VAV_terminal_total};

        this->currentAllComponentList =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::VAV_TERMINAL);

        for(auto& component : this->currentAllComponentList)
        {
            if(auto vav = dynamic_cast<VAV_terminal*>(component.data()))
            {
                ui->comboBox_VAV_terminal_model->addItem(vav->model);
            }
        }
        ui->comboBox_VAV_terminal_model->setCurrentIndex(-1);
        ui->stackedWidget_table->setCurrentWidget(ui->page_VAV_terminal);
        ui->comboBox_VAV_terminal_model->blockSignals(false);
    }
    else if(arg1 == "阀门/圆形调风门")
    {
        ui->comboBox_circular_damper_model->blockSignals(true);
        noi_after_cal_lineEdits = { ui->lineEdit_circular_damper_after_63, ui->lineEdit_circular_damper_after_125, ui->lineEdit_circular_damper_after_250,ui->lineEdit_circular_damper_after_500,
                                   ui->lineEdit_circular_damper_after_1k, ui->lineEdit_circular_damper_after_2k, ui->lineEdit_circular_damper_after_4k, ui->lineEdit_circular_damper_after_8k,
                                   ui->lineEdit_circular_damper_after_total};

        noi_lineEdits = { ui->lineEdit_circular_damper_63, ui->lineEdit_circular_damper_125, ui->lineEdit_circular_damper_250,ui->lineEdit_circular_damper_500,
                         ui->lineEdit_circular_damper_1k, ui->lineEdit_circular_damper_2k, ui->lineEdit_circular_damper_4k, ui->lineEdit_circular_damper_8k,
                         ui->lineEdit_circular_damper_total};

        this->currentAllComponentList =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::CIRCULAR_DAMPER);
        ui->comboBox_circular_damper_model->clear();
        ui->comboBox_circular_damper_model->addItem("经验公式");
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto circluar_damper = qSharedPointerDynamicCast<Circular_damper>(component))
            {
                ui->comboBox_circular_damper_model->addItem(circluar_damper->model);
            }
        }
        ui->comboBox_circular_damper_model->setCurrentIndex(-1);
        ui->comboBox_circular_damper_model->blockSignals(false);
        ui->stackedWidget_table->setCurrentWidget(ui->page_circular_damper);
    }
    else if(arg1 == "阀门/方形调风门")
    {
        ui->comboBox_rect_damper_model->blockSignals(true);
        noi_after_cal_lineEdits = { ui->lineEdit_rect_damper_after_63, ui->lineEdit_rect_damper_after_125, ui->lineEdit_rect_damper_after_250,ui->lineEdit_rect_damper_after_500,
                                   ui->lineEdit_rect_damper_after_1k, ui->lineEdit_rect_damper_after_2k, ui->lineEdit_rect_damper_after_4k, ui->lineEdit_rect_damper_after_8k,
                                   ui->lineEdit_rect_damper_after_total};

        noi_lineEdits = { ui->lineEdit_rect_damper_63, ui->lineEdit_rect_damper_125, ui->lineEdit_rect_damper_250,ui->lineEdit_rect_damper_500,
                         ui->lineEdit_rect_damper_1k, ui->lineEdit_rect_damper_2k, ui->lineEdit_rect_damper_4k, ui->lineEdit_rect_damper_8k,
                         ui->lineEdit_rect_damper_total};


        this->currentAllComponentList =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::RECT_DAMPER);

        ui->comboBox_rect_damper_model->clear();
        ui->comboBox_rect_damper_model->addItem("经验公式");
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto rect_damper = qSharedPointerDynamicCast<Rect_damper>(component))
            {
                ui->comboBox_rect_damper_model->addItem(rect_damper->model);
            }
        }
        ui->comboBox_rect_damper_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_rect_damper);
        ui->comboBox_rect_damper_model->blockSignals(false);
    }
    else if(arg1 == "末端/布风器+散流器")
    {
        ui->comboBox_air_distributor_model->blockSignals(true);
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
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::AIRDIFF);

        ui->comboBox_air_distributor_model->clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto air_diff = qSharedPointerDynamicCast<AirDiff>(component))
            {
                QString model = air_diff->air_distributor_model;
                if(ui->comboBox_air_distributor_model->findText(model) == -1)
                    ui->comboBox_air_distributor_model->addItem(model);
            }
        }
        ui->comboBox_air_distributor_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_air_diff);
        ui->comboBox_air_distributor_model->blockSignals(false);
    }
    else if(arg1 == "末端/抽风头")
    {
        ui->comboBox_pump_model->blockSignals(true);
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
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::PUMPSEND);

        ui->comboBox_pump_model->clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto pump = qSharedPointerDynamicCast<PumpSend>(component))
            {
                QString model = pump->model;
                if(pump->type_pump_or_send == pump_send_type::PUMP && ui->comboBox_pump_model->findText(model) == -1)
                    ui->comboBox_pump_model->addItem(model);
            }
        }
        ui->comboBox_pump_model->setCurrentIndex(-1);


        ui->stackedWidget_table->setCurrentWidget(ui->page_pump);
        ui->comboBox_pump_model->blockSignals(false);
    }
    else if(arg1 == "末端/送风头")
    {
        ui->comboBox_send_model->blockSignals(true);
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
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::PUMPSEND);

        ui->comboBox_send_model->clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto send = qSharedPointerDynamicCast<PumpSend>(component))
            {
                QString model = send->model;
                if(send->type_pump_or_send == pump_send_type::SEND && ui->comboBox_send_model->findText(model) == -1)
                    ui->comboBox_send_model->addItem(model);
            }
        }
        ui->comboBox_send_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_send);
        ui->comboBox_send_model->blockSignals(false);
    }
    else if(arg1 == "末端/静压箱+格栅")
    {
        ui->comboBox_static_box_model->blockSignals(true);
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
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::STATICBOX_GRILLE);

        ui->comboBox_static_box_model->clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto staticBox_grille = qSharedPointerDynamicCast<StaticBox_grille>(component))
            {
                QString model = staticBox_grille->staticBox_model;
                if(ui->comboBox_static_box_model->findText(model) == -1)
                    ui->comboBox_static_box_model->addItem(model);
            }
        }
        ui->comboBox_static_box_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_staticBox_grille);
        ui->comboBox_static_box_model->blockSignals(false);
    }
    else if(arg1 == "末端/置换通风末端")
    {
        ui->comboBox_disp_vent_terminal_model->blockSignals(true);
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
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::DISP_VENT_TERMINAL);

        ui->comboBox_disp_vent_terminal_model->clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto disp = qSharedPointerDynamicCast<Disp_vent_terminal>(component))
            {
                QString model = disp->model;
                if(ui->comboBox_disp_vent_terminal_model->findText(model) == -1)
                    ui->comboBox_disp_vent_terminal_model->addItem(model);
            }
        }
        ui->comboBox_disp_vent_terminal_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_disp_vent_terminal);
        ui->comboBox_disp_vent_terminal_model->blockSignals(false);
    }
    else if(arg1 == "末端/其他通风末端")
    {
        ui->comboBox_other_send_terminal_model->blockSignals(true);
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
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::OTHER_SEND_TERMINAL);

        ui->comboBox_other_send_terminal_model->clear();
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto other = qSharedPointerDynamicCast<Other_send_terminal>(component))
            {
                QString model = other->model;
                if(ui->comboBox_other_send_terminal_model->findText(model) == -1)
                    ui->comboBox_other_send_terminal_model->addItem(model);
            }
        }
        ui->comboBox_other_send_terminal_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_other_send_terminal);
        ui->comboBox_other_send_terminal_model->blockSignals(false);
    }
    else if(arg1 == "分支/静压箱")
    {
        ui->comboBox_staticBox_model->blockSignals(true);
        noi_after_cal_lineEdits = { ui->lineEdit_static_box_after_63, ui->lineEdit_static_box_after_125, ui->lineEdit_static_box_after_250,ui->lineEdit_static_box_after_500,
                                   ui->lineEdit_static_box_after_1k, ui->lineEdit_static_box_after_2k, ui->lineEdit_static_box_after_4k, ui->lineEdit_static_box_after_8k,
                                   ui->lineEdit_static_box_after_total};

        atten_lineEdits = { ui->lineEdit_static_box_63, ui->lineEdit_static_box_125, ui->lineEdit_static_box_250,ui->lineEdit_static_box_500,
                           ui->lineEdit_static_box_1k, ui->lineEdit_static_box_2k, ui->lineEdit_static_box_4k, ui->lineEdit_static_box_8k,
                           ui->lineEdit_static_box_total};

        this->currentAllComponentList =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::STATICBOX);

        ui->comboBox_staticBox_model->clear();
        ui->comboBox_staticBox_model->addItem("经验公式");
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto staticBox = qSharedPointerDynamicCast<Static_box>(component))
            {
                QString model = staticBox->model;
                if(ui->comboBox_staticBox_model->findText(model) == -1)
                    ui->comboBox_staticBox_model->addItem(model);
            }
        }
        ui->comboBox_staticBox_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_static_box);
        ui->comboBox_staticBox_model->blockSignals(false);
    }
    else if(arg1 == "分支/风道多分支")
    {
        ui->comboBox_multi_ranc_model->blockSignals(true);
        noi_after_cal_lineEdits = { ui->lineEdit_multi_ranc_after_63, ui->lineEdit_multi_ranc_after_125, ui->lineEdit_multi_ranc_after_250,ui->lineEdit_multi_ranc_after_500,
                                   ui->lineEdit_multi_ranc_after_1k, ui->lineEdit_multi_ranc_after_2k, ui->lineEdit_multi_ranc_after_4k, ui->lineEdit_multi_ranc_after_8k,
                                   ui->lineEdit_multi_ranc_after_total};

        atten_lineEdits = { ui->lineEdit_multi_ranc_63, ui->lineEdit_multi_ranc_125, ui->lineEdit_multi_ranc_250,ui->lineEdit_multi_ranc_500,
                           ui->lineEdit_multi_ranc_1k, ui->lineEdit_multi_ranc_2k, ui->lineEdit_multi_ranc_4k, ui->lineEdit_multi_ranc_8k,
                           ui->lineEdit_multi_ranc_total};

        this->currentAllComponentList =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::MULTI_RANC);

        ui->comboBox_multi_ranc_model->clear();
        ui->comboBox_multi_ranc_model->addItem("经验公式");
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto multi = qSharedPointerDynamicCast<Multi_ranc>(component))
            {
                QString model = multi->model;
                if(ui->comboBox_multi_ranc_model->findText(model) == -1)
                    ui->comboBox_multi_ranc_model->addItem(model);
            }
        }
        ui->comboBox_multi_ranc_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_multi_ranc);
        ui->comboBox_multi_ranc_model->blockSignals(false);
    }
    else if(arg1 == "分支/三通")
    {
        ui->comboBox_tee_model->blockSignals(true);
        noi_after_cal_lineEdits = { ui->lineEdit_tee_after_63, ui->lineEdit_tee_after_125, ui->lineEdit_tee_after_250,ui->lineEdit_tee_after_500,
                                   ui->lineEdit_tee_after_1k, ui->lineEdit_tee_after_2k, ui->lineEdit_tee_after_4k, ui->lineEdit_tee_after_8k,
                                   ui->lineEdit_tee_after_total};

        atten_lineEdits = { ui->lineEdit_tee_63, ui->lineEdit_tee_125, ui->lineEdit_tee_250,ui->lineEdit_tee_500,
                           ui->lineEdit_tee_1k, ui->lineEdit_tee_2k, ui->lineEdit_tee_4k, ui->lineEdit_tee_8k,
                           ui->lineEdit_tee_total};


        this->currentAllComponentList =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::TEE);

        ui->comboBox_tee_model->clear();
        ui->comboBox_tee_model->addItem("经验公式");
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto tee = qSharedPointerDynamicCast<Tee>(component))
            {
                QString model = tee->model;
                if(ui->comboBox_tee_model->findText(model) == -1)
                    ui->comboBox_tee_model->addItem(model);
            }
        }
        ui->comboBox_tee_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_tee);
        ui->comboBox_tee_model->blockSignals(false);
    }
    else if(arg1 == "直管")
    {
        ui->comboBox_pipe_model->blockSignals(true);
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
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::PIPE);

        ui->comboBox_pipe_model->clear();
        ui->comboBox_pipe_model->addItem("经验公式");
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto pipe = qSharedPointerDynamicCast<Pipe>(component))
            {
                QString model = pipe->model;
                if(ui->comboBox_pipe_model->findText(model) == -1)
                    ui->comboBox_pipe_model->addItem(model);
            }
        }
        ui->comboBox_pipe_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_pipe);
        ui->comboBox_pipe_model->blockSignals(false);
    }
    else if(arg1 == "弯头")
    {
        ui->comboBox_elbow_model->blockSignals(true);
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
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::ELBOW);

        ui->comboBox_elbow_model->clear();
        ui->comboBox_elbow_model->addItem("经验公式");
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto elbow = qSharedPointerDynamicCast<Elbow>(component))
            {
                QString model = elbow->model;
                if(ui->comboBox_elbow_model->findText(model) == -1)
                    ui->comboBox_elbow_model->addItem(model);
            }
        }
        ui->comboBox_elbow_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_elbow);
        ui->comboBox_elbow_model->blockSignals(false);
    }
    else if(arg1 == "变径")
    {
        ui->comboBox_reducer_model->blockSignals(true);
        noi_after_cal_lineEdits = { ui->lineEdit_reducer_after_63, ui->lineEdit_reducer_after_125, ui->lineEdit_reducer_after_250,ui->lineEdit_reducer_after_500,
                                   ui->lineEdit_reducer_after_1k, ui->lineEdit_reducer_after_2k, ui->lineEdit_reducer_after_4k, ui->lineEdit_reducer_after_8k,
                                   ui->lineEdit_reducer_after_total};

        atten_lineEdits = { ui->lineEdit_reducer_63, ui->lineEdit_reducer_125, ui->lineEdit_reducer_250,ui->lineEdit_reducer_500,
                           ui->lineEdit_reducer_1k, ui->lineEdit_reducer_2k, ui->lineEdit_reducer_4k, ui->lineEdit_reducer_8k,
                           ui->lineEdit_reducer_total};

        this->currentAllComponentList =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::REDUCER);

        ui->comboBox_reducer_model->clear();
        ui->comboBox_reducer_model->addItem("经验公式");
        for(auto& component : currentAllComponentList)
        {
            // 尝试将component转换为Silencer类型的智能指针
            if(auto reducer = qSharedPointerDynamicCast<Reducer>(component))
            {
                QString model = reducer->model;
                if(ui->comboBox_reducer_model->findText(model) == -1)
                    ui->comboBox_reducer_model->addItem(model);
            }
        }
        ui->comboBox_reducer_model->setCurrentIndex(-1);

        ui->stackedWidget_table->setCurrentWidget(ui->page_reducer);
        ui->comboBox_reducer_model->blockSignals(false);
    }
    else if(arg1 == "消音器")
    {
        noi_after_cal_lineEdits = { ui->lineEdit_silencer_after_63, ui->lineEdit_silencer_after_125, ui->lineEdit_silencer_after_250,ui->lineEdit_silencer_after_500,
                                   ui->lineEdit_silencer_after_1k, ui->lineEdit_silencer_after_2k, ui->lineEdit_silencer_after_4k, ui->lineEdit_silencer_after_8k, ui->lineEdit_silencer_after_total};

        atten_lineEdits = { ui->lineEdit_silencer_63, ui->lineEdit_silencer_125, ui->lineEdit_silencer_250,ui->lineEdit_silencer_500,
                           ui->lineEdit_silencer_1k, ui->lineEdit_silencer_2k, ui->lineEdit_silencer_4k, ui->lineEdit_silencer_8k, ui->lineEdit_silencer_total};

        this->currentAllComponentList =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::SILENCER);


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
    this->currentTableType = arg1;
    ui->stackedWidget_title->setCurrentWidget(ui->page_unit);
    ui->comboBox_sound_type->setCurrentText(arg1);
    if(arg1 == "噪音源")
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
    QSharedPointer<ComponentBase> changedComponent = ComponentManager::getInstance().findComponent(false, uuid);
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
    noi_input_room_lineEdits.clear();
    room_noi_revise_lineEdits.clear();
    test_point_noi_lineEdits.clear();
    A_weighted_noi_lineEdit = nullptr;
}

void RoomCalTable::switchComboBoxState(bool canChoose, QComboBox* comboBox)
{
    if(canChoose)
    {
        // 显示下拉箭头
        comboBox->setStyleSheet("QComboBox{border:none; "
                                "border-bottom:1px solid black;}"
                                "QLineEdit{font-family: '黑体';"
                                "font-size: 14px;}"); // CSS隐藏下拉箭头
        comboBox->setMaxVisibleItems(10);
        comboBox->setEditable(false);
    }
    else
    {
        // 隐藏下拉箭头
        comboBox->setStyleSheet("QComboBox { combobox-popup: 0; border:none;"
                                "border-bottom:1px solid black;"
                                "font-family: '黑体';"
                                "font-size: 14px;}"
                                "QComboBox::drop-down { width:0px; }"); // CSS隐藏下拉箭头
        // 防止显示下拉列表
        comboBox->setEditable(true);
        comboBox->lineEdit()->setReadOnly(true);
    }
}

void RoomCalTable::on_comboBox_room_type_currentTextChanged(const QString &arg1)
{
    this->currentUnitType = arg1;
    variations[0] = INT_MIN;
    clearPage(ui->stackedWidget_table->currentWidget(), true);
    if(arg1 == "类型1")
    {
        noi_input_room_lineEdits = {ui->lineEdit_room_less425_input_63, ui->lineEdit_room_less425_input_125,
                                    ui->lineEdit_room_less425_input_250, ui->lineEdit_room_less425_input_500,
                                    ui->lineEdit_room_less425_input_1k, ui->lineEdit_room_less425_input_2k,
                                    ui->lineEdit_room_less425_input_4k, ui->lineEdit_room_less425_input_8k,
                                    ui->lineEdit_room_less425_input_total};
        room_noi_revise_lineEdits = {ui->lineEdit_room_less425_amend_63, ui->lineEdit_room_less425_amend_125,
                                     ui->lineEdit_room_less425_amend_250, ui->lineEdit_room_less425_amend_500,
                                     ui->lineEdit_room_less425_amend_1k, ui->lineEdit_room_less425_amend_2k,
                                     ui->lineEdit_room_less425_amend_4k, ui->lineEdit_room_less425_amend_8k,
                                     ui->lineEdit_room_less425_amend_total};
        test_point_noi_lineEdits = {ui->lineEdit_room_less425_test_press_63, ui->lineEdit_room_less425_test_press_125,
                                    ui->lineEdit_room_less425_test_press_250, ui->lineEdit_room_less425_test_press_500,
                                    ui->lineEdit_room_less425_test_press_1k, ui->lineEdit_room_less425_test_press_2k,
                                    ui->lineEdit_room_less425_test_press_4k, ui->lineEdit_room_less425_test_press_8k,
                                    ui->lineEdit_room_less425_test_press_total};
        A_weighted_noi_lineEdit = ui->lineEdit_room_less425_Anoise;
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_less425);
    }
    else if(arg1 == "类型2")
    {
        noi_input_room_lineEdits = {ui->lineEdit_room_more425_input_63, ui->lineEdit_room_more425_input_125,
                                    ui->lineEdit_room_more425_input_250, ui->lineEdit_room_more425_input_500,
                                    ui->lineEdit_room_more425_input_1k, ui->lineEdit_room_more425_input_2k,
                                    ui->lineEdit_room_more425_input_4k, ui->lineEdit_room_more425_input_8k,
                                    ui->lineEdit_room_more425_input_total};
        room_noi_revise_lineEdits = {ui->lineEdit_room_more425_amend_63, ui->lineEdit_room_more425_amend_125,
                                     ui->lineEdit_room_more425_amend_250, ui->lineEdit_room_more425_amend_500,
                                     ui->lineEdit_room_more425_amend_1k, ui->lineEdit_room_more425_amend_2k,
                                     ui->lineEdit_room_more425_amend_4k, ui->lineEdit_room_more425_amend_8k,
                                     ui->lineEdit_room_more425_amend_total};
        test_point_noi_lineEdits = {ui->lineEdit_room_more425_test_press_63, ui->lineEdit_room_more425_test_press_125,
                                    ui->lineEdit_room_more425_test_press_250, ui->lineEdit_room_more425_test_press_500,
                                    ui->lineEdit_room_more425_test_press_1k, ui->lineEdit_room_more425_test_press_2k,
                                    ui->lineEdit_room_more425_test_press_4k, ui->lineEdit_room_more425_test_press_8k,
                                    ui->lineEdit_room_more425_test_press_total};
        A_weighted_noi_lineEdit = ui->lineEdit_room_more425_Anoise;
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_more425);
    }
    else if(arg1 == "类型3")
    {
        noi_input_room_lineEdits = {ui->lineEdit_room_noFurniture_input_63, ui->lineEdit_room_noFurniture_input_125,
                                    ui->lineEdit_room_noFurniture_input_250, ui->lineEdit_room_noFurniture_input_500,
                                    ui->lineEdit_room_noFurniture_input_1k, ui->lineEdit_room_noFurniture_input_2k,
                                    ui->lineEdit_room_noFurniture_input_4k, ui->lineEdit_room_noFurniture_input_8k,
                                    ui->lineEdit_room_noFurniture_input_total};
        room_noi_revise_lineEdits = {ui->lineEdit_room_noFurniture_amend_63, ui->lineEdit_room_noFurniture_amend_125,
                                     ui->lineEdit_room_noFurniture_amend_250, ui->lineEdit_room_noFurniture_amend_500,
                                     ui->lineEdit_room_noFurniture_amend_1k, ui->lineEdit_room_noFurniture_amend_2k,
                                     ui->lineEdit_room_noFurniture_amend_4k, ui->lineEdit_room_noFurniture_amend_8k,
                                     ui->lineEdit_room_noFurniture_amend_total};
        test_point_noi_lineEdits = {ui->lineEdit_room_noFurniture_test_press_63, ui->lineEdit_room_noFurniture_test_press_125,
                                    ui->lineEdit_room_noFurniture_test_press_250, ui->lineEdit_room_noFurniture_test_press_500,
                                    ui->lineEdit_room_noFurniture_test_press_1k, ui->lineEdit_room_noFurniture_test_press_2k,
                                    ui->lineEdit_room_noFurniture_test_press_4k, ui->lineEdit_room_noFurniture_test_press_8k,
                                    ui->lineEdit_room_noFurniture_test_press_total};
        A_weighted_noi_lineEdit = ui->lineEdit_room_noFurniture_Anoise;
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_noFurniture);
    }
    else if(arg1 == "类型4")
    {
        noi_input_room_lineEdits = {ui->lineEdit_room_open_input_63, ui->lineEdit_room_open_input_125,
                                    ui->lineEdit_room_open_input_250, ui->lineEdit_room_open_input_500,
                                    ui->lineEdit_room_open_input_1k, ui->lineEdit_room_open_input_2k,
                                    ui->lineEdit_room_open_input_4k, ui->lineEdit_room_open_input_8k,
                                    ui->lineEdit_room_open_input_total};
        room_noi_revise_lineEdits = {ui->lineEdit_room_open_amend_63, ui->lineEdit_room_open_amend_125,
                                     ui->lineEdit_room_open_amend_250, ui->lineEdit_room_open_amend_500,
                                     ui->lineEdit_room_open_amend_1k, ui->lineEdit_room_open_amend_2k,
                                     ui->lineEdit_room_open_amend_4k, ui->lineEdit_room_open_amend_8k,
                                     ui->lineEdit_room_open_amend_total};
        test_point_noi_lineEdits = {ui->lineEdit_room_open_test_press_63, ui->lineEdit_room_open_test_press_125,
                                    ui->lineEdit_room_open_test_press_250, ui->lineEdit_room_open_test_press_500,
                                    ui->lineEdit_room_open_test_press_1k, ui->lineEdit_room_open_test_press_2k,
                                    ui->lineEdit_room_open_test_press_4k, ui->lineEdit_room_open_test_press_8k,
                                    ui->lineEdit_room_open_test_press_total};
        A_weighted_noi_lineEdit = ui->lineEdit_room_open_Anoise;
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_open);
    }
    else if(arg1 == "类型5")
    {
        noi_input_room_lineEdits = {ui->lineEdit_room_gap_tuyere_input_63, ui->lineEdit_room_gap_tuyere_input_125,
                                    ui->lineEdit_room_gap_tuyere_input_250, ui->lineEdit_room_gap_tuyere_input_500,
                                    ui->lineEdit_room_gap_tuyere_input_1k, ui->lineEdit_room_gap_tuyere_input_2k,
                                    ui->lineEdit_room_gap_tuyere_input_4k, ui->lineEdit_room_gap_tuyere_input_8k,
                                    ui->lineEdit_room_gap_tuyere_input_total};
        room_noi_revise_lineEdits = {ui->lineEdit_room_gap_tuyere_amend_63, ui->lineEdit_room_gap_tuyere_amend_125,
                                     ui->lineEdit_room_gap_tuyere_amend_250, ui->lineEdit_room_gap_tuyere_amend_500,
                                     ui->lineEdit_room_gap_tuyere_amend_1k, ui->lineEdit_room_gap_tuyere_amend_2k,
                                     ui->lineEdit_room_gap_tuyere_amend_4k, ui->lineEdit_room_gap_tuyere_amend_8k,
                                     ui->lineEdit_room_gap_tuyere_amend_total};
        test_point_noi_lineEdits = {ui->lineEdit_room_gap_tuyere_test_press_63, ui->lineEdit_room_gap_tuyere_test_press_125,
                                    ui->lineEdit_room_gap_tuyere_test_press_250, ui->lineEdit_room_gap_tuyere_test_press_500,
                                    ui->lineEdit_room_gap_tuyere_test_press_1k, ui->lineEdit_room_gap_tuyere_test_press_2k,
                                    ui->lineEdit_room_gap_tuyere_test_press_4k, ui->lineEdit_room_gap_tuyere_test_press_8k,
                                    ui->lineEdit_room_gap_tuyere_test_press_total};
        A_weighted_noi_lineEdit = ui->lineEdit_room_gap_tuyere_Anoise;
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_gap_tuyere);

    }
    else if(arg1 == "类型6")
    {
        noi_input_room_lineEdits = {ui->lineEdit_room_rain_input_63, ui->lineEdit_room_rain_input_125,
                                    ui->lineEdit_room_rain_input_250, ui->lineEdit_room_rain_input_500,
                                    ui->lineEdit_room_rain_input_1k, ui->lineEdit_room_rain_input_2k,
                                    ui->lineEdit_room_rain_input_4k, ui->lineEdit_room_rain_input_8k,
                                    ui->lineEdit_room_rain_input_total};
        room_noi_revise_lineEdits = {ui->lineEdit_room_rain_amend_63, ui->lineEdit_room_rain_amend_125,
                                     ui->lineEdit_room_rain_amend_250, ui->lineEdit_room_rain_amend_500,
                                     ui->lineEdit_room_rain_amend_1k, ui->lineEdit_room_rain_amend_2k,
                                     ui->lineEdit_room_rain_amend_4k, ui->lineEdit_room_rain_amend_8k,
                                     ui->lineEdit_room_rain_amend_total};
        test_point_noi_lineEdits = {ui->lineEdit_room_rain_test_press_63, ui->lineEdit_room_rain_test_press_125,
                                    ui->lineEdit_room_rain_test_press_250, ui->lineEdit_room_rain_test_press_500,
                                    ui->lineEdit_room_rain_test_press_1k, ui->lineEdit_room_rain_test_press_2k,
                                    ui->lineEdit_room_rain_test_press_4k, ui->lineEdit_room_rain_test_press_8k,
                                    ui->lineEdit_room_rain_test_press_total};
        A_weighted_noi_lineEdit = ui->lineEdit_room_rain_Anoise;
        ui->stackedWidget_table->setCurrentWidget(ui->page_room_rain);
    }
    for(auto i = 0; i < noi_input_room_lineEdits.length(); i++) {
        noi_input_room_lineEdits[i]->setText(noise_before_cal[i]);
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

void RoomCalTable::registerSetTableInfoFuncMap()
{
    auto setComboBoxValue = [](QComboBox* comboBox, const QString& value) {
        if (comboBox->findText(value) == -1) {
            qDebug() << "error: setComboBoxValue faile";
            return false;
        }
        comboBox->setCurrentText(value);
        return true;
    };

    auto getJsonValue = [](QJsonObject jsonObj, const QString& key, QString& value) {
        if(!jsonObj.contains(key)) {
            qDebug() << "error: jsonObj does not have this key";
            return false;
        }
        value = jsonObj[key].toString();
        return true;
    };

    setTableInfoFunc.insert(RoomCalTableType::FAN, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音源");
        ui->comboBox_unit_name->setCurrentText(roomCalTableTypeToString(RoomCalTableType::FAN));

        //获取json数据
        QString number;
        QString noise_locate;
        if(!getJsonValue(jsonObj, "number", number)) {
            return;
        }
        if(!getJsonValue(jsonObj, "noise_locate", noise_locate)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_fan_number, number)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_fan_noise_locate, noise_locate)) {
            return;
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::FANCOIL, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音源");
        ui->comboBox_unit_name->setCurrentText(roomCalTableTypeToString(RoomCalTableType::FANCOIL));

        //获取json数据
        QString number;
        QString model;
        QString noise_locate;
        if(!getJsonValue(jsonObj, "number", number)) {
            return;
        }
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "noise_locate", noise_locate)) {
            return;
        }

        ui->lineEdit_fanCoil_number->setText(number);
        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_fanCoil_model, model)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_fan_noise_locate, noise_locate)) {
            return;
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::AIRCONDITION_SINGLE, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音源");
        ui->comboBox_unit_name->setCurrentText(roomCalTableTypeToString(RoomCalTableType::AIRCONDITION_SINGLE));

        //获取json数据
        QString number;
        QString noise_locate;
        if(!getJsonValue(jsonObj, "number", number)) {
            return;
        }
        if(!getJsonValue(jsonObj, "noise_locate", noise_locate)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_aircondition_number, number)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_aircondition_noise_locate, noise_locate)) {
            return;
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::AIRCONDITION_DOUBLE, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音源");
        ui->comboBox_unit_name->setCurrentText(roomCalTableTypeToString(RoomCalTableType::AIRCONDITION_DOUBLE));

        //获取json数据
        QString number;
        QString noise_locate;
        if(!getJsonValue(jsonObj, "number", number)) {
            return;
        }
        if(!getJsonValue(jsonObj, "noise_locate", noise_locate)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_aircondition_number, number)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_aircondition_noise_locate, noise_locate)) {
            return;
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::VAV_TERMINAL, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("气流噪音");
        ui->comboBox_unit_name->setCurrentText("阀门/" + roomCalTableTypeToString(RoomCalTableType::VAV_TERMINAL));

        //获取json数据
        QString model;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_VAV_terminal_model, model)) {
            return;
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::CIRCULAR_DAMPER, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("气流噪音");
        ui->comboBox_unit_name->setCurrentText("阀门/" + roomCalTableTypeToString(RoomCalTableType::CIRCULAR_DAMPER));

        //获取json数据
        QString model;
        QString diameter;
        QString angle;
        QString air_volume;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "diameter", diameter)) {
            return;
        }
        if(!getJsonValue(jsonObj, "angle", angle)) {
            return;
        }
        if(!getJsonValue(jsonObj, "air_volume", air_volume)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_circular_damper_model, model)) {
            return;
        }
        if(model == "经验公式"){
            ui->lineEdit_circular_damper_diameter->setText(diameter);
            if(!setComboBoxValue(ui->comboBox_circular_damper_angle, angle)) {
                return;
            }
            ui->lineEdit_circular_damper_air_volume->setText(air_volume);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::RECT_DAMPER, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("气流噪音");
        ui->comboBox_unit_name->setCurrentText("阀门/" + roomCalTableTypeToString(RoomCalTableType::RECT_DAMPER));

        //获取json数据
        QString model;
        QString size;
        QString angle;
        QString air_volume;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "size", size)) {
            return;
        }
        if(!getJsonValue(jsonObj, "angle", angle)) {
            return;
        }
        if(!getJsonValue(jsonObj, "air_volume", air_volume)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_rect_damper_model, model)) {
            return;
        }
        if(model == "经验公式"){
            ui->lineEdit_rect_damper_size->setText(size);
            if(!setComboBoxValue(ui->comboBox_rect_damper_angle, angle)) {
                return;
            }
            ui->lineEdit_rect_damper_air_volume->setText(air_volume);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::AIRDIFF, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减+气流噪音");
        ui->comboBox_unit_name->setCurrentText("末端/" + roomCalTableTypeToString(RoomCalTableType::AIRDIFF));

        //获取json数据
        QString air_distributor_model;
        QString diffuser_model;
        QString terminal_type;
        QString size;
        if(!getJsonValue(jsonObj, "air_distributor_model", air_distributor_model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "diffuser_model", diffuser_model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "terminal_type", terminal_type)) {
            return;
        }
        if(!getJsonValue(jsonObj, "size", size)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_air_distributor_model, air_distributor_model)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_diffuser_model, diffuser_model)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_air_diff_terminal_type, terminal_type)) {
            return;
        }
        if(air_distributor_model == "经验公式"){
            ui->lineEdit_air_diff_size->setText(size);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::PUMP, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减+气流噪音");
        ui->comboBox_unit_name->setCurrentText("末端/" + roomCalTableTypeToString(RoomCalTableType::PUMP));

        //获取json数据
        QString model;
        QString terminal_type;
        QString size;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "terminal_type", terminal_type)) {
            return;
        }
        if(!getJsonValue(jsonObj, "size", size)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_pump_model, model)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_pump_terminal_type, terminal_type)) {
            return;
        }
        if(model == "经验公式"){
            ui->lineEdit_pump_size->setText(size);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::SEND, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减+气流噪音");
        ui->comboBox_unit_name->setCurrentText("末端/" + roomCalTableTypeToString(RoomCalTableType::SEND));

        //获取json数据
        QString model;
        QString terminal_type;
        QString size;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "terminal_type", terminal_type)) {
            return;
        }
        if(!getJsonValue(jsonObj, "size", size)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_send_model, model)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_send_terminal_type, terminal_type)) {
            return;
        }
        if(model == "经验公式"){
            ui->lineEdit_send_size->setText(size);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::STATICBOX_GRILLE, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减+气流噪音");
        ui->comboBox_unit_name->setCurrentText("末端/" + roomCalTableTypeToString(RoomCalTableType::STATICBOX_GRILLE));

        //获取json数据
        QString staticBox_model;
        QString grille_model;
        QString terminal_type;
        QString size;
        if(!getJsonValue(jsonObj, "staticBox_model", staticBox_model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "grille_model", grille_model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "terminal_type", terminal_type)) {
            return;
        }
        if(!getJsonValue(jsonObj, "size", size)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_staticBox_model, staticBox_model)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_grille_model, grille_model)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_staticBox_grille_terminal_type, terminal_type)) {
            return;
        }
        if(staticBox_model == "经验公式"){
            ui->lineEdit_staticBox_grille_size->setText(size);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::DISP_VENT_TERMINAL, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减+气流噪音");
        ui->comboBox_unit_name->setCurrentText("末端/" + roomCalTableTypeToString(RoomCalTableType::DISP_VENT_TERMINAL));

        //获取json数据
        QString model;
        QString terminal_type;
        QString size;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "terminal_type", terminal_type)) {
            return;
        }
        if(!getJsonValue(jsonObj, "size", size)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_disp_vent_terminal_model, model)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_disp_vent_terminal_type, terminal_type)) {
            return;
        }
        if(model == "经验公式"){
            ui->lineEdit_disp_vent_terminal_size->setText(size);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::OTHER_SEND_TERMINAL, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减+气流噪音");
        ui->comboBox_unit_name->setCurrentText("末端/" + roomCalTableTypeToString(RoomCalTableType::OTHER_SEND_TERMINAL));

        //获取json数据
        QString model;
        QString terminal_type;
        QString size;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "terminal_type", terminal_type)) {
            return;
        }
        if(!getJsonValue(jsonObj, "size", size)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_other_send_terminal_model, model)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_other_send_terminal_type, terminal_type)) {
            return;
        }
        if(model == "经验公式"){
            ui->lineEdit_other_send_terminal_size->setText(size);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::STATICBOX, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减");
        ui->comboBox_unit_name->setCurrentText("分支/" + roomCalTableTypeToString(RoomCalTableType::STATICBOX));

        //获取json数据
        QString model;
        QString total_air_volume;
        QString branch_air_volume;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "total_air_volume", total_air_volume)) {
            return;
        }
        if(!getJsonValue(jsonObj, "branch_air_volume", branch_air_volume)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_static_box_model, model)) {
            return;
        }
        if(model == "经验公式"){
            ui->lineEdit_static_box_total_air_volume->setText(total_air_volume);
            ui->lineEdit_static_box_branch_air_volume->setText(branch_air_volume);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::MULTI_RANC, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减");
        ui->comboBox_unit_name->setCurrentText("分支/" + roomCalTableTypeToString(RoomCalTableType::MULTI_RANC));

        //获取json数据
        QString model;
        QString total_air_volume;
        QString branch_air_volume;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "total_air_volume", total_air_volume)) {
            return;
        }
        if(!getJsonValue(jsonObj, "branch_air_volume", branch_air_volume)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_multi_ranc_model, model)) {
            return;
        }
        if(model == "经验公式"){
            ui->lineEdit_multi_ranc_total_air_volume->setText(total_air_volume);
            ui->lineEdit_multi_ranc_branch_air_volume->setText(branch_air_volume);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::TEE, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减");
        ui->comboBox_unit_name->setCurrentText("分支/" + roomCalTableTypeToString(RoomCalTableType::TEE));

        //获取json数据
        QString model;
        QString total_air_volume;
        QString branch_air_volume;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "total_air_volume", total_air_volume)) {
            return;
        }
        if(!getJsonValue(jsonObj, "branch_air_volume", branch_air_volume)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_tee_model, model)) {
            return;
        }
        if(model == "经验公式"){
            ui->lineEdit_tee_total_air_volume->setText(total_air_volume);
            ui->lineEdit_tee_branch_air_volume->setText(branch_air_volume);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::PIPE, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减");
        ui->comboBox_unit_name->setCurrentText(roomCalTableTypeToString(RoomCalTableType::PIPE));

        //获取json数据
        QString model;
        QString type;
        QString size;
        QString length;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "type", type)) {
            return;
        }
        if(!getJsonValue(jsonObj, "size", size)) {
            return;
        }
        if(!getJsonValue(jsonObj, "length", length)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_pipe_model, model)) {
            return;
        }
        if(model == "经验公式"){
            if(!setComboBoxValue(ui->comboBox_pipe_type, type)) {
                return;
            }
            ui->lineEdit_pipe_size->setText(size);
        }
        ui->lineEdit_pipe_length->setText(length);
    });

    setTableInfoFunc.insert(RoomCalTableType::ELBOW, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减");
        ui->comboBox_unit_name->setCurrentText(roomCalTableTypeToString(RoomCalTableType::ELBOW));

        //获取json数据
        QString model;
        QString type;
        QString size;
        QString count;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "type", type)) {
            return;
        }
        if(!getJsonValue(jsonObj, "size", size)) {
            return;
        }
        if(!getJsonValue(jsonObj, "count", count)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_elbow_model, model)) {
            return;
        }
        if(model == "经验公式"){
            if(!setComboBoxValue(ui->comboBox_elbow_type, type)) {
                return;
            }
            ui->lineEdit_elbow_size->setText(size);
        }
        ui->lineEdit_elbow_count->setText(count);
    });

    setTableInfoFunc.insert(RoomCalTableType::REDUCER, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减");
        ui->comboBox_unit_name->setCurrentText(roomCalTableTypeToString(RoomCalTableType::REDUCER));

        //获取json数据
        QString model;
        QString type;
        QString before_size;
        QString after_size;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "type", type)) {
            return;
        }
        if(!getJsonValue(jsonObj, "before_size", before_size)) {
            return;
        }
        if(!getJsonValue(jsonObj, "after_size", after_size)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_reducer_model, model)) {
            return;
        }
        if(model == "经验公式"){
            if(!setComboBoxValue(ui->comboBox_reducer_type, type)) {
                return;
            }
            ui->lineEdit_reducer_before_size->setText(before_size);
            ui->lineEdit_reducer_after_size->setText(after_size);
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::SILENCER, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(0);
        ui->comboBox_sound_type->setCurrentText("噪音衰减");
        ui->comboBox_unit_name->setCurrentText(roomCalTableTypeToString(RoomCalTableType::SILENCER));

        //获取json数据
        QString model;
        QString type;
        if(!getJsonValue(jsonObj, "model", model)) {
            return;
        }
        if(!getJsonValue(jsonObj, "type", type)) {
            return;
        }

        //设置下拉框内容
        if(!setComboBoxValue(ui->comboBox_silencer_type, type)) {
            return;
        }
        if(!setComboBoxValue(ui->comboBox_silencer_model, model)) {
            return;
        }
    });

    setTableInfoFunc.insert(RoomCalTableType::ROOM_LESS425, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(1);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
        ui->comboBox_room_type->setCurrentText(roomCalTableTypeToString(RoomCalTableType::ROOM_LESS425));

        //获取json数据
        QString test_distance;
        QString volume;
        if(!getJsonValue(jsonObj, "test_distance", test_distance)) {
            return;
        }
        if(!getJsonValue(jsonObj, "volume", volume)) {
            return;
        }

        ui->lineEdit_room_less425_test_distance->setText(test_distance);
        ui->lineEdit_room_less425_volume->setText(volume);
    });

    setTableInfoFunc.insert(RoomCalTableType::ROOM_MORE425, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(1);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
        ui->comboBox_room_type->setCurrentText(roomCalTableTypeToString(RoomCalTableType::ROOM_MORE425));

        //获取json数据
        QString test_distance;
        if(!getJsonValue(jsonObj, "test_distance", test_distance)) {
            return;
        }

        ui->lineEdit_room_more425_test_distance->setText(test_distance);
    });

    setTableInfoFunc.insert(RoomCalTableType::ROOM_NOFURNITURE, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(1);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
        ui->comboBox_room_type->setCurrentText(roomCalTableTypeToString(RoomCalTableType::ROOM_NOFURNITURE));

        //获取json数据
        QString test_distance;
        QString angle;
        QString absorption_rate;
        QString area;
        QString height;
        if(!getJsonValue(jsonObj, "test_distance", test_distance)) {
            return;
        }
        if(!getJsonValue(jsonObj, "angle", angle)) {
            return;
        }
        if(!getJsonValue(jsonObj, "absorption_rate", absorption_rate)) {
            return;
        }
        if(!getJsonValue(jsonObj, "area", area)) {
            return;
        }
        if(!getJsonValue(jsonObj, "height", height)) {
            return;
        }

        ui->lineEdit_room_noFurniture_test_distance->setText(test_distance);
        ui->lineEdit_room_noFurniture_angle->setText(angle);
        ui->lineEdit_room_noFurniture_absorption_rate->setText(absorption_rate);
        ui->lineEdit_room_noFurniture_area->setText(area);
        ui->lineEdit_room_noFurniture_height->setText(height);
    });

    setTableInfoFunc.insert(RoomCalTableType::ROOM_OPEN, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(1);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
        ui->comboBox_room_type->setCurrentText(roomCalTableTypeToString(RoomCalTableType::ROOM_OPEN));

        //获取json数据
        QString test_distance;
        QString angle;
        if(!getJsonValue(jsonObj, "test_distance", test_distance)) {
            return;
        }
        if(!getJsonValue(jsonObj, "angle", angle)) {
            return;
        }

        ui->lineEdit_room_open_test_distance->setText(test_distance);
        ui->lineEdit_room_open_angle->setText(angle);
    });

    setTableInfoFunc.insert(RoomCalTableType::ROOM_GAP_TUYERE, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(1);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
        ui->comboBox_room_type->setCurrentText(roomCalTableTypeToString(RoomCalTableType::ROOM_GAP_TUYERE));

        //获取json数据
        QString test_distance;
        QString angle;
        QString absorption_rate;
        QString area;
        QString height;
        QString gap_length;
        if(!getJsonValue(jsonObj, "test_distance", test_distance)) {
            return;
        }
        if(!getJsonValue(jsonObj, "angle", angle)) {
            return;
        }
        if(!getJsonValue(jsonObj, "absorption_rate", absorption_rate)) {
            return;
        }
        if(!getJsonValue(jsonObj, "area", area)) {
            return;
        }
        if(!getJsonValue(jsonObj, "height", height)) {
            return;
        }
        if(!getJsonValue(jsonObj, "gap_length", gap_length)) {
            return;
        }

        ui->lineEdit_room_gap_tuyere_test_distance->setText(test_distance);
        ui->lineEdit_room_gap_tuyere_angle->setText(angle);
        ui->lineEdit_room_gap_tuyere_absorption_rete->setText(absorption_rate);
        ui->lineEdit_room_gap_tuyere_area->setText(area);
        ui->lineEdit_room_gap_tuyere_height->setText(height);
        ui->lineEdit_room_gap_tuyere_gap_length->setText(gap_length);
    });

    setTableInfoFunc.insert(RoomCalTableType::ROOM_RAIN, [=, this](QJsonObject jsonObj) {
        ui->stackedWidget_title->setCurrentIndex(1);
        ui->comboBox_sound_type_room->setCurrentText("声压级计算");
        ui->comboBox_room_type->setCurrentText(roomCalTableTypeToString(RoomCalTableType::ROOM_RAIN));

        //获取json数据
        QString test_distance;
        QString area;
        QString height;
        if(!getJsonValue(jsonObj, "test_distance", test_distance)) {
            return;
        }
        if(!getJsonValue(jsonObj, "area", area)) {
            return;
        }
        if(!getJsonValue(jsonObj, "height", height)) {
            return;
        }

        ui->lineEdit_room_rain_test_distance->setText(test_distance);
        ui->lineEdit_room_rain_area->setText(area);
        ui->lineEdit_room_rain_height->setText(height);
    });

    setTableInfoFunc.insert(RoomCalTableType::UNDEFINED, [=, this](QJsonObject jsonObj) {
        qDebug() << "type is undefined (type err)";
        return;
    });
}

void RoomCalTable::registerCreateTableInfoJsonFuncMap()
{
    createTableInfoJsonFunc.insert(RoomCalTableType::FAN, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::FAN) || !isTableCompleted()) {
            return;
        }

        jsonObj.insert("number", ui->comboBox_fan_number->currentText());
        jsonObj.insert("noise_locate", ui->comboBox_fan_noise_locate->currentText());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::FANCOIL, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::FANCOIL) || !isTableCompleted()) {
            return;
        }

        jsonObj.insert("number", ui->lineEdit_fanCoil_number->text());
        jsonObj.insert("model", ui->comboBox_fanCoil_model->currentText());
        jsonObj.insert("noise_locate", ui->comboBox_fanCoil_noise_locate->currentText());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::AIRCONDITION_SINGLE, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::AIRCONDITION_SINGLE) || !isTableCompleted()) {
            return;
        }

        jsonObj.insert("number", ui->comboBox_aircondition_number->currentText());
        jsonObj.insert("noise_locate", ui->comboBox_aircondition_noise_locate->currentText());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::AIRCONDITION_DOUBLE, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::AIRCONDITION_DOUBLE) || !isTableCompleted()) {
            return;
        }

        jsonObj.insert("number", ui->comboBox_aircondition_number->currentText());
        jsonObj.insert("noise_locate", ui->comboBox_aircondition_noise_locate->currentText());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::VAV_TERMINAL, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::VAV_TERMINAL) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_VAV_terminal_model->currentText());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::CIRCULAR_DAMPER, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::CIRCULAR_DAMPER) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_circular_damper_model->currentText());
        jsonObj.insert("diameter", ui->lineEdit_circular_damper_diameter->text());
        jsonObj.insert("angle", ui->comboBox_circular_damper_angle->currentText());
        jsonObj.insert("air_volume", ui->lineEdit_circular_damper_air_volume->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::RECT_DAMPER, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::RECT_DAMPER) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_rect_damper_model->currentText());
        jsonObj.insert("size", ui->lineEdit_rect_damper_size->text());
        jsonObj.insert("angle", ui->comboBox_rect_damper_angle->currentText());
        jsonObj.insert("air_volume", ui->lineEdit_rect_damper_air_volume->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::AIRDIFF, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::AIRDIFF) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("air_distributor_model", ui->comboBox_air_distributor_model->currentText());
        jsonObj.insert("diffuser_model", ui->comboBox_diffuser_model->currentText());
        jsonObj.insert("terminal_type", ui->comboBox_air_diff_terminal_type->currentText());
        jsonObj.insert("size", ui->lineEdit_air_diff_size->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::PUMP, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::PUMP) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_pump_model->currentText());
        jsonObj.insert("terminal_type", ui->comboBox_pump_terminal_type->currentText());
        jsonObj.insert("size", ui->lineEdit_pump_size->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::SEND, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::SEND) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_send_model->currentText());
        jsonObj.insert("terminal_type", ui->comboBox_send_terminal_type->currentText());
        jsonObj.insert("size", ui->lineEdit_send_size->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::STATICBOX_GRILLE, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::STATICBOX_GRILLE) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("staticBox_model", ui->comboBox_staticBox_model->currentText());
        jsonObj.insert("grille_model", ui->comboBox_grille_model->currentText());
        jsonObj.insert("terminal_type", ui->comboBox_staticBox_grille_terminal_type->currentText());
        jsonObj.insert("size", ui->lineEdit_staticBox_grille_size->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::DISP_VENT_TERMINAL, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::DISP_VENT_TERMINAL) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_disp_vent_terminal_model->currentText());
        jsonObj.insert("terminal_type", ui->comboBox_disp_vent_terminal_type->currentText());
        jsonObj.insert("size", ui->lineEdit_disp_vent_terminal_size->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::OTHER_SEND_TERMINAL, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::OTHER_SEND_TERMINAL) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_other_send_terminal_model->currentText());
        jsonObj.insert("terminal_type", ui->comboBox_other_send_terminal_type->currentText());
        jsonObj.insert("size", ui->lineEdit_other_send_terminal_size->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::STATICBOX, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::STATICBOX) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_static_box_model->currentText());
        jsonObj.insert("total_air_volume", ui->lineEdit_static_box_total_air_volume->text());
        jsonObj.insert("branch_air_volume", ui->lineEdit_static_box_branch_air_volume->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::MULTI_RANC, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::MULTI_RANC) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_multi_ranc_model->currentText());
        jsonObj.insert("total_air_volume", ui->lineEdit_multi_ranc_total_air_volume->text());
        jsonObj.insert("branch_air_volume", ui->lineEdit_multi_ranc_branch_air_volume->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::TEE, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::TEE) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_tee_model->currentText());
        jsonObj.insert("total_air_volume", ui->lineEdit_tee_total_air_volume->text());
        jsonObj.insert("branch_air_volume", ui->lineEdit_tee_branch_air_volume->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::PIPE, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::PIPE) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_pipe_model->currentText());
        jsonObj.insert("type", ui->comboBox_pipe_type->currentText());
        jsonObj.insert("size", ui->lineEdit_pipe_size->text());
        jsonObj.insert("length", ui->lineEdit_pipe_length->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::ELBOW, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::ELBOW) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_elbow_model->currentText());
        jsonObj.insert("type", ui->comboBox_elbow_type->currentText());
        jsonObj.insert("size", ui->lineEdit_elbow_size->text());
        jsonObj.insert("count", ui->lineEdit_elbow_count->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::REDUCER, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::REDUCER) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_reducer_model->currentText());
        jsonObj.insert("type", ui->comboBox_reducer_type->currentText());
        jsonObj.insert("before_size", ui->lineEdit_reducer_before_size->text());
        jsonObj.insert("after_size", ui->lineEdit_reducer_after_size->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::SILENCER, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::SILENCER) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("model", ui->comboBox_silencer_model->currentText());
        jsonObj.insert("type", ui->comboBox_silencer_type->currentText());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::ROOM_LESS425, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::ROOM_LESS425) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("test_distance", ui->lineEdit_room_less425_test_distance->text());
        jsonObj.insert("volume", ui->lineEdit_room_less425_volume->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::ROOM_MORE425, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::ROOM_MORE425) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("test_distance", ui->lineEdit_room_more425_test_distance->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::ROOM_NOFURNITURE, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::ROOM_NOFURNITURE) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("test_distance", ui->lineEdit_room_more425_test_distance->text());
        jsonObj.insert("angle", ui->lineEdit_room_noFurniture_angle->text());
        jsonObj.insert("absorption_rate", ui->lineEdit_room_noFurniture_absorption_rate->text());
        jsonObj.insert("area", ui->lineEdit_room_noFurniture_area->text());
        jsonObj.insert("height", ui->lineEdit_room_noFurniture_height->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::ROOM_OPEN, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::ROOM_OPEN) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("test_distance", ui->lineEdit_room_open_test_distance->text());
        jsonObj.insert("angle", ui->lineEdit_room_open_angle->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::ROOM_GAP_TUYERE, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::ROOM_GAP_TUYERE) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("test_distance", ui->lineEdit_room_gap_tuyere_test_distance->text());
        jsonObj.insert("angle", ui->lineEdit_room_gap_tuyere_angle->text());
        jsonObj.insert("absorption_rate", ui->lineEdit_room_gap_tuyere_absorption_rete->text());
        jsonObj.insert("area", ui->lineEdit_room_gap_tuyere_area->text());
        jsonObj.insert("height", ui->lineEdit_room_gap_tuyere_height->text());
        jsonObj.insert("gap_length", ui->lineEdit_room_gap_tuyere_gap_length->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::ROOM_RAIN, [=, this](QJsonObject& jsonObj) {
        if (currentUnitType != roomCalTableTypeToString(RoomCalTableType::ROOM_RAIN) || !isTableCompleted()) {
            return;
        }
        jsonObj.insert("test_distance", ui->lineEdit_room_rain_test_distance->text());
        jsonObj.insert("area", ui->lineEdit_room_rain_area->text());
        jsonObj.insert("heigh", ui->lineEdit_room_rain_height->text());
    });

    createTableInfoJsonFunc.insert(RoomCalTableType::UNDEFINED, [=, this](QJsonObject& jsonObj) {
        qDebug() << "type is undefined (type err)";
        return;
    });
}

void RoomCalTable::initRoomCalSlotFuncConn()
{
    connect(ui->lineEdit_room_less425_test_distance, &QLineEdit::textChanged, this, &RoomCalTable::roomLess425Cal);
    connect(ui->lineEdit_room_less425_volume, &QLineEdit::textChanged, this, &RoomCalTable::roomLess425Cal);

    connect(ui->lineEdit_room_more425_test_distance, &QLineEdit::textChanged, this, &RoomCalTable::roomMore425Cal);

    connect(ui->lineEdit_room_noFurniture_test_distance, &QLineEdit::textChanged, this, &RoomCalTable::roomNoFurnitureCal);
    connect(ui->lineEdit_room_noFurniture_angle, &QLineEdit::textChanged, this, &RoomCalTable::roomNoFurnitureCal);
    connect(ui->lineEdit_room_noFurniture_absorption_rate, &QLineEdit::textChanged, this, &RoomCalTable::roomNoFurnitureCal);
    connect(ui->lineEdit_room_noFurniture_area, &QLineEdit::textChanged, this, &RoomCalTable::roomNoFurnitureCal);

    connect(ui->lineEdit_room_open_test_distance, &QLineEdit::textChanged, this, &RoomCalTable::roomOpenCal);
    connect(ui->lineEdit_room_open_angle, &QLineEdit::textChanged, this, &RoomCalTable::roomOpenCal);

    connect(ui->lineEdit_room_gap_tuyere_test_distance, &QLineEdit::textChanged, this, &RoomCalTable::roomGapTuyereCal);
    connect(ui->lineEdit_room_gap_tuyere_angle, &QLineEdit::textChanged, this, &RoomCalTable::roomGapTuyereCal);
    connect(ui->lineEdit_room_gap_tuyere_absorption_rete, &QLineEdit::textChanged, this, &RoomCalTable::roomGapTuyereCal);
    connect(ui->lineEdit_room_gap_tuyere_area, &QLineEdit::textChanged, this, &RoomCalTable::roomGapTuyereCal);
    connect(ui->lineEdit_room_gap_tuyere_gap_length, &QLineEdit::textChanged, this, &RoomCalTable::roomGapTuyereCal);

    connect(ui->lineEdit_room_rain_test_distance, &QLineEdit::textChanged, this, &RoomCalTable::roomRainCal);
    connect(ui->lineEdit_room_rain_area, &QLineEdit::textChanged, this, &RoomCalTable::roomRainCal);
}

void RoomCalTable::callSetTableInfoFunc(const RoomCalTableType &type, QJsonObject jsonObj)
{
    setTableInfoFunc[type](jsonObj);
}

void RoomCalTable::createTableInfoJsonObj(RoomCalTableType &type, QJsonObject &jsonObj)
{
    type = stringToRoomCalTableType(this->currentUnitType);
    createTableInfoJsonFunc[type](jsonObj);
}

bool RoomCalTable::isTableCompleted()
{
    bool res = true;
    if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::FAN)) {
        if(ui->comboBox_fan_number->currentIndex() == -1 || ui->comboBox_fan_noise_locate->currentIndex() == -1) {
            qDebug() << "fan table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::FANCOIL)) {
        if(ui->comboBox_fanCoil_model->currentIndex() == -1 || ui->comboBox_fanCoil_noise_locate->currentIndex() == -1 ||
            ui->lineEdit_fanCoil_number->text().isEmpty()) {
            qDebug() << "fanCoil table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::AIRCONDITION_SINGLE)) {
        if(ui->comboBox_aircondition_number->currentIndex() == -1 ||
            ui->comboBox_aircondition_noise_locate->currentIndex() == -1) {
            qDebug() << "aircondition table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::AIRCONDITION_DOUBLE)) {
        if(ui->comboBox_aircondition_number->currentIndex() == -1 ||
            ui->comboBox_aircondition_noise_locate->currentIndex() == -1) {
            qDebug() << "aircondition table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::VAV_TERMINAL)) {
        if(ui->comboBox_VAV_terminal_model->currentIndex() == -1) {
            qDebug() << "VAV_terminal table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::CIRCULAR_DAMPER)) {
        if(ui->comboBox_circular_damper_model->currentIndex() == -1 ||
            ui->lineEdit_circular_damper_diameter->text().isEmpty() ||
            ui->comboBox_circular_damper_angle->currentIndex() == -1 ||
            ui->lineEdit_circular_damper_air_volume->text().isEmpty()) {
            qDebug() << "circular_damper table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::RECT_DAMPER)) {
        if(ui->comboBox_rect_damper_model->currentIndex() == -1 ||
            ui->lineEdit_rect_damper_size->text().isEmpty() ||
            ui->comboBox_rect_damper_angle->currentIndex() == -1 ||
            ui->lineEdit_rect_damper_air_volume->text().isEmpty()) {
            qDebug() << "rect_damper table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::AIRDIFF)) {
        if(ui->comboBox_air_distributor_model->currentIndex() == -1 || ui->comboBox_diffuser_model->currentIndex() == -1 ||
            ui->comboBox_air_diff_terminal_type->currentIndex() == -1 || ui->lineEdit_air_diff_size->text().isEmpty()) {
            qDebug() << "air_diff table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::PUMP)) {
        if(ui->comboBox_pump_model->currentIndex() == -1 ||
            ui->comboBox_pump_terminal_type->currentIndex() == -1 || ui->lineEdit_pump_size->text().isEmpty()) {
            qDebug() << "pump table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::SEND)) {
        if(ui->comboBox_send_model->currentIndex() == -1 ||
            ui->comboBox_send_terminal_type->currentIndex() == -1 || ui->lineEdit_send_size->text().isEmpty()) {
            qDebug() << "send table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::STATICBOX_GRILLE)) {
        if(ui->comboBox_staticBox_model->currentIndex() == -1 || ui->comboBox_grille_model->currentIndex() == -1 ||
            ui->comboBox_staticBox_grille_terminal_type->currentIndex() == -1 ||
            ui->lineEdit_staticBox_grille_size->text().isEmpty()) {
            qDebug() << "staticBox_grille table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::DISP_VENT_TERMINAL)) {
        if(ui->comboBox_disp_vent_terminal_model->currentIndex() == -1 ||
            ui->comboBox_disp_vent_terminal_type->currentIndex() == -1 ||
            ui->lineEdit_disp_vent_terminal_size->text().isEmpty()) {
            qDebug() << "disp_vent_terminal table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::OTHER_SEND_TERMINAL)) {
        if(ui->comboBox_other_send_terminal_model->currentIndex() == -1 ||
            ui->comboBox_other_send_terminal_type->currentIndex() == -1 ||
            ui->lineEdit_other_send_terminal_size->text().isEmpty()) {
            qDebug() << "other_send_terminal table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::STATICBOX)) {
        if(ui->comboBox_static_box_model->currentIndex() == -1 ||
            ui->lineEdit_static_box_total_air_volume->text().isEmpty() ||
            ui->lineEdit_static_box_branch_air_volume->text().isEmpty()) {
            qDebug() << "static_box table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::MULTI_RANC)) {
        if(ui->comboBox_multi_ranc_model->currentIndex() == -1 ||
            ui->lineEdit_multi_ranc_total_air_volume->text().isEmpty() ||
            ui->lineEdit_multi_ranc_branch_air_volume->text().isEmpty()) {
            qDebug() << "multi_ranc table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::TEE)) {
        if(ui->comboBox_tee_model->currentIndex() == -1 ||
            ui->lineEdit_tee_total_air_volume->text().isEmpty() ||
            ui->lineEdit_tee_branch_air_volume->text().isEmpty()) {
            qDebug() << "tee table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::PIPE)) {
        if(ui->comboBox_pipe_model->currentIndex() == -1 || ui->comboBox_pipe_type->currentIndex() == -1 ||
            ui->lineEdit_pipe_size->text().isEmpty() || ui->lineEdit_pipe_length->text().isEmpty()) {
            qDebug() << "pipe table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::ELBOW)) {
        if(ui->comboBox_elbow_model->currentIndex() == -1 || ui->comboBox_elbow_type->currentIndex() == -1 ||
            ui->lineEdit_elbow_size->text().isEmpty() || ui->lineEdit_elbow_count->text().isEmpty()) {
            qDebug() << "elbow table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::REDUCER)) {
        if(ui->comboBox_reducer_model->currentIndex() == -1 || ui->comboBox_reducer_type->currentIndex() == -1 ||
            ui->lineEdit_reducer_before_size->text().isEmpty() || ui->lineEdit_reducer_after_size->text().isEmpty()) {
            qDebug() << "reducer table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::SILENCER)) {
        if(ui->comboBox_silencer_model->currentIndex() == -1 || ui->comboBox_silencer_type->currentIndex() == -1) {
            qDebug() << "silencer table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::ROOM_LESS425)) {
        if(ui->lineEdit_room_less425_test_distance->text().isEmpty() || ui->lineEdit_room_less425_volume->text().isEmpty()) {
            qDebug() << "room_less425 table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::ROOM_MORE425)) {
        if(ui->lineEdit_room_more425_test_distance->text().isEmpty()) {
            qDebug() << "room_more425 table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::ROOM_NOFURNITURE)) {
        if(ui->lineEdit_room_noFurniture_test_distance->text().isEmpty() ||
            ui->lineEdit_room_noFurniture_angle->text().isEmpty() ||
            ui->lineEdit_room_noFurniture_absorption_rate->text().isEmpty() ||
            ui->lineEdit_room_noFurniture_area->text().isEmpty() ||
            ui->lineEdit_room_noFurniture_height->text().isEmpty()) {
            qDebug() << "room_noFurniture table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::ROOM_OPEN)) {
        if(ui->lineEdit_room_open_test_distance->text().isEmpty() ||
            ui->lineEdit_room_open_angle->text().isEmpty()) {
            qDebug() << "room_open table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::ROOM_GAP_TUYERE)) {
        if(ui->lineEdit_room_gap_tuyere_test_distance->text().isEmpty() ||
            ui->lineEdit_room_gap_tuyere_angle->text().isEmpty() ||
            ui->lineEdit_room_gap_tuyere_absorption_rete->text().isEmpty() ||
            ui->lineEdit_room_gap_tuyere_area->text().isEmpty() ||
            ui->lineEdit_room_gap_tuyere_height->text().isEmpty() ||
            ui->lineEdit_room_gap_tuyere_gap_length->text().isEmpty()) {
            qDebug() << "room_gap_tuyere table information is incomplete";
            res = false;
        }
    } else if(this->currentUnitType == roomCalTableTypeToString(RoomCalTableType::ROOM_RAIN)) {
        if(ui->lineEdit_room_rain_test_distance->text().isEmpty() ||
            ui->lineEdit_room_rain_area->text().isEmpty() ||
            ui->lineEdit_room_rain_height->text().isEmpty()) {
            qDebug() << "room_rain table information is incomplete";
            res = false;
        }
    }
    return res;
}

bool RoomCalTable::isRoomCalTable()
{
    if(ui->stackedWidget_title->currentIndex() == 1) {
        return true;
    }
    return false;
}

bool RoomCalTable::isNoiseTable()
{
    if(ui->comboBox_sound_type->currentText() == "噪音源") {
        return true;
    }
    return false;
}

void RoomCalTable::calVariations()
{
    bool isSuperimposed = false;
    bool isTerminal = false;
    bool isRoomCal = false;

    if(!noi_lineEdits.empty())
    {
        isSuperimposed = true;
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
        isSuperimposed = false;
        for(int i = 0; i < 8; i++)
        {
            if(!(atten_lineEdits[i] && atten_lineEdits[i]->text() != ""))
            {
                variations = QVector<QString>(8,QString());
                return;
            }
            variations[i] = QString::number(-atten_lineEdits[i]->text().toDouble(),'f',2);
        }
    }
    else if(!terminal_atten_lineEdits.empty() && !terminal_refl_lineEdits.empty() && !terminal_noi_lineEdits.empty())
    {
        isTerminal = true;
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
            double refl = terminal_refl_lineEdits[i]->text().toDouble();
            double noi = terminal_noi_lineEdits[i]->text().toDouble();
            double last_noi = noise_before_cal[i].toDouble();

            double after_atten_noi = last_noi - atten - refl;

            double sup_noi = 10 * log10(pow(10, noi / 10) + pow(10, after_atten_noi / 10));

            variations[i] = QString::number(sup_noi ,'f', 2);
        }
    }
    else if(!each_atten_lineEdits.empty() && !sum_atten_lineEdits.empty())
    {
        isSuperimposed = false;
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
                sum_atten_lineEdits[i]->setText(QString::number(each_atten * ui->lineEdit_pipe_length->text().toInt(),'f',2));
                variations[i] = QString::number(-each_atten * ui->lineEdit_pipe_length->text().toInt(),'f',2);
            }
            else if(ui->lineEdit_elbow_count && ui->lineEdit_elbow_count->text() != "")
            {
                double each_atten = -each_atten_lineEdits[i]->text().toDouble();
                sum_atten_lineEdits[i]->setText(QString::number(each_atten * ui->lineEdit_elbow_count->text().toInt(),'f',2));
                variations[i] = QString::number(-each_atten * ui->lineEdit_elbow_count->text().toInt(),'f',2);
            }
            else
            {
                return;
            }
        }
    }
    else if(!noi_input_room_lineEdits.empty() && !room_noi_revise_lineEdits.empty() && !test_point_noi_lineEdits.empty()
             && A_weighted_noi_lineEdit) {
        isRoomCal = true;
        for(auto i = 0; i < noi_input_room_lineEdits.length(); i++) {
            noi_input_room_lineEdits[i]->setText(noise_before_cal[i]);
        }
    }

    if(variations[0] == INT_MIN) {
        return;
    }

    std::array<QLineEdit*, 9> lineEdits{0};
    for(int i = 0; i < 8; i++)
    {
        if(variations[i].isEmpty())
        {
            return;
        }

        //如果是末端就直接用计算好的数据
        if(isTerminal) {
            noi_after_cal_lineEdits[i]->setText(QString::number(variations[i].toDouble()));
            lineEdits[i] = noi_after_cal_lineEdits[i];
        } else if(isRoomCal) {
            test_point_noi_lineEdits[i]->setText(QString::number(variations[i].toDouble() + noi_input_room_lineEdits[i]->text().toDouble()));
            lineEdits[i] = test_point_noi_lineEdits[i];
        }
        else {
            //如果是噪音叠加就用log公式
            if(isSuperimposed) {
                noi_after_cal_lineEdits[i]->setText(QString::number(noiseSuperposition(variations[i].toDouble(), noise_before_cal[i].toDouble()),'f',2));
            } else {
                noi_after_cal_lineEdits[i]->setText(QString::number(variations[i].toDouble() + noise_before_cal[i].toDouble(),'f',2));
            }
            lineEdits[i] = noi_after_cal_lineEdits[i];
        }

    }
    if(!isRoomCal)
        noi_after_cal_lineEdits[8]->setText(QString::number(calNoiseTotalValue(lineEdits),'f' ,1)); //总值
    else {
        test_point_noi_lineEdits[8]->setText(QString::number(calNoiseTotalValue(lineEdits),'f' ,1)); //总值
        A_weighted_noi_lineEdit->setText(test_point_noi_lineEdits[8]->text());
    }
}

/**********风机**********/
#pragma region "fan"{
//风机编号选择
void RoomCalTable::on_comboBox_fan_number_currentTextChanged(const QString &arg1)
{
    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto fan = qSharedPointerDynamicCast<Fan>(component);
        if(fan && fan->number == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
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
//风机盘管型号选择
void RoomCalTable::on_comboBox_fanCoil_model_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_fanCoil_air_volume->clear();
    ui->lineEdit_fanCoil_static_press->clear();
    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto fanCoil = dynamic_cast<FanCoil*>(component.data());
        if(fanCoil && fanCoil->model == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
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
//空调器编号选择
void RoomCalTable::on_comboBox_aircondition_number_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_aircondition_air_volume->clear();
    ui->lineEdit_aircondition_model->clear();
    ui->lineEdit_aircondition_static_press->clear();
    ui->lineEdit_aircondition_fan_type->clear();

    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto aircondition = dynamic_cast<Aircondition*>(component.data());
        if(aircondition && (aircondition->exhaust_number == arg1 || aircondition->send_number == arg1))
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
    {
        if(auto aircondition = qSharedPointerDynamicCast<Aircondition>(*it))
        {
            currentComponent = (*it);
            if(aircondition->exhaust_number == arg1){
                ui->lineEdit_aircondition_fan_type->setText("排风机");
                ui->lineEdit_aircondition_air_volume->setText(aircondition->exhaust_air_volume);
                ui->lineEdit_aircondition_static_press->setText(aircondition->exhaust_static_pressure);
            }
            else if(aircondition->send_number == arg1) {
                ui->lineEdit_aircondition_fan_type->setText("送风机");
                ui->lineEdit_aircondition_air_volume->setText(aircondition->send_air_volume);
                ui->lineEdit_aircondition_static_press->setText(aircondition->send_static_pressure);
            }
            ui->lineEdit_aircondition_model->setText(aircondition->model);
        }
    }

    on_comboBox_aircondition_noise_locate_currentIndexChanged(ui->comboBox_aircondition_noise_locate->currentIndex());
}

void RoomCalTable::on_comboBox_aircondition_noise_locate_currentIndexChanged(int index)
{
    if(index == -1)
        return;
    if(auto aircondition = qSharedPointerDynamicCast<Aircondition>(currentComponent))
    {
        if(index == 0)
        {
            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                if(ui->lineEdit_aircondition_fan_type->text() == "送风机")
                    noi_after_cal_lineEdits[i]->setText(aircondition->noi_send_in[i]);
                else if(ui->lineEdit_aircondition_fan_type->text() == "排风机")
                    noi_after_cal_lineEdits[i]->setText(aircondition->noi_exhaust_in[i]);
            }
        }
        else if(index == 1)
        {
            for(int i = 0; i < noi_after_cal_lineEdits.size(); i++)
            {
                if(ui->lineEdit_aircondition_fan_type->text() == "送风机")
                    noi_after_cal_lineEdits[i]->setText(aircondition->noi_send_out[i]);
                else if(ui->lineEdit_aircondition_fan_type->text() == "排风机")
                    noi_after_cal_lineEdits[i]->setText(aircondition->noi_exhaust_out[i]);
            }
        }
    }
}
#pragma endregion}
/**********空调器**********/

/**********变风量末端**********/
#pragma region "VAV_terminal"{
void RoomCalTable::on_comboBox_VAV_terminal_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_VAV_terminal_model->currentIndex() == -1)
        return;
    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto vav = qSharedPointerDynamicCast<VAV_terminal>(component);
        // TODO 这里要问清楚是否用model来选择
        if(vav && vav->model == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
    {
        if(auto vav = qSharedPointerDynamicCast<VAV_terminal>(*it))
        {
            currentComponent = (*it);
            ui->lineEdit_VAV_terminal_air_volume->setText(vav->air_volume);
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

void RoomCalTable::on_comboBox_circular_damper_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_circular_damper_model->currentIndex() == -1)
        return;

    if(ui->comboBox_circular_damper_model->currentText() == "经验公式")
    {
        switchComboBoxState(true, ui->comboBox_circular_damper_angle);

        ui->lineEdit_circular_damper_diameter->setReadOnly(false);
        ui->lineEdit_circular_damper_air_volume->setReadOnly(false);

        connect(ui->comboBox_circular_damper_angle, &QComboBox::currentTextChanged, this, &RoomCalTable::circular_damper_noise_cal);
        connect(ui->lineEdit_circular_damper_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::circular_damper_noise_cal);
        connect(ui->lineEdit_circular_damper_diameter, &QLineEdit::textChanged, this, &RoomCalTable::circular_damper_noise_cal);
        return;
    }

    switchComboBoxState(false, ui->comboBox_circular_damper_angle);

    ui->lineEdit_circular_damper_diameter->setReadOnly(true);
    ui->lineEdit_circular_damper_air_volume->setReadOnly(true);

    disconnect(ui->comboBox_circular_damper_angle, &QComboBox::currentTextChanged, this, &RoomCalTable::circular_damper_noise_cal);
    disconnect(ui->lineEdit_circular_damper_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::circular_damper_noise_cal);
    disconnect(ui->lineEdit_circular_damper_diameter, &QLineEdit::textChanged, this, &RoomCalTable::circular_damper_noise_cal);

    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto damper = qSharedPointerDynamicCast<Circular_damper>(component);
        if(damper && damper->model == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
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

void RoomCalTable::on_comboBox_rect_damper_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_rect_damper_model->currentIndex() == -1)
        return;

    if(ui->comboBox_rect_damper_model->currentText() == "经验公式")
    {
        switchComboBoxState(true, ui->comboBox_rect_damper_angle);

        ui->lineEdit_rect_damper_size->setReadOnly(false);
        ui->lineEdit_rect_damper_air_volume->setReadOnly(false);

        connect(ui->comboBox_rect_damper_angle, &QComboBox::currentTextChanged, this, &RoomCalTable::rect_damper_noise_cal);
        connect(ui->lineEdit_rect_damper_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::rect_damper_noise_cal);
        connect(ui->lineEdit_rect_damper_size, &QLineEdit::textChanged, this, &RoomCalTable::rect_damper_noise_cal);
        return;
    }

    switchComboBoxState(false, ui->comboBox_rect_damper_angle);

    ui->lineEdit_circular_damper_diameter->setReadOnly(true);
    ui->lineEdit_circular_damper_air_volume->setReadOnly(true);

    disconnect(ui->comboBox_rect_damper_angle, &QComboBox::currentTextChanged, this, &RoomCalTable::rect_damper_noise_cal);
    disconnect(ui->lineEdit_rect_damper_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::rect_damper_noise_cal);
    disconnect(ui->lineEdit_rect_damper_size, &QLineEdit::textChanged, this, &RoomCalTable::rect_damper_noise_cal);

    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto damper = qSharedPointerDynamicCast<Rect_damper>(component);
        if(damper && damper->model == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
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
void RoomCalTable::on_comboBox_air_distributor_model_currentTextChanged(const QString &arg1)
{
    ui->comboBox_diffuser_model->clear();
    ui->comboBox_diffuser_model->blockSignals(true);
    for(auto& component : currentAllComponentList)
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
    for(auto& component : currentAllComponentList)
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
    for(auto& component : currentAllComponentList)
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

void RoomCalTable::on_comboBox_pump_model_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    ui->lineEdit_pump_size->clear();
    ui->comboBox_pump_terminal_type->clear();
    // 在添加item之前，暂时阻止comboBox的信号
    ui->comboBox_pump_terminal_type->blockSignals(true);
    for(auto& component : currentAllComponentList)
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
    for(auto& component : currentAllComponentList)
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
void RoomCalTable::on_comboBox_send_model_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    ui->lineEdit_send_size->clear();
    ui->comboBox_send_terminal_type->clear();
    // 在添加item之前，暂时阻止comboBox的信号
    ui->comboBox_send_terminal_type->blockSignals(true);
    for(auto& component : currentAllComponentList)
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
    for(auto& component : currentAllComponentList)
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
void RoomCalTable::on_comboBox_staticBox_model_currentTextChanged(const QString &arg1)
{
    ui->comboBox_grille_model->clear();
    ui->comboBox_grille_model->blockSignals(true);
    for(auto& component : currentAllComponentList)
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
    for(auto& component : currentAllComponentList)
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
    for(auto& component : currentAllComponentList)
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
void RoomCalTable::on_comboBox_disp_vent_terminal_model_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    ui->lineEdit_disp_vent_terminal_size->clear();
    ui->comboBox_disp_vent_terminal_type->clear();
    // 在添加item之前，暂时阻止comboBox的信号
    ui->comboBox_disp_vent_terminal_type->blockSignals(true);
    for(auto& component : currentAllComponentList)
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
    for(auto& component : currentAllComponentList)
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
void RoomCalTable::on_comboBox_other_send_terminal_model_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    ui->lineEdit_other_send_terminal_size->clear();
    ui->comboBox_other_send_terminal_type->clear();
    // 在添加item之前，暂时阻止comboBox的信号
    ui->comboBox_other_send_terminal_type->blockSignals(true);
    for(auto& component : currentAllComponentList)
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
    for(auto& component : currentAllComponentList)
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

void RoomCalTable::on_comboBox_static_box_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_static_box_model->currentIndex() == -1)
        return;

    if(arg1 == "经验公式")
    {
        ui->lineEdit_static_box_total_air_volume->setReadOnly(false);
        ui->lineEdit_static_box_branch_air_volume->setReadOnly(false);

        connect(ui->lineEdit_static_box_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::static_box_atten_cal);
        connect(ui->lineEdit_static_box_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::static_box_atten_cal);
        return;
    }

    ui->lineEdit_static_box_total_air_volume->setReadOnly(true);
    ui->lineEdit_static_box_branch_air_volume->setReadOnly(true);

    disconnect(ui->lineEdit_static_box_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::static_box_atten_cal);
    disconnect(ui->lineEdit_static_box_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::static_box_atten_cal);

    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto static_box = qSharedPointerDynamicCast<Static_box>(component);
        if(static_box && static_box->model == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
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

void RoomCalTable::on_comboBox_multi_ranc_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_multi_ranc_model->currentIndex() == -1)
        return;

    if(arg1 == "经验公式")
    {
        ui->lineEdit_multi_ranc_total_air_volume->setReadOnly(false);
        ui->lineEdit_multi_ranc_branch_air_volume->setReadOnly(false);

        connect(ui->lineEdit_multi_ranc_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::multi_ranc_atten_cal);
        connect(ui->lineEdit_multi_ranc_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::multi_ranc_atten_cal);
        return;
    }

    ui->lineEdit_multi_ranc_total_air_volume->setReadOnly(true);
    ui->lineEdit_multi_ranc_branch_air_volume->setReadOnly(true);

    disconnect(ui->lineEdit_multi_ranc_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::multi_ranc_atten_cal);
    disconnect(ui->lineEdit_multi_ranc_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::multi_ranc_atten_cal);

    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto multi_ranc = qSharedPointerDynamicCast<Multi_ranc>(component);
        if(multi_ranc && multi_ranc->model == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
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

void RoomCalTable::on_comboBox_tee_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_tee_model->currentIndex() == -1)
        return;

    if(arg1 == "经验公式")
    {
        ui->lineEdit_tee_total_air_volume->setReadOnly(false);
        ui->lineEdit_tee_branch_air_volume->setReadOnly(false);

        connect(ui->lineEdit_tee_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::tee_atten_cal);
        connect(ui->lineEdit_tee_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::tee_atten_cal);
        return;
    }

    ui->lineEdit_tee_total_air_volume->setReadOnly(true);
    ui->lineEdit_tee_branch_air_volume->setReadOnly(true);

    disconnect(ui->lineEdit_tee_total_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::tee_atten_cal);
    disconnect(ui->lineEdit_tee_branch_air_volume, &QLineEdit::textChanged, this, &RoomCalTable::tee_atten_cal);

    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto tee = qSharedPointerDynamicCast<Tee>(component);
        if(tee && tee->model == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
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
    {
        for(int i = 0; i < each_atten_lineEdits.size(); i++)
        {
            each_atten_lineEdits[i]->setText("");
        }
        return;
    }

    array<double, 8> atten;
    if(ui->comboBox_pipe_type->currentIndex() == 0)
    {
        atten = caPipeNoise(Circle, ui->lineEdit_pipe_size->text().toDouble());
    }
    else{
        auto dimensions = splitDimension(ui->lineEdit_pipe_size->text());
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
        auto dimensions = splitDimension(arg1);
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

    if(arg1 == "经验公式")
    {
        switchComboBoxState(true, ui->comboBox_pipe_type);

        ui->lineEdit_pipe_size->setReadOnly(false);

        connect(ui->lineEdit_pipe_size, &QLineEdit::textChanged, this, &RoomCalTable::pipe_atten_cal);
        return;
    }

    switchComboBoxState(false, ui->comboBox_pipe_type);

    ui->lineEdit_pipe_size->setReadOnly(true);

    disconnect(ui->lineEdit_pipe_size, &QLineEdit::textChanged, this, &RoomCalTable::pipe_atten_cal);

    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto pipe = qSharedPointerDynamicCast<Pipe>(component);
        if(pipe && pipe->model == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
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

    if(arg1 == "经验公式")
    {
        switchComboBoxState(true, ui->comboBox_elbow_type);

        ui->lineEdit_elbow_size->setReadOnly(false);

        connect(ui->lineEdit_elbow_size, &QLineEdit::textChanged, this, &RoomCalTable::pipe_atten_cal);
        return;
    }

    switchComboBoxState(false, ui->comboBox_elbow_type);

    ui->lineEdit_elbow_size->setReadOnly(true);

    disconnect(ui->lineEdit_elbow_size, &QLineEdit::textChanged, this, &RoomCalTable::pipe_atten_cal);

    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto elbow = qSharedPointerDynamicCast<Elbow>(component);
        if(elbow && elbow->model == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
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

void RoomCalTable::on_comboBox_reducer_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_reducer_model->currentIndex() == -1)
        return;

    if(arg1 == "经验公式")
    {
        switchComboBoxState(true, ui->comboBox_reducer_type);

        ui->lineEdit_reducer_before_size->setReadOnly(false);
        ui->lineEdit_reducer_after_size->setReadOnly(false);

        connect(ui->lineEdit_reducer_before_size, &QLineEdit::textChanged, this, &RoomCalTable::reducer_atten_cal);
        connect(ui->lineEdit_reducer_after_size, &QLineEdit::textChanged, this, &RoomCalTable::reducer_atten_cal);
        return;
    }

    switchComboBoxState(false, ui->comboBox_reducer_type);

    ui->lineEdit_reducer_before_size->setReadOnly(true);
    ui->lineEdit_reducer_after_size->setReadOnly(true);

    disconnect(ui->lineEdit_reducer_before_size, &QLineEdit::textChanged, this, &RoomCalTable::reducer_atten_cal);
    disconnect(ui->lineEdit_reducer_after_size, &QLineEdit::textChanged, this, &RoomCalTable::reducer_atten_cal);

    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto reducer = qSharedPointerDynamicCast<Reducer>(component);
        if(reducer && reducer->model == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
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
void RoomCalTable::on_comboBox_silencer_type_currentTextChanged(const QString &arg1)
{
    clearPage(ui->stackedWidget_table->currentWidget(), false);

    ui->comboBox_silencer_model->blockSignals(true);

    for(auto& component : currentAllComponentList)
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
    auto it = std::find_if(this->currentAllComponentList.begin(),this->currentAllComponentList.end(), [&](const QSharedPointer<ComponentBase>& component){
        auto silencer = qSharedPointerDynamicCast<Silencer>(component);
        if(silencer && silencer->model == arg1)
            return true;
        return false;
    });

    if(it != currentAllComponentList.end())
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

/**********体积小于425m2的房间(点噪声源)**********/
#pragma region "roomLess425"{
void RoomCalTable::roomLess425Cal()
{
    if(ui->lineEdit_room_less425_test_distance->text().isEmpty() || ui->lineEdit_room_less425_volume->text().isEmpty()) {
        return;
    }
    array<double, 8> revise = calRoomLess425Revise(ui->lineEdit_room_less425_test_distance->text(),
                                                   ui->lineEdit_room_less425_volume->text());
    variations.clear();
    for(auto i = 0; i < 8; i++) {
        variations.push_back(QString::number(revise[i], 'f', 2));
    }

    for(auto i = 0; i < 8; i++) {
        room_noi_revise_lineEdits[i]->setText(variations[i]);
    }
}
#pragma endregion}
/**********体积小于425m2的房间(点噪声源)**********/

/**********体积大于425m2的房间(点噪声源)**********/
#pragma region "roomMore425"{
void RoomCalTable::roomMore425Cal()
{
    if(ui->lineEdit_room_more425_test_distance->text().isEmpty()) {
        return;
    }
    array<double, 8> revise = calRoomMore425Revise(ui->lineEdit_room_more425_test_distance->text());
    variations.clear();
    for(auto i = 0; i < 8; i++) {
        variations.push_back(QString::number(revise[i], 'f', 2));
    }

    for(auto i = 0; i < 8; i++) {
        room_noi_revise_lineEdits[i]->setText(variations[i]);
    }
}
#pragma endregion}
/**********体积大于425m2的房间(点噪声源)**********/

/**********无家具房间（点噪声源）**********/
#pragma region "roomNoFurniture"{
void RoomCalTable::roomNoFurnitureCal()
{
    if(ui->lineEdit_room_noFurniture_test_distance->text().isEmpty() ||
        ui->lineEdit_room_noFurniture_angle->text().isEmpty() ||
        ui->lineEdit_room_noFurniture_absorption_rate->text().isEmpty() ||
        ui->lineEdit_room_noFurniture_area->text().isEmpty()) {
        return;
    }
    array<double, 8> revise = calRoomNoFurnitureRevise(ui->lineEdit_room_noFurniture_test_distance->text(), ui->lineEdit_room_noFurniture_angle->text(),
                                                       ui->lineEdit_room_noFurniture_absorption_rate->text(), ui->lineEdit_room_noFurniture_area->text());
    variations.clear();
    for(auto i = 0; i < 8; i++) {
        variations.push_back(QString::number(revise[i], 'f', 2));
    }

    for(auto i = 0; i < 8; i++) {
        room_noi_revise_lineEdits[i]->setText(variations[i]);
    }
}
#pragma endregion}
/**********无家具房间（点噪声源）**********/

/**********室外开敞住所(点噪声源)**********/
#pragma region "roomOpen"{
void RoomCalTable::roomOpenCal()
{
    if(ui->lineEdit_room_open_test_distance->text().isEmpty() || ui->lineEdit_room_open_angle->text().isEmpty()) {
        return;
    }
    array<double, 8> revise = calRoomOpenRevise(ui->lineEdit_room_open_test_distance->text(),
                                                ui->lineEdit_room_open_angle->text());
    variations.clear();
    for(auto i = 0; i < 8; i++) {
        variations.push_back(QString::number(revise[i], 'f', 2));
    }

    for(auto i = 0; i < 8; i++) {
        room_noi_revise_lineEdits[i]->setText(variations[i]);
    }
}
#pragma endregion}
/**********室外开敞住所(点噪声源)**********/

/**********条缝风口房间(线噪声源)**********/
#pragma region "roomGapTuyere"{
void RoomCalTable::roomGapTuyereCal()
{
    if(ui->lineEdit_room_gap_tuyere_test_distance->text().isEmpty() ||
        ui->lineEdit_room_gap_tuyere_angle->text().isEmpty() ||
        ui->lineEdit_room_gap_tuyere_absorption_rete->text().isEmpty() ||
        ui->lineEdit_room_gap_tuyere_area->text().isEmpty() ||
        ui->lineEdit_room_gap_tuyere_gap_length->text().isEmpty()) {
        return;
    }
    array<double, 8> revise = calRoomGapTuyereRevise(ui->lineEdit_room_gap_tuyere_test_distance->text(),
                                                     ui->lineEdit_room_gap_tuyere_angle->text(),
                                                     ui->lineEdit_room_gap_tuyere_absorption_rete->text(),
                                                     ui->lineEdit_room_gap_tuyere_area->text(),
                                                     ui->lineEdit_room_gap_tuyere_gap_length->text());
    variations.clear();
    for(auto i = 0; i < 8; i++) {
        variations.push_back(QString::number(revise[i], 'f', 2));
    }

    for(auto i = 0; i < 8; i++) {
        room_noi_revise_lineEdits[i]->setText(variations[i]);
    }
}
#pragma endregion}
/**********条缝风口房间(线噪声源)**********/

/**********雨降风口房间(面噪声源)**********/
#pragma region "roomRain"{
void RoomCalTable::roomRainCal()
{
    if(ui->lineEdit_room_rain_test_distance->text().isEmpty() || ui->lineEdit_room_rain_area->text().isEmpty()) {
        return;
    }
    array<double, 8> revise = calRoomRainRevise(ui->lineEdit_room_rain_test_distance->text(),
                                                ui->lineEdit_room_rain_area->text());
    variations.clear();
    for(auto i = 0; i < 8; i++) {
        variations.push_back(QString::number(revise[i], 'f', 2));
    }

    for(auto i = 0; i < 8; i++) {
        room_noi_revise_lineEdits[i]->setText(variations[i]);
    }
}
#pragma endregion}
/**********雨降风口房间(面噪声源)**********/

