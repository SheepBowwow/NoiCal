#include "form_system_list.h"
#include "ui_form_system_list.h"

Form_system_list::Form_system_list(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_system_list)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 自适应大小
}

Form_system_list::~Form_system_list()
{
    delete ui;
}
