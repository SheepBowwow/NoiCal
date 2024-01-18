#include "dialog_addroom.h"
#include "ui_dialog_addroom.h"

Dialog_addroom::Dialog_addroom(QWidget *parent) :
    InputBaseDialog(parent),
    ui(new Ui::Dialog_addroom)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);  // 写在窗口类构造函数里，隐藏边框
    setTopWidget(ui->widget_top);
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

void *Dialog_addroom::getNoi()
{

}

void Dialog_addroom::on_pushButton_clicked()
{
    emit accept();
    QString name=ui->lineEdit_roomid->text();
    int num=ui->lineEdit_pipe->text().toInt();
    emit dialogsent(name,num);
    close();
}

void Dialog_addroom::on_close_clicked()
{
    close();
}
