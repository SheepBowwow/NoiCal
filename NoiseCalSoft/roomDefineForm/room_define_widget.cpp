#include "room_define_widget.h"
#include "ui_room_define_widget.h"
#include "roomDefineForm/roomcalinfomanager.h"
#include "databasemanager.h"

#include <QCheckBox>
#include <QMessageBox>


Room_define_widget::Room_define_widget(QString systemName, QWidget *parent) :
    QWidget(parent),
    system_name(systemName),
    ui(new Ui::Room_define_widget)
{
    ui->setupUi(this);
    ui->tableWidget_room_define->verticalHeader()->setVisible(false);

    ui->tableWidget_room_define->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //添加房间同时创建主风管
    connect(&RoomCalInfoManager::getInstance(), &RoomCalInfoManager::roomAdd,
                this, &Room_define_widget::addRoomToTable);

}

Room_define_widget::~Room_define_widget()
{
    delete ui;
}

void Room_define_widget::switchOuter()
{
    isOuter = true;
    ui->label_cg1->setText("定义室外");
    // 设置列标题
    ui->tableWidget_room_define->setColumnCount(10);
    QStringList headerLabels;
    headerLabels<<""<<"室外编号"<<"室外名称"<<"甲板"<<"噪音源支管数量"<<"房间类型"<<"处所类型"<<"噪声限值dB(A)" << "是否计算" << "引用室外编号";
    ui->tableWidget_room_define->setHorizontalHeaderLabels(headerLabels);
}

void Room_define_widget::setSystemItem(QTreeWidgetItem *item)
{
    _systemItem = item;
    mvz_name = _systemItem->parent()->text(0);
}

void Room_define_widget::setTableRowDuctNum(QString room_or_outer_number, QString ductNum)
{
    // 遍历表格的每一行
    for (int row = 0; row < ui->tableWidget_room_define->rowCount(); ++row) {
        // 获取第二列的文本
        QString currentRoomOrOuterNumber = ui->tableWidget_room_define->item(row, 1)->text();

        // 如果第二列文本与指定的房间号或外部号匹配
        if (currentRoomOrOuterNumber == room_or_outer_number) {
            // 设置该行第五列的文本为 ductNum
            QTableWidgetItem *item = ui->tableWidget_room_define->item(row, 4);
            if (item) {
                item->setText(ductNum);
            }
            break;
        }
    }
}

void Room_define_widget::on_pushButton_add_clicked()
{
    if (ui->label_cg1->text() == "定义室外") {
        dialog = new Dialog_add_room(mvz_name);
        dialog->switchOuterMode();
    } else {
        dialog = new Dialog_add_room(system_name);
    }

    // 弹窗点击确定，接收信号并发送提醒主界面改变
    if (dialog->exec() == QDialog::Accepted)
    {
        // 获取Dialog_addroom中的参数并传递给槽函数
        QString roomNumber = dialog->getRoomNumber();
        QString roomName = dialog->getRoomName();
        QString deck = dialog->getDeck();
        QString ductNum = dialog->getDuctNum();
        QString roomType = dialog->getRoomType();
        QString placeType = dialog->getPlaceType();
        QString limit = dialog->getLimit();
        QString isCal = dialog->getIsCal();
        QString referenceNumber = dialog->getReferenceNumber();

        if(!isOuter) {
            //直接调用这个然后RoomCalInfoManager发送信号再调用addRoomToTable
            RoomCalInfoManager::getInstance().createRoom(system_name, roomNumber, roomName, deck,
                                                         ductNum.toInt(), placeType, roomType, limit.toDouble(), isCal, referenceNumber);
        } else {
            //直接调用这个然后RoomCalInfoManager发送信号再调用addRoomToTable
            RoomCalInfoManager::getInstance().createOuter(mvz_name, roomNumber, roomName, deck,
                                                         ductNum.toInt(), placeType, roomType, limit.toDouble(), isCal, referenceNumber);
        }
    }
}

