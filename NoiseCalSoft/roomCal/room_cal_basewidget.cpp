#include "room_cal_basewidget.h"
#include "ui_room_cal_basewidget.h"
#include "roomCal/roomcaltable.h"
#include <QVBoxLayout>
#include <QMenu>
#include <QDebug>
#include "globle_var.h"
#include "databasemanager.h"
#include "room_cal_total_widget.h"
#include <QMessageBox>

///@test
#include <QJsonObject>

Room_cal_baseWidget::Room_cal_baseWidget(QWidget *parent, bool isOuter, bool named) :
    QWidget(parent),
    isAllCollapsed(false),
    isNamed(named),
    _isOuter(isOuter),
    _systemName(QLatin1String("")),
    _cal_total_page(nullptr),
    _loadDataMod(false),
    ui(new Ui::Room_cal_baseWidget)
{   
    ui->setupUi(this);

    QWidget *scrollWidget = new QWidget;
    QVBoxLayout *scrollLayout = new QVBoxLayout(ui->scrollArea);
    scrollLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    // 全局菜单只初始化一次
    if (!globalMenu) {
        globalMenu = new QMenu();
        // 添加初始的菜单项
        globalMenu->addAction("噪音源");
        globalMenu->addAction("气流噪音");
        globalMenu->addAction("噪音衰减+气流噪音");
        globalMenu->addAction("噪音衰减");
        globalMenu->addAction("声压级计算");

        // 设置菜单的水平对齐方式为居中
        globalMenu->setStyleSheet("QMenu::item {padding:5px 32px; color:rgba(51,51,51,1); font-size:12px;margin:0px 8px;}"
                           "QMenu::item:hover {background-color:#409CE1;}"
                           "QMenu::item:selected {background-color:#409CE1;}");
    }


    // 设置垂直方向上的间距为10像素
    scrollLayout->setSpacing(2);
    scrollLayout->setContentsMargins(0, 15, 0, 15);

    ui->scrollArea->setWidgetResizable(true);
    scrollWidget->setLayout(scrollLayout);
    ui->scrollArea->setWidget(scrollWidget);
}

Room_cal_baseWidget::~Room_cal_baseWidget()
{
    delete ui;
}

void Room_cal_baseWidget::handleMenuAction(QString actionName)
{
    if(actionName == "噪音源" || actionName == "气流噪音"
        || actionName == "噪音衰减+气流噪音" || actionName == "噪音衰减" || actionName == "声压级计算")
    {
        this->addTable(-1,actionName);
    }
    else    //典型房间
    {
        if(classicRoomMap.find(actionName) != classicRoomMap.end())
        {
            QVector<QWidget*> v = classicRoomMap[actionName];
            for(int i = 0; i < v.size(); i++)
            {
                RoomCalTable * table = qobject_cast<RoomCalTable *>(v[i]);
                this->addTable(-1,table->currentTableType);
            }
        }
    }
}

double Room_cal_baseWidget::getDuctAWeightNoise()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    RoomCalTable *lastTable = qobject_cast<RoomCalTable *>(layout->itemAt(layout->count() - 1)->widget());
    return lastTable->getDuctAWidgetNoise();
}

double Room_cal_baseWidget::getDuctTestPointDistance()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    RoomCalTable *lastTable = qobject_cast<RoomCalTable *>(layout->itemAt(layout->count() - 1)->widget());
    return lastTable->getDuctTestPointDistance();
}

void Room_cal_baseWidget::updateAllTables()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    for (int i = 1; i < layout->count(); ++i) {
        RoomCalTable *beforeRoomCalTable = qobject_cast<RoomCalTable *>(layout->itemAt(i - 1)->widget());
        RoomCalTable *currentRoomCalTable = qobject_cast<RoomCalTable *>(layout->itemAt(i)->widget());
        //更新所有表格的noise_before_cal,并且计算变化量,和结果
        if (beforeRoomCalTable && currentRoomCalTable) {
            currentRoomCalTable->noise_before_cal = beforeRoomCalTable->noise_after_cal;
            currentRoomCalTable->calVariations();
        }
    }
}

