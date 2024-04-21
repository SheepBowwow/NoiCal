#include "outer_before.h"
#include "ui_outer_before.h"

outer_before::outer_before(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::outer_before)
{
    ui->setupUi(this);

    ui->tableWidget->verticalHeader()->setVisible(false);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

outer_before::~outer_before()
{
    delete ui;
}

void outer_before::setInfo(QString zhushuqu, QString deck, QString room_number, QString noise_limit, QString duct_num)
{
    ui->lineEdit_zhushuqu->setText(zhushuqu);
    ui->lineEdit_deck->setText(deck);
    ui->lineEdit_room_number->setText(room_number);
    ui->lineEdit_noise_limit->setText(noise_limit);
    ui->lineEdit_duct_num->setText(duct_num);
}