void Room_define_widget::addRoomToTable(const QString& systemName, const QString& roomNumber,
                                        const QString& roomName, const QString& deck, const QString& ductNum,
                                        const QString& roomType, const QString& placeType, const QString& limit,
                                        const QString& isCal, const QString& referenceNumber)
{
    if(systemName != this->system_name)
        return;

    // 始终在表格末尾插入新行
    int rowToInsert = ui->tableWidget_room_define->rowCount();
    ui->tableWidget_room_define->insertRow(rowToInsert);

    // 添加复选框
    QCheckBox* newCheckBox = new QCheckBox();
    QWidget* checkBoxWidget = new QWidget();
    checkBoxWidget->setStyleSheet("background-color: #C0C0C0;");
    QHBoxLayout* layout = new QHBoxLayout(checkBoxWidget);
    layout->addWidget(newCheckBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    ui->tableWidget_room_define->setCellWidget(rowToInsert, 0, checkBoxWidget);

    // 添加内容
    QTableWidgetItem *roomNumberItem = new QTableWidgetItem(roomNumber);
    QTableWidgetItem *roomNameItem = new QTableWidgetItem(roomName);
    QTableWidgetItem *deckItem = new QTableWidgetItem(deck);
    QTableWidgetItem *ductNumItem = new QTableWidgetItem(ductNum);
    QTableWidgetItem *roomTypeItem = new QTableWidgetItem(roomType);
    QTableWidgetItem *placeTypeItem = new QTableWidgetItem(placeType);
    QTableWidgetItem *limitItem = new QTableWidgetItem(limit);
    QTableWidgetItem *isCalItem = new QTableWidgetItem(isCal);
    QTableWidgetItem *referenceNumberItem = new QTableWidgetItem(referenceNumber);
    QList<QTableWidgetItem*> items{roomNumberItem, roomNameItem, deckItem, ductNumItem,
                                   roomTypeItem, placeTypeItem, limitItem, isCalItem, referenceNumberItem};

    int col = 1;
    for (auto& item : items) {
        item->setFlags(Qt::ItemIsEditable); // 设置为可编辑
        item->setBackground(QBrush(Qt::lightGray)); // 背景颜色设置为灰色
        ui->tableWidget_room_define->setItem(rowToInsert, col++, item);
    }
}

void Room_define_widget::on_pushButton_del_clicked()
{
    // 获取选中的行索引
    QList<int> selectedRows;
    for (int row = 0; row < ui->tableWidget_room_define->rowCount(); ++row)
    {
        QWidget* widget = ui->tableWidget_room_define->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox && checkBox->isChecked())
        {
            selectedRows.append(row);
        }
    }

    // 弹窗确认
    QString confirmationMessage = "确认删除所选房间吗？\n";

    QMessageBox msgBox;
    msgBox.setWindowTitle("确认删除");
    msgBox.setText(confirmationMessage);
    msgBox.setIcon(QMessageBox::Warning);
    QPushButton *yesButton = msgBox.addButton("确认", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("取消", QMessageBox::NoRole);
    Q_UNUSED(noButton);
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton)
    {
        for (int row = ui->tableWidget_room_define->rowCount() - 1; row >= 0; --row)
        {
            QWidget *widget = ui->tableWidget_room_define->cellWidget(row, 0); // 提取出第一列的widget
            QCheckBox* checkBoxItem = widget->findChild<QCheckBox*>();          // widget转成checkbox
            QString number = ui->tableWidget_room_define->item(row, 1)->text();
            if (checkBoxItem && checkBoxItem->checkState() == Qt::Checked)
            {
                if(isOuter)
                    RoomCalInfoManager::getInstance().deleteOuter(number);
                else
                    RoomCalInfoManager::getInstance().deleteRoom(number);
                ui->tableWidget_room_define->removeRow(row);
            }
        }
    }
}

