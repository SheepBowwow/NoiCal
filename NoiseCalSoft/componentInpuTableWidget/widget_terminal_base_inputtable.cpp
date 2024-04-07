#include "widget_terminal_base_inputtable.h"
#include "ui_widget_terminal_base_inputtable.h"

Widget_terminal_base_inputTable::Widget_terminal_base_inputTable(QWidget *parent) :
    Widget_base_inputTable(parent,false),
    ui(new Ui::Widget_terminal_base_inputTable)
{
    ui->setupUi(this);
}

Widget_terminal_base_inputTable::~Widget_terminal_base_inputTable()
{
    delete ui;
}

void Widget_terminal_base_inputTable::setTitle(const QString &title)
{
    ui->label_title->setText(title);
    ui->label_title->adjustSize();
}

void Widget_terminal_base_inputTable::setCurrentTable(const int &index)
{
    ui->stackedWidget->setCurrentIndex(index);
}
