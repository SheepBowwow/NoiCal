#include "table_atten_noi.h"
#include "ui_table_atten_noi.h"

Table_atten_noi::Table_atten_noi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Table_atten_noi)
{
    ui->setupUi(this);
}

Table_atten_noi::~Table_atten_noi()
{
    delete ui;
}
