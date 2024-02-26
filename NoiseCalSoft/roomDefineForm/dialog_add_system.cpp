#include "dialog_add_system.h"
#include "ui_dialog_add_system.h"
#include "globle_var.h"
#include <QMessageBox>

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

void dialog_add_system::setvalues(int type_index, QString number, QString model)
{
    ui->comboBox_type->setCurrentIndex(type_index);
    ui->comboBox_number->setCurrentText(number);
    ui->lineEdit_model->setText(model);
}

QString dialog_add_system::getType()
{
    if(ui->comboBox_type->currentIndex()!=0)
        return ui->comboBox_type->currentText();
    return QString("");
}

QString dialog_add_system::getNumber()
{
    if(ui->comboBox_number->currentIndex()!=0)
        return ui->comboBox_number->currentText();
    return QString("");
}

QString dialog_add_system::getModel()
{
    return ui->lineEdit_model->text();
}


void dialog_add_system::on_pushButton_queding_clicked()
{
    if(ui->comboBox_number->currentIndex() == 0 || ui->comboBox_type->currentIndex() == 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("警告"); // 设置对话框的标题
        msgBox.setText("输入有误");    // 设置对话框的文本
        msgBox.setIcon(QMessageBox::Warning); // 设置对话框的图标为警告图标

        // 添加自定义文本的按钮，并设置为对话框的接受按钮
        QPushButton *confirmButton = msgBox.addButton("确认", QMessageBox::AcceptRole);
         // 显示对话框

        msgBox.exec();
        close();
        return;
    }

    emit accept();
    close();
}


void dialog_add_system::on_close_clicked()
{
    close();
}

void dialog_add_system::on_comboBox_type_currentTextChanged(const QString &arg1)
{
    if(arg1 == "空调器")
    {
        ui->comboBox_number->clear();
        ui->comboBox_number->addItem("请选择设备编号");
        QVector<QString> airconditionNumbers = componentManager.getAirconditionNumbers();
        for(const QString &number: airconditionNumbers)
        {
            ui->comboBox_number->addItem(number);
        }
    }
    else if(arg1 == "独立排风机")
    {
        ui->comboBox_number->clear();
        ui->comboBox_number->addItem("请选择设备编号");
        QVector<QString> fanNumbers = componentManager.getFanNumbers();
        for(const QString &number : fanNumbers)
        {
            ui->comboBox_number->addItem(number);
        }
    }
    else if(arg1 == "公共区域风机盘管")
    {
        ui->comboBox_number->clear();
        ui->comboBox_number->addItem("请选择设备编号");
        QVector<QString> fanCoilNumbers = componentManager.getFanCoilNumbers();
        for(const QString &number: fanCoilNumbers)
        {
            ui->comboBox_number->addItem(number);
        }
    }
}


void dialog_add_system::on_comboBox_number_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_type->currentText() == "公共区域风机盘管" && ui->comboBox_number->currentText() != "请选择设备编号")
    {
        ui->lineEdit_model->setText(ui->comboBox_number->currentText());
        return;
    }

    QString model = componentManager.getModelByNumber(arg1);
    ui->lineEdit_model->setText(model);
}

