#include "room_cal_total.h"
#include "ui_room_cal_total.h"

room_cal_total::room_cal_total(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::room_cal_total)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 自适应大小
}

room_cal_total::~room_cal_total()
{
    delete ui;
}
