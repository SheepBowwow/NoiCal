#include "dialog_addroom.h"
#include "ui_dialog_addroom.h"
#include "globle_var.h"

Dialog_addroom::Dialog_addroom(QWidget *parent) :
    InputBaseDialog(parent),
    ui(new Ui::Dialog_addroom)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);  // 写在窗口类构造函数里，隐藏边框
    setTopWidget(ui->widget_top);

    for(auto& room : rooms)
    {
        ui->comboBox->addItem(room.name);
    }
}

Dialog_addroom::~Dialog_addroom()
{
    delete ui;
}

void Dialog_addroom::setvalues(QString s1, QString s2, QString s3, QString num, QString s4, QString s5)
{
    ui->lineEdit_roomid->setText(s1);
    ui->lineEdit_roomname->setText(s2);
    ui->lineEdit_jiaban->setText(s3);
    ui->lineEdit_pipe->setText(num);
    ui->lineEdit_limit->setText(s4);
    ui->lineEdit_roomcalclass->setText(s5);
}

QString Dialog_addroom::getroomid()
{
    return ui->lineEdit_roomid->text();
}

QString Dialog_addroom::getroomname()
{
    return ui->lineEdit_roomname->text();
}

QString Dialog_addroom::getjiaban()
{
    return ui->lineEdit_jiaban->text();
}

int Dialog_addroom::getmainpipe()
{
    return ui->lineEdit_pipe->text().toInt();
}

QString Dialog_addroom::getroomclass()
{
    return " ";
}

QString Dialog_addroom::getlimit()
{
    return ui->lineEdit_limit->text();
}

QString Dialog_addroom::getroomcalclass()
{
    return ui->lineEdit_roomcalclass->text();
}

void Dialog_addroom::on_pushButton_clicked()
{
    QString name=ui->lineEdit_roomid->text();
    int num=ui->lineEdit_pipe->text().toInt();
    QString jiaban = ui->lineEdit_jiaban->text();
    QString limit = ui->lineEdit_limit->text();

    if(ui->lineEdit_pipe->text().isEmpty()||
            ui->lineEdit_limit->text().isEmpty()||
            ui->lineEdit_jiaban->text().isEmpty()||
            ui->lineEdit_roomid->text().isEmpty()||
            ui->lineEdit_roomname->text().isEmpty()||
            ui->lineEdit_roomcalclass->text().isEmpty())
    {
        QMessageBox::information(this,"提示","内容未填写完整");
        return;
    }

    emit dialogsent(name,num, jiaban, limit);
    close();
}

void Dialog_addroom::on_close_clicked()
{
    close();
}

//房间类型选择后更改下面两项
void Dialog_addroom::on_comboBox_currentTextChanged(const QString &arg1)
{
    Room matchedRoom;
    // 假设rooms是QList<Room>，且已经在类中定义和填充了数据
    for(const Room &room : std::as_const(rooms)) {
        if(room.name == arg1) {
            // 找到匹配的Room，可以根据需要保存或使用它
            matchedRoom = room; // 使用局部变量保存找到的Room
            // 根据需要处理matchedRoom
            break; // 如果只期望有一个匹配，找到后即可退出循环
        }
    }

    ui->lineEdit_limit->setText(matchedRoom.noise);
    ui->lineEdit_roomcalclass->setText(matchedRoom.type);
}

