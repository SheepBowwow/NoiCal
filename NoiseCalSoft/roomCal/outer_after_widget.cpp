#include "outer_after_widget.h"
#include "ui_outer_after_widget.h"

Outer_after_widget::Outer_after_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Outer_after_widget)
{
    ui->setupUi(this);
}

Outer_after_widget::~Outer_after_widget()
{
    delete ui;
}

void Outer_after_widget::setInfo(QString MVZName, QString deck,QString outerNumer, QString noiseLimit, QString ductNum)
{
    ui->lineEdit_MVZName->setText(MVZName);
    ui->lineEdit_deck->setText(deck);
    ui->lineEdit_room_number->setText(outerNumer);
    ui->lineEdit_noise_limit->setText(noiseLimit);
    ui->lineEdit_duct_num->setText(ductNum);
}

void Outer_after_widget::setDuctNum(QString ductNum)
{
    ui->lineEdit_duct_num->setText(ductNum);
}