void Room_cal_baseWidget::addTable(int index, QString type)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    if (!layout || index < -1 || index > layout->count()) {
        return;
    }

    RoomCalTable *newRoomCalTable = new RoomCalTable(_systemName, nullptr, _isOuter, type);
    //绑定信号与槽，当有数据改变时，更新所有表格
    connect(newRoomCalTable, &RoomCalTable::tableChanged, this, &Room_cal_baseWidget::updateAllTables);
    if (!newRoomCalTable->isValid) {
        delete newRoomCalTable;
        return;
    }
    if(index == -1)
    {
        layout->addWidget(newRoomCalTable);
        newRoomCalTable->setSerialNum(layout->count());
    }
    else
    {
        layout->insertWidget(index, newRoomCalTable);
        // 遍历垂直布局中的所有widget
        for (int i = 0; i < layout->count(); ++i) {
            RoomCalTable *roomCalTable = qobject_cast<RoomCalTable *>(layout->itemAt(i)->widget());
            if (roomCalTable) {
                roomCalTable->setSerialNum(i + 1);
            }
        }
    }

    //遍历到插入的表格的上一个表格
    RoomCalTable *beforeCalTable = nullptr;
    for (int i = 0; i < layout->count() - 1; ++i) {
        beforeCalTable = qobject_cast<RoomCalTable *>(layout->itemAt(i)->widget());
    }

    //设置表格上一个的值
    if (beforeCalTable) {
        newRoomCalTable->noise_before_cal = beforeCalTable->noise_after_cal;
    }

    // Connect signals and slots for the new RoomCalTable
    connect(newRoomCalTable, &RoomCalTable::addBeforeClicked, this, &Room_cal_baseWidget::handleAddBefore);
    connect(newRoomCalTable, &RoomCalTable::addAfterClicked, this, &Room_cal_baseWidget::handleAddAfter);
    connect(newRoomCalTable, &RoomCalTable::deleteClicked, this, &Room_cal_baseWidget::handleDelete);

    // Set the new RoomCalTable as the selected RoomCalTable
    selectedTable = newRoomCalTable;
    updateAllTables();
}

void Room_cal_baseWidget::addTable(const RoomCalTableType &type, const QJsonObject &jsonObj)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    if (!layout) {
        return;
    }

    RoomCalTable *newRoomCalTable = new RoomCalTable(_systemName, nullptr, _isOuter);

    // 绑定信号与槽，当有数据改变时，更新所有表格
    connect(newRoomCalTable, &RoomCalTable::tableChanged, this, &Room_cal_baseWidget::updateAllTables);

    if (!newRoomCalTable->isValid) {
        delete newRoomCalTable;
        return;
    }

    // 将表格添加到布局的末尾
    layout->addWidget(newRoomCalTable);
    newRoomCalTable->setSerialNum(layout->count());

    // 调用 `callSetTableInfoFunc` 方法来设置表格信息
    newRoomCalTable->callSetTableInfoFunc(type, jsonObj);

    // 遍历到插入的表格的上一个表格
    RoomCalTable *beforeCalTable = nullptr;
    if (layout->count() > 1) {
        beforeCalTable = qobject_cast<RoomCalTable *>(layout->itemAt(layout->count() - 2)->widget());
    }

    // 设置表格上一个的值
    if (beforeCalTable) {
        newRoomCalTable->noise_before_cal = beforeCalTable->noise_after_cal;
    }

    // Connect signals and slots for the new RoomCalTable
    connect(newRoomCalTable, &RoomCalTable::addBeforeClicked, this, &Room_cal_baseWidget::handleAddBefore);
    connect(newRoomCalTable, &RoomCalTable::addAfterClicked, this, &Room_cal_baseWidget::handleAddAfter);
    connect(newRoomCalTable, &RoomCalTable::deleteClicked, this, &Room_cal_baseWidget::handleDelete);

    // Set the new RoomCalTable as the selected RoomCalTable
    selectedTable = newRoomCalTable;
    updateAllTables();
}

