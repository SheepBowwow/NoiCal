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

void room_cal_total::change_outer_cal()
{
    ui->label_cg1->setText("室外编号:");
    ui->label_cg2->setText("噪音源支管数量:");
    ui->label_cg3->setText("室外噪音总值dB(A):");
    // 设置列标题
    ui->tableWidget->setColumnCount(3);
    QStringList headerLabels;
    headerLabels<<"室外编号"<<"测试点距离(m)"<<"A级计权噪音dB(A)";
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
}

void room_cal_total::setInfo(QString zhushuqu, QString deck, QString room_number, QString noise_limit, QString duct_num)
{
    ui->lineEdit_zhushuqu->setText(zhushuqu);
    ui->lineEdit_deck->setText(deck);
    ui->lineEdit_room_number->setText(room_number);
    ui->lineEdit_noise_limit->setText(noise_limit);
    ui->lineEdit_duct_num->setText(duct_num);
}
