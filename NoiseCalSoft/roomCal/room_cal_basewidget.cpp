#include "room_cal_basewidget.h"
#include "ui_room_cal_basewidget.h"
#include "roomCal/roomcaltable.h"
#include <QVBoxLayout>
#include <QMenu>

room_cal_baseWidget::room_cal_baseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::room_cal_baseWidget)
{
    ui->setupUi(this);
    QWidget *scrollWidget = new QWidget;
    QVBoxLayout *scrollLayout = new QVBoxLayout(ui->scrollArea);
    scrollLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

//    // 设置垂直方向上的间距为10像素
    scrollLayout->setSpacing(30);
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
        layout->addWidget(newRoomCalTable);
    else
        layout->insertWidget(index, newRoomCalTable);
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
        }

        // 删除布局项
        delete item;

        ui->scrollArea->widget()->update();  // 或者使用 repaint()
    }, Qt::QueuedConnection);
}

void room_cal_baseWidget::on_pushButton_add_clicked()
{
    // 创建下拉菜单
    QMenu *menu = new QMenu(this);

    QAction *actionNoiSrc = menu->addAction("声源噪音");
    QAction *actionAirflowNoi = menu->addAction("气流噪音");
    QAction *actionAttenAirflowNoi = menu->addAction("噪音衰减+气流噪音");
    QAction *actionAtten = menu->addAction("噪音衰减");
    QAction *actionSoundPressure = menu->addAction("声压级计算");

    // Connect each action to a corresponding slot
    connect(actionNoiSrc, &QAction::triggered, this, &room_cal_baseWidget::onNoiSrcClicked);
    connect(actionAirflowNoi, &QAction::triggered, this, &room_cal_baseWidget::onAirflowNoiClicked);
    connect(actionAttenAirflowNoi, &QAction::triggered, this, &room_cal_baseWidget::onAttenAirflowNoiClicked);
    connect(actionAtten, &QAction::triggered, this, &room_cal_baseWidget::onAttenClicked);
    connect(actionSoundPressure, &QAction::triggered, this, &room_cal_baseWidget::onSoundPressureClicked);

    // 设置菜单的水平对齐方式为居中
    menu->setStyleSheet("QMenu::item {padding:5px 32px; color:rgba(51,51,51,1); font-size:12px;margin:0px 8px;}"
                       "QMenu::item:hover {background-color:#409CE1;}"
                       "QMenu::item:selected {background-color:#409CE1;}");

    // 获取按钮的下方中心位置，使菜单相对于按钮弹出
    QPoint buttonPos = ui->pushButton_add->mapToGlobal(ui->pushButton_add->rect().bottomLeft());

    // 将下拉菜单居中在按钮的下方
    QPoint menuPos = QPoint(buttonPos.x() - menu->width() / 2, buttonPos.y());

    // 显示下拉菜单
    menu->exec(menuPos);
}

// Define slots for each action
void room_cal_baseWidget::onNoiSrcClicked()
{
    addTable(-1,"声源噪音");
}

void room_cal_baseWidget::onAirflowNoiClicked()
{
    // Your code for "气流噪音" action
    addTable(-1,"气流噪音");
}

void room_cal_baseWidget::onAttenAirflowNoiClicked()
{
    // Your code for "噪音衰减+气流噪音" action
    addTable(-1,"噪音衰减+气流噪音");
}

void room_cal_baseWidget::onAttenClicked()
{
    // Your code for "噪音衰减" action
    addTable(-1,"噪音衰减");
}

void room_cal_baseWidget::onSoundPressureClicked()
{
    // Your code for "声压级计算" action
    addTable(-1,"声压级计算");
}
