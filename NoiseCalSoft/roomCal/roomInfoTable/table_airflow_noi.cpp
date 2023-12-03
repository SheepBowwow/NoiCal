#include "table_airflow_noi.h"
#include "ui_table_airflow_noi.h"

Table_airFlow_noi::Table_airFlow_noi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Table_airFlow_noi)
{
    ui->setupUi(this);
}

Table_airFlow_noi::~Table_airFlow_noi()
{
    delete ui;
}
