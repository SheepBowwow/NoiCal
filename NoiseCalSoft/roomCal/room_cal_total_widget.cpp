#include "room_cal_total_widget.h"
#include "ui_room_cal_total_widget.h"

Room_cal_total_widget::Room_cal_total_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Room_cal_total_widget)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 自适应大小
}

Room_cal_total_widget::~Room_cal_total_widget()
{
    delete ui;
}

void Room_cal_total_widget::change_outer_cal()
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

void Room_cal_total_widget::setInfo(QString MVZName, QString deck,QString numer, QString noiseLimit, QString ductNum)
{
    ui->lineEdit_MVZName->setText(MVZName);
    ui->lineEdit_deck->setText(deck);
    ui->lineEdit_room_number->setText(numer);
    ui->lineEdit_noise_limit->setText(noiseLimit);
    ui->lineEdit_duct_num->setText(ductNum);
}

void Room_cal_total_widget::setDuctNum(QString ductNum)
{
    ui->lineEdit_duct_num->setText(ductNum);
}

void Room_cal_total_widget::setMVZName(QString MVZName)
{
    ui->lineEdit_MVZName->setText(MVZName);
}
