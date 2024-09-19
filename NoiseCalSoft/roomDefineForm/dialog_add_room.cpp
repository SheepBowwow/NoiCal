#include "dialog_add_room.h"
#include "ui_dialog_add_room.h"
#include "globle_var.h"
#include "roomDefineForm/roomcalinfomanager.h"

Dialog_add_room::Dialog_add_room(QString systemOrMVZName, QWidget *parent) :
    InputBaseDialog(parent),
    _systemOrMVZName(systemOrMVZName),
    ui(new Ui::Dialog_add_room)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);  // 写在窗口类构造函数里，隐藏边框
    setTopWidget(ui->widget_top);

    for(auto& noiseLimit : ProjectManager::getInstance().getNoiseLimits())
    {
        if(ui->comboBox_place_type->findText(noiseLimit.placeType) == -1)
            ui->comboBox_place_type->addItem(noiseLimit.placeType);
    }

    for(auto& referenceRoomNumber : RoomCalInfoManager::getInstance().getCalRoomNumbers(systemOrMVZName)) {
        ui->comboBox_reference->addItem(referenceRoomNumber);
    }
    ui->checkBox_is_cal->setCheckState(Qt::Checked);
}

Dialog_add_room::~Dialog_add_room()
{
    delete ui;
}

void Dialog_add_room::setValues(QString roomNumber, QString roomName, QString deck,
                                QString ductNum, QString placeType, QString roomType,
                                QString limit, QString isCal, QString referenceNumber)
{
    if(isCal == "是") {
        ui->checkBox_is_cal->setCheckState(Qt::Checked);
        int index = ui->comboBox_reference->findText(roomNumber);
        if(index != -1)
            ui->comboBox_reference->removeItem(index);
    } else if (isCal == "否") {
        ui->checkBox_is_cal->setCheckState(Qt::Unchecked);
        ui->comboBox_reference->setCurrentText(referenceNumber);
    }
    ui->lineEdit_room_number->setText(roomNumber);
    ui->lineEdit_room_name->setText(roomName);
    ui->lineEdit_deck->setText(deck);
    ui->lineEdit_duct_num->setText(ductNum);
    ui->lineEdit_limit->setText(limit);
    ui->comboBox_place_type->setCurrentText(placeType);
    ui->comboBox_room_type->setCurrentText(roomType);

    ui->lineEdit_duct_num->setReadOnly(true);
}

QString Dialog_add_room::getRoomNumber()
{
    return ui->lineEdit_room_number->text();
}

QString Dialog_add_room::getRoomName()
{
    return ui->lineEdit_room_name->text();
}

QString Dialog_add_room::getDeck()
{
    return ui->lineEdit_deck->text();
}

QString Dialog_add_room::getDuctNum()
{
    return ui->lineEdit_duct_num->text();
}

QString Dialog_add_room::getRoomType()
{
    return ui->comboBox_room_type->currentText();
}

QString Dialog_add_room::getLimit()
{
    return ui->lineEdit_limit->text();
}

QString Dialog_add_room::getPlaceType()
{
    return ui->comboBox_place_type->currentText();
}

QString Dialog_add_room::getIsCal()
{
    return ui->checkBox_is_cal->isChecked() ? "是" : "否";
}

QString Dialog_add_room::getReferenceNumber()
{
    return ui->checkBox_is_cal->isChecked() ? "-" : ui->comboBox_reference->currentText();
}

void Dialog_add_room::switchOuterMode()
{
    ui->label_title->setText("定义室外");
    ui->label_number->setText("室外编号:");
    ui->label_name->setText("室外名称:");
    ui->label_duct_num->setText("噪音源支管数量:");
    ui->label_reference->setText("引用室外编号");

    ui->comboBox_place_type->addItem("外部区域");
    ui->comboBox_place_type->setCurrentText("外部区域");
    // 设置为只读
    ui->comboBox_place_type->setEnabled(false);

    ui->comboBox_reference->clear();
    for(auto& referenceOuterNumber : RoomCalInfoManager::getInstance().getCalOuterNumbers(_systemOrMVZName)) {
        ui->comboBox_reference->addItem(referenceOuterNumber);
    }
}

