#include "dialog_addouter.h"
#include "ui_dialog_addouter.h"

Dialog_addouter::Dialog_addouter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_addouter)
{
    ui->setupUi(this);
}

Dialog_addouter::~Dialog_addouter()
{
    delete ui;
}