void Room_cal_baseWidget::handleAddBefore(int index)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    if (!layout || index < 0 || index >= layout->count()) {
        return;
    }

    selectedTable = qobject_cast<RoomCalTable *>(layout->itemAt(index)->widget());
    if (!selectedTable) {
        return;
    }

    // Create a new RoomCalTable in the main thread
    QMetaObject::invokeMethod(this, [this, index, layout]() {
        // Call the addTable function with default value for 'type'
        addTable(index);

        // Set the new RoomCalTable as the selected RoomCalTable
        selectedTable = qobject_cast<RoomCalTable *>(layout->itemAt(index)->widget());
    }, Qt::QueuedConnection);
}

void Room_cal_baseWidget::handleAddAfter(int index)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    if (!layout || index < 0 || index >= layout->count()) {
        return;
    }

    selectedTable = qobject_cast<RoomCalTable *>(layout->itemAt(index)->widget());
    if (!selectedTable) {
        return;
    }

    // Create a new RoomCalTable in the main thread
    QMetaObject::invokeMethod(this, [this, index, layout]() {
        // Call the addTable function with 'index + 1' to insert the new table after the selected one
        addTable(index + 1);

        // Set the new RoomCalTable as the selected RoomCalTable
        selectedTable = qobject_cast<RoomCalTable *>(layout->itemAt(index + 1)->widget());
    }, Qt::QueuedConnection);
}

void Room_cal_baseWidget::handleDelete(int index)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());

    // Delete the RoomCalTable in the main thread
    QMetaObject::invokeMethod(this, [this, index ,layout]() {
        QLayoutItem *item = layout->takeAt(index);
        RoomCalTable *roomCalTable = qobject_cast<RoomCalTable *>(item->widget());
        if (roomCalTable) {
            // 断开与父对象的关系
            roomCalTable->setParent(nullptr);
            // 手动删除对象
            delete roomCalTable;
            // 遍历垂直布局中的所有widget
            for (int i = 0; i < layout->count(); ++i) {
                RoomCalTable *roomCalTable = qobject_cast<RoomCalTable *>(layout->itemAt(i)->widget());
                if (roomCalTable) {
                    roomCalTable->setSerialNum(i + 1);
                }
            }
        }

        // 删除布局项
        delete item;

        ui->scrollArea->widget()->update();  // 或者使用 repaint()
    }, Qt::QueuedConnection);
}

void Room_cal_baseWidget::addMenuAction(QString itemName)
{
    QAction *newAction = new QAction(itemName, this);
    globalMenu->addAction(newAction);
}

void Room_cal_baseWidget::on_pushButton_add_clicked()
{
    QPoint buttonPos = ui->pushButton_add->mapToGlobal(ui->pushButton_add->rect().bottomLeft());

    globalMenu->show();
    globalMenu->close();

    // 将下拉菜单居中在按钮的下方
    QPoint menuPos = QPoint(buttonPos.x() - globalMenu->width() / 2, buttonPos.y());

    QAction* action = globalMenu->exec(menuPos);
    if(action)
    {
        // 显示下拉菜单
        this->handleMenuAction(action->text());
    }
}

void Room_cal_baseWidget::on_pushButton_fold_clicked()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    if (layout) {
        // 遍历垂直布局中的所有widget
        for (int i = 0; i < layout->count(); ++i) {
            RoomCalTable *roomCalTable = qobject_cast<RoomCalTable *>(layout->itemAt(i)->widget());
            if (roomCalTable) {
                if(isAllCollapsed)
                {
                    roomCalTable->setIsCollapsed(true);
                }
                else
                {
                    roomCalTable->setIsCollapsed(false);
                }
                roomCalTable->setCollapsed();
            }
        }
    }
    isAllCollapsed = !isAllCollapsed;
}

