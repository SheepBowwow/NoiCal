#include "room_cal_basewidget.h"
#include "ui_room_cal_basewidget.h"
#include "roomCal/roomcaltable.h"
#include <QVBoxLayout>
#include <QMenu>
#include <QDebug>
#include "globle_var.h"


room_cal_baseWidget::room_cal_baseWidget(QWidget *parent, QString m_roomName) :
    QWidget(parent),
    isAllCollapsed(false),
    roomName(m_roomName),
    ui(new Ui::room_cal_baseWidget)
{   
    ui->setupUi(this);

    if(m_roomName == "")
    {
        ui->pushButton_confirm->hide();
    }

    QWidget *scrollWidget = new QWidget;
    QVBoxLayout *scrollLayout = new QVBoxLayout(ui->scrollArea);
    scrollLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    // 全局菜单只初始化一次
    if (!globalMenu) {
        globalMenu = new QMenu();
        // 添加初始的菜单项
        globalMenu->addAction("声源噪音");
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

room_cal_baseWidget::~room_cal_baseWidget()
{
    delete ui;
}

void room_cal_baseWidget::handleMenuAction(QString actionName)
{
    if(actionName == "声源噪音" || actionName == "气流噪音" || actionName == "噪音衰减+气流噪音" || actionName == "噪音衰减" || actionName == "声压级计算")
    {
        this->addTable(-1,actionName);
    }
    else
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

void room_cal_baseWidget::updateAllTables()
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

void room_cal_baseWidget::addTable(int index, QString type)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    if (!layout || index < -1 || index > layout->count()) {
        return;
    }

    RoomCalTable *newRoomCalTable = new RoomCalTable(systemName,nullptr,type);
    //绑定信号与槽，当有数据改变时，更新所有表格
    connect(newRoomCalTable, &RoomCalTable::tableChanged, this, &room_cal_baseWidget::updateAllTables);
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
    for (int i = 0; i < index; ++i) {
        beforeCalTable = qobject_cast<RoomCalTable *>(layout->itemAt(i)->widget());
    }

    //设置表格上一个的值
    if (beforeCalTable) {
        newRoomCalTable->noise_before_cal = beforeCalTable->noise_after_cal;
    }

    // Connect signals and slots for the new RoomCalTable
    connect(newRoomCalTable, &RoomCalTable::addBeforeClicked, this, &room_cal_baseWidget::handleAddBefore);
    connect(newRoomCalTable, &RoomCalTable::addAfterClicked, this, &room_cal_baseWidget::handleAddAfter);
    connect(newRoomCalTable, &RoomCalTable::deleteClicked, this, &room_cal_baseWidget::handleDelete);

    // Set the new RoomCalTable as the selected RoomCalTable
    selectedTable = newRoomCalTable;
}

void room_cal_baseWidget::handleAddBefore(int index)
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

void room_cal_baseWidget::handleAddAfter(int index)
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

void room_cal_baseWidget::handleDelete(int index)
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

void room_cal_baseWidget::addMenuAction(QString itemName)
{
    QAction *newAction = new QAction(itemName, this);
    globalMenu->addAction(newAction);
}

void room_cal_baseWidget::on_pushButton_add_clicked()
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

void room_cal_baseWidget::on_pushButton_fold_clicked()
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

void room_cal_baseWidget::on_pushButton_confirm_clicked()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    QVector<QWidget*> widgets;
    if (layout) {
        // 遍历垂直布局中的所有widget
        for (int i = 0; i < layout->count(); ++i) {
            widgets.push_back(layout->itemAt(i)->widget());
        }
    }
    if(roomName != "")
    {
        classicRoomMap[roomName] = widgets;
    }
}

void room_cal_baseWidget::setInfo(QString zhushuqu, QString deck, QString room_number, QString noise_limit, QString duct_num)
{
    ui->lineEdit_zhushuqu->setText(zhushuqu);
    ui->lineEdit_deck->setText(deck);
    ui->lineEdit_room_number->setText(room_number);
    ui->lineEdit_noise_limit->setText(noise_limit);
    ui->lineEdit_duct_num->setText(duct_num);
}

void room_cal_baseWidget::setMainDuctNumber(QString number) //设置主风管编号
{
    ui->lineEdit_main_duct_number->setText(number);
}

void room_cal_baseWidget::setSystemName(QString systemName) //设置主风管编号
{
    this->systemName = systemName;
}

void room_cal_baseWidget::change_outer_cal()
{
    title_label="噪音源支管";
    ui->label_cg1->setText("室外编号:");
    ui->label_cg2->setText("噪音源支管数量:");
}
