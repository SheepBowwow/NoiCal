#include "widget_aircondition_inputtable.h"
#include "ui_widget_aircondition_inputtable.h"

Widget_aircondition_inputTable::Widget_aircondition_inputTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_aircondition_inputTable)
{
    ui->setupUi(this);
}

Widget_aircondition_inputTable::~Widget_aircondition_inputTable()
{
    delete ui;
}
