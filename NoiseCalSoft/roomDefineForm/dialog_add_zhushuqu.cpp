#include "dialog_add_zhushuqu.h"
#include "ui_dialog_add_zhushuqu.h"

Dialog_add_zhushuqu::Dialog_add_zhushuqu(QWidget *parent) :
    InputBaseDialog(parent),
    ui(new Ui::Dialog_add_zhushuqu)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);  // 写在窗口类构造函数里，隐藏边框
    setTopWidget(ui->widget_top);
}

Dialog_add_zhushuqu::~Dialog_add_zhushuqu()
{
    delete ui;
}

void Dialog_add_zhushuqu::setlabeltext(int i)
{
    if(i==0){
        ui->label->setText("主竖区：");
        ui->label_title->setText("主竖区：");
    }
    if(i==1){
        ui->label->setText("甲板：");
        ui->label_title->setText("甲板：");
    }
}


QString Dialog_add_zhushuqu::getname()
{
    return ui->lineEdit->text();
}

void *Dialog_add_zhushuqu::getNoi()
{

}

void Dialog_add_zhushuqu::on_close_clicked()
{
    close();
}

void Dialog_add_zhushuqu::on_pushButton_clicked()
{
    emit accept();
    close();
}
