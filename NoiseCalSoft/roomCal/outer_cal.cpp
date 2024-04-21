#include "outer_cal.h"
#include "ui_outer_cal.h"

outer_cal::outer_cal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::outer_cal)
{
    ui->setupUi(this);
}

outer_cal::~outer_cal()
{
    delete ui;
}
