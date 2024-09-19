#include "outer_before_widget.h"
#include "ui_outer_before_widget.h"

Outer_before_widget::Outer_before_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Outer_before_widget)
{
    ui->setupUi(this);

    ui->tableWidget->verticalHeader()->setVisible(false);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

Outer_before_widget::~Outer_before_widget()
{
    delete ui;
}

void Outer_before_widget::setInfo(QString MVZName, QString deck,QString outerNumer, QString noiseLimit, QString ductNum)
{
    ui->lineEdit_MVZName->setText(MVZName);
    ui->lineEdit_deck->setText(deck);
    ui->lineEdit_room_number->setText(outerNumer);
    ui->lineEdit_noise_limit->setText(noiseLimit);
    ui->lineEdit_duct_num->setText(ductNum);
}

void Outer_before_widget::setDuctNum(QString ductNum)
{
    ui->lineEdit_duct_num->setText(ductNum);
}
