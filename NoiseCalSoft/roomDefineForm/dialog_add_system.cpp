#include "dialog_add_system.h"
#include "ui_dialog_add_system.h"

dialog_add_system::dialog_add_system(QWidget *parent) :
    InputBaseDialog(parent),
    ui(new Ui::dialog_add_system)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);  // 写在窗口类构造函数里，隐藏边框
    setTopWidget(ui->widget_top);
}

dialog_add_system::~dialog_add_system()
{
    delete ui;
}

void *dialog_add_system::getNoi()
{

}

void dialog_add_system::setvalues(int n, QString bh, QString xh)
{
    ui->comboBox_lx->setCurrentIndex(n);
    ui->comboBox_bh->setCurrentText(bh);
    ui->comboBox_xh->setCurrentText(xh);
}

QString dialog_add_system::getsyslx()
{
    if(ui->comboBox_lx->currentIndex()!=0)
        return ui->comboBox_lx->currentText();
}

QString dialog_add_system::getsysbh()
{
    if(ui->comboBox_bh->currentIndex()!=0)
        return ui->comboBox_bh->currentText();
}

QString dialog_add_system::getsysxh()
{
    if(ui->comboBox_xh->currentIndex()!=0)
        return ui->comboBox_xh->currentText();
}


void dialog_add_system::on_pushButton_queding_clicked()
{
    emit accept();
    close();
}


void dialog_add_system::on_close_clicked()
{
    close();
}