void Dialog_add_room::on_pushButton_confirm_clicked()
{
    if(ui->lineEdit_duct_num->text().isEmpty()||
            ui->lineEdit_limit->text().isEmpty()||
            ui->lineEdit_deck->text().isEmpty()||
            ui->lineEdit_room_number->text().isEmpty()||
            ui->lineEdit_room_name->text().isEmpty() ||
        (!(ui->checkBox_is_cal->isChecked()) && ui->comboBox_reference->currentText().isEmpty()))
    {
        QMessageBox::information(this,"提示","内容未填写完整");
        return;
    }

    accept();
    close();
}

void Dialog_add_room::on_close_clicked()
{
    close();
}

void Dialog_add_room::on_comboBox_place_type_currentTextChanged(const QString &arg1)
{
    QList<NoiseLimit> noiseLimits = ProjectManager::getInstance().getNoiseLimits();
    ui->comboBox_room_type->clear();
    // 假设rooms是QList<Room>，且已经在类中定义和填充了数据
    for(const NoiseLimit &noiseLimit : std::as_const(noiseLimits)) {
        if(noiseLimit.placeType == arg1) {
            ui->comboBox_room_type->addItem(noiseLimit.roomType);
        }
    }
}

//房间类型选择后更改下面两项
void Dialog_add_room::on_comboBox_room_type_currentTextChanged(const QString &arg1)
{
    QList<NoiseLimit> noiseLimits = ProjectManager::getInstance().getNoiseLimits();
    NoiseLimit matchedRoom;
    // 假设rooms是QList<Room>，且已经在类中定义和填充了数据
    for(const NoiseLimit &noiseLimit : std::as_const(noiseLimits)) {
        if(noiseLimit.roomType == arg1) {
            // 找到匹配的Room，可以根据需要保存或使用它
            matchedRoom = noiseLimit; // 使用局部变量保存找到的Room
            // 根据需要处理matchedRoom
            break; // 如果只期望有一个匹配，找到后即可退出循环
        }
    }

    ui->lineEdit_limit->setText(matchedRoom.noiseLimit);
}

void Dialog_add_room::on_checkBox_is_cal_stateChanged(int arg1)
{
    ui->comboBox_reference->setCurrentIndex(-1);
    if(arg1 == Qt::Unchecked) {
        ui->label_reference->show();
        ui->comboBox_reference->show();
        ui->lineEdit_duct_num->setReadOnly(true);
        ui->comboBox_place_type->setEnabled(false);
        ui->comboBox_room_type->setEnabled(false);
        ui->lineEdit_limit->setReadOnly(true);
        ui->lineEdit_duct_num->clear();
        ui->comboBox_place_type->setCurrentIndex(-1);
        ui->comboBox_room_type->setCurrentIndex(-1);
        ui->lineEdit_limit->clear();
    } else if (arg1 == Qt::Checked) {
        ui->label_reference->hide();
        ui->comboBox_reference->hide();
        ui->lineEdit_duct_num->setReadOnly(false);
        ui->comboBox_place_type->setEnabled(true);
        ui->comboBox_room_type->setEnabled(true);
        ui->lineEdit_limit->setReadOnly(false);
        ui->lineEdit_duct_num->clear();
        ui->comboBox_place_type->setCurrentIndex(-1);
        ui->comboBox_room_type->setCurrentIndex(-1);
        ui->lineEdit_limit->clear();
    }
}


void Dialog_add_room::on_comboBox_reference_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    RoomDBData data;
    if(isOuter) {
        data = RoomCalInfoManager::getInstance().getRoomDataByNumber(arg1);
    } else {
        data = RoomCalInfoManager::getInstance().getOuterDataByNumber(arg1);
    }
    ui->lineEdit_duct_num->setText(QString::number(data.ductNum));
    ui->comboBox_place_type->addItem(data.placeType);
    ui->comboBox_place_type->setCurrentText(data.placeType);
    ui->comboBox_room_type->addItem(data.roomType);
    ui->comboBox_room_type->setCurrentText(data.roomType);
    ui->lineEdit_limit->setText(QString::number(data.limit));
}

