#include "table_noi_src.h"
#include "ui_table_noi_src.h"

Table_noi_src::Table_noi_src(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Table_noi_src)
{
    ui->setupUi(this);


}

Table_noi_src::~Table_noi_src()
{
    delete ui;
}