void Room_define_widget::on_pushButton_revise_clicked()
{
    // 获取选中的行索引
    int insertRow = -1;
    for (int row = 0; row < ui->tableWidget_room_define->rowCount(); ++row)
    {
        QWidget* widget = ui->tableWidget_room_define->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked())
        {
            insertRow = row;
            break;
        }
    }

    if(insertRow == -1)   return;

    if (ui->label_cg1->text() == "定义室外") {
        dialog = new Dialog_add_room(mvz_name);
        dialog->switchOuterMode();
    } else {
        dialog = new Dialog_add_room(system_name);
    }

    QString roomNumber = ui->tableWidget_room_define->item(insertRow,1)->text();
    QString roomName = ui->tableWidget_room_define->item(insertRow,2)->text();
    QString deck = ui->tableWidget_room_define->item(insertRow,3)->text();
    QString ductNum = ui->tableWidget_room_define->item(insertRow,4)->text();
    QString placeType = ui->tableWidget_room_define->item(insertRow,5)->text();
    QString roomType = ui->tableWidget_room_define->item(insertRow,6)->text();
    QString limit = ui->tableWidget_room_define->item(insertRow,7)->text();
    QString isCal = ui->tableWidget_room_define->item(insertRow,8)->text();
    QString referenceNumber = ui->tableWidget_room_define->item(insertRow,9)->text();

    RoomDBData old_room_data{roomNumber, roomName, deck, ductNum.toInt(), placeType, roomType, limit.toDouble(), isCal, referenceNumber};
    OuterDBData& old_outer_data = old_room_data;
    dialog->setValues(roomNumber,roomName,deck,ductNum, placeType, roomType, limit, isCal, referenceNumber);

    if(dialog->exec() == QDialog::Accepted)
    {
        roomNumber = dialog->getRoomNumber();
        roomName = dialog->getRoomName();
        deck = dialog->getDeck();
        ductNum = dialog->getDuctNum();
        placeType = dialog->getPlaceType();
        roomType = dialog->getRoomType();
        limit = dialog->getLimit();
        isCal = dialog->getIsCal();
        referenceNumber = dialog->getReferenceNumber();

        RoomDBData new_room_data{roomNumber, roomName, deck, ductNum.toInt(), placeType, roomType, limit.toDouble(), isCal, referenceNumber};
        OuterDBData& new_outer_data = new_room_data;
        if(!isOuter)
            RoomCalInfoManager::getInstance().updateRoom(system_name, old_room_data, new_room_data);
        else
            RoomCalInfoManager::getInstance().updateOuter(_systemItem->parent()->text(0), old_outer_data, new_outer_data);

        //添加内容
        QTableWidgetItem *roomNumberItem = new QTableWidgetItem(roomNumber);
        QTableWidgetItem *roomNameItem = new QTableWidgetItem(roomName);
        QTableWidgetItem *deckItem = new QTableWidgetItem(deck);
        QTableWidgetItem *ductNumItem = new QTableWidgetItem(ductNum);
        QTableWidgetItem *roomTypeItem = new QTableWidgetItem(placeType);
        QTableWidgetItem *placeTypeItem = new QTableWidgetItem(roomType);
        QTableWidgetItem *limitItem = new QTableWidgetItem(limit);
        QTableWidgetItem *isCalItem = new QTableWidgetItem(isCal);
        QTableWidgetItem *referenceNumberItem = new QTableWidgetItem(referenceNumber);
        QList<QTableWidgetItem*> items{roomNumberItem, roomNameItem, deckItem, ductNumItem,
                                        roomTypeItem, placeTypeItem, limitItem, isCalItem, referenceNumberItem};

        int col = 1;
        for(auto& item : items) {
            item->setFlags(Qt::ItemIsEditable); // 设置为只读
            item->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
            ui->tableWidget_room_define->setItem(insertRow, col++, item);
        }
    }
}
