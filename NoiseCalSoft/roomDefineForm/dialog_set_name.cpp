#include "dialog_set_name.h"
#include "ui_dialog_set_name.h"

Dialog_set_name::Dialog_set_name(QString title, QWidget *parent) :
    InputBaseDialog(parent),
    ui(new Ui::Dialog_set_name)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);  // 写在窗口类构造函数里，隐藏边框
    setTopWidget(ui->widget_top);
    ui->pushButton->setDefault(true);

    ui->label_title->setText("设置" + title);
    ui->label_title->adjustSize();
    ui->label->setText(title+":");
    ui->label->adjustSize();
}

Dialog_set_name::~Dialog_set_name()
{
    delete ui;
}


QString Dialog_set_name::getName()
{
    return ui->lineEdit->text();
}

void Dialog_set_name::on_close_clicked()
{
    close();
}

void Dialog_set_name::on_pushButton_clicked()
{
    if(ui->lineEdit->text().isEmpty()){
        QMessageBox::information(this,"提示","内容未填写完整");
        return;
    }
    accept();
    close();
}
