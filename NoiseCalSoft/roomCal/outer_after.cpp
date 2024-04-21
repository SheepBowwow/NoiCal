#include "outer_after.h"
#include "ui_outer_after.h"

outer_after::outer_after(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::outer_after)
{
    ui->setupUi(this);
}

outer_after::~outer_after()
{
    delete ui;
}

void outer_after::setInfo(QString zhushuqu, QString deck, QString room_number, QString noise_limit, QString duct_num)
{
    ui->lineEdit_zhushuqu->setText(zhushuqu);
    ui->lineEdit_deck->setText(deck);
    ui->lineEdit_room_number->setText(room_number);
    ui->lineEdit_noise_limit->setText(noise_limit);
    ui->lineEdit_duct_num->setText(duct_num);
}
