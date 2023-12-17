#include "room_cal_basewidget.h"
#include "ui_room_cal_basewidget.h"
#include "roomCal/roomcaltable.h"
#include <QVBoxLayout>
#include <QMenu>

// 定义静态成员
QMenu *room_cal_baseWidget::menu = nullptr;

room_cal_baseWidget::room_cal_baseWidget(QWidget *parent) :
    QWidget(parent),
    isAllCollapsed(false),
    ui(new Ui::room_cal_baseWidget)
{   
    ui->setupUi(this);
    QWidget *scrollWidget = new QWidget;
    QVBoxLayout *scrollLayout = new QVBoxLayout(ui->scrollArea);
    scrollLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    if (!menu)
    {
        // 创建 QMenu 实例，如果已经创建过，就不再重复创建
        menu = new QMenu();
        // 添加初始的菜单项
        addActionToMenu("声源噪音", [=]() { addTable(-1, "声源噪音"); });
        addActionToMenu("气流噪音", [=]() { addTable(-1, "气流噪音"); });
        addActionToMenu("噪音衰减+气流噪音", [=]() { addTable(-1, "噪音衰减+气流噪音"); });
        addActionToMenu("噪音衰减", [=]() { addTable(-1, "噪音衰减"); });
        addActionToMenu("声压级计算", [=]() { addTable(-1, "声压级计算"); });

        // 设置菜单的水平对齐方式为居中
        menu->setStyleSheet("QMenu::item {padding:5px 32px; color:rgba(51,51,51,1); font-size:12px;margin:0px 8px;}"
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

void room_cal_baseWidget::addTable(int index, QString type)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollArea->widget()->layout());
    if (!layout || index < -1 || index > layout->count()) {
        return;
    }

    RoomCalTable *newRoomCalTable = new RoomCalTable(nullptr,type);
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

void room_cal_baseWidget::on_pushButton_add_clicked()
{
    QPoint buttonPos = ui->pushButton_add->mapToGlobal(ui->pushButton_add->rect().bottomLeft());

    // 将下拉菜单居中在按钮的下方
    QPoint menuPos = QPoint(buttonPos.x() - menu->width() / 2, buttonPos.y());

    // 显示下拉菜单
    menu->exec(menuPos);
}

void room_cal_baseWidget::addActionToMenu(const QString& itemName, const std::function<void()>& slotFunction)
{
    QAction* action = menu->addAction(itemName);

    // 使用 lambda 表达式关联槽函数
    connect(action, &QAction::triggered, this, slotFunction);

    // 将 QAction 添加到菜单中
    menu->addAction(action);
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

