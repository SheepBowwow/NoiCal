#include "form_outer_before.h"
#include "ui_form_outer_before.h"

Form_outer_before::Form_outer_before(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_outer_before)
{
    ui->setupUi(this);
}

Form_outer_before::~Form_outer_before()
{
    delete ui;
}
