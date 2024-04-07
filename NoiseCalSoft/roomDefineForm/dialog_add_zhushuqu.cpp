#include "dialog_add_zhushuqu.h"
#include "ui_dialog_add_zhushuqu.h"

Dialog_add_zhushuqu::Dialog_add_zhushuqu(QWidget *parent) :
    InputBaseDialog(parent),
    ui(new Ui::Dialog_add_zhushuqu)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);  // 写在窗口类构造函数里，隐藏边框
    setTopWidget(ui->widget_top);
    ui->pushButton->setDefault(true);
}

Dialog_add_zhushuqu::~Dialog_add_zhushuqu()
{
    delete ui;
}

void Dialog_add_zhushuqu::setlabeltext(QString text)
{
    ui->label_title->setText(text);
    ui->label_title->adjustSize();
    ui->label->setText(text+":");
    ui->label->adjustSize();
}


QString Dialog_add_zhushuqu::getname()
{
    return ui->lineEdit->text();
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