void Room_cal_baseWidget::on_pushButton_confirm_clicked()
{
    //QVector<QWidget*> widgets;
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    if (!layout) {
        qDebug() << "layout is not exist";
        return;
    }

    QStringList list;
    for (int i = 0; i < layout->count(); ++i) {
        RoomCalTable* table = static_cast<RoomCalTable*>(layout->itemAt(i)->widget());
        if(!table->isTableCompleted()) {
            list.append(QString::number(i + 1));
        }
    }

    if (!list.isEmpty()) {
        // 拼接提示信息
        QString message = "请完善以下表格：\n";
        for (const QString& item : list) {
            message += item + "\n";
        }
        // 使用 QMessageBox::warning 显示警告对话框
        QMessageBox::warning(nullptr, "表格不完整", message);
        return;
    }

    list.clear();
    int noiseTableNum = 0;
    int roomCalTableNum = 0;
    for (int i = 0; i < layout->count(); ++i) {
        RoomCalTable* table = static_cast<RoomCalTable*>(layout->itemAt(i)->widget());
        if(table->isRoomCalTable()) {
            roomCalTableNum++;
        }
        if(table->isNoiseTable()) {
            noiseTableNum++;
        }
    }
    if(noiseTableNum != 1 || roomCalTableNum != 1) {
        if(!_loadDataMod)
            QMessageBox::warning(nullptr, "警告", "请检查噪音源表格和声压级计算表格是否存在,或存在多个");
        return;
    }

    _cal_total_page->addDuctToTable(_ductNumber, getDuctTestPointDistance(), getDuctAWeightNoise());

    if(!_loadDataMod)
    {
        //先去除之前的数据再插入新的数据
        DatabaseManager::getInstance().removeDuctCalTableFromDatabase(_isOuter, _ductNumber);
        for (int i = 0; i < layout->count(); ++i) {
            RoomCalTable* table = static_cast<RoomCalTable*>(layout->itemAt(i)->widget());
            RoomCalTableType type = RoomCalTableType::UNDEFINED;
            QJsonObject jsonObj;
            table->createTableInfoJsonObj(type, jsonObj);
            if(jsonObj.isEmpty() || type == RoomCalTableType::UNDEFINED) {
                qDebug() << "json is empty or setType faile";
                return;
            }
            QString jsonString = QString(QJsonDocument(jsonObj).toJson());
            if(!DatabaseManager::getInstance().addDuctCalTableToDatabase(_isOuter, i, roomCalTableTypeToString(type),
                                                                          jsonString, _MVZName, _systemName, _roomOrOuterNumber, _ductNumber)) {
                return;
            }
        }
    }

    ///TODO 将典型房间的添加改一下
    // if(roomName != "")
    // {
    //     classicRoomMap[roomName] = widgets;
    // }
}

void Room_cal_baseWidget::set_cal_total_page(Room_cal_total_widget *newCal_total_page)
{
    _cal_total_page = newCal_total_page;
}

void Room_cal_baseWidget::setInfo(QString MVZName, QString deck, QString roomOrOuterNumber)
{
    ui->lineEdit_MVZName->setText(MVZName);
    ui->lineEdit_deck->setText(deck);
    ui->lineEdit_room_number->setText(roomOrOuterNumber);
    _roomOrOuterNumber = roomOrOuterNumber;
    _MVZName = MVZName;
}

void Room_cal_baseWidget::setDuctNumber(QString number) //设置主风管编号
{
    ui->lineEdit_main_duct_number->setText(number);
    _ductNumber = number;
}

void Room_cal_baseWidget::setAirVolume(QString airVolume)
{
    ui->lineEdit_air_volume->setText(airVolume);
}

void Room_cal_baseWidget::setSystemName(QString systemName) //设置系统名
{
    this->_systemName = systemName;
}

void Room_cal_baseWidget::setMVZName(QString MVZName)
{
    ui->lineEdit_MVZName->setText(MVZName);
    _MVZName = MVZName;
}

bool Room_cal_baseWidget::getIsOuter()
{
    return _isOuter;
}

void Room_cal_baseWidget::loadDataToCalTotalTable()
{
    _loadDataMod = true;
    on_pushButton_confirm_clicked();
    _loadDataMod = false;
}

void Room_cal_baseWidget::handle_duct_number_revise(QString origin_number, QString new_number)
{
    _cal_total_page->handle_duct_number_revise(origin_number, new_number);
}

void Room_cal_baseWidget::switch_outer_cal()
{
    title_label="噪音源支管";
    ui->label_cg1->setText("室外编号:");
    _isOuter = true;
}
