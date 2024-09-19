#include "dialog_add_system.h"
#include "ui_dialog_add_system.h"
#include "Component/ComponentStructs.h"
#include "Component/ComponentManager.h"
#include "roomDefineForm/roomcalinfomanager.h"
#include <QMessageBox>

dialog_add_system::dialog_add_system(QWidget *parent) :
    InputBaseDialog(parent),
    ui(new Ui::dialog_add_system)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);  // 写在窗口类构造函数里，隐藏边框
    setTopWidget(ui->widget_top);
    ui->comboBox_type->setCurrentIndex(-1);
    ui->comboBox_model->setVisible(false);
}


dialog_add_system::~dialog_add_system()
{
    delete ui;
}

void dialog_add_system::setvalues(QString type, QString number, QString model)
{
    ui->comboBox_type->setCurrentText(type);
    ui->comboBox_num->setCurrentText(number);
    ui->lineEdit_model->setText(model);
    ui->comboBox_model->setCurrentText(model);
}

QString dialog_add_system::getType()
{
    if(ui->comboBox_type->currentIndex() != -1)
        return ui->comboBox_type->currentText();
    return QString("");
}

QString dialog_add_system::getNumber()
{
    if(getType() == noise_src_component::FANCOIL)
        return "-";

    if(ui->comboBox_num->currentIndex() != -1)
        return ui->comboBox_num->currentText();
    return QString("");
}

QString dialog_add_system::getModel()
{
    if(getType() == noise_src_component::FANCOIL)
        return ui->comboBox_model->currentText();
    return ui->lineEdit_model->text();
}

QString dialog_add_system::getComponentUUID() const
{
    return componentUUID;
}

void dialog_add_system::on_pushButton_confirm_clicked()
{
    if( ui->comboBox_type->currentIndex() == -1
        || (ui->comboBox_type->currentIndex() != 1 && ui->comboBox_num->currentIndex() == -1)
        || (ui->comboBox_type->currentIndex() == 1 && ui->comboBox_model->currentIndex() == -1))
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

    accept();
    close();
}


void dialog_add_system::on_close_clicked()
{
    close();
}

void dialog_add_system::on_comboBox_type_currentIndexChanged(int index)
{
    if(index == 0)
    {
        ui->comboBox_num->blockSignals(true);
        ui->comboBox_num->clear();
        ui->lineEdit_model->clear();
        ui->comboBox_model->clear();
        ui->label_num->setVisible(true);
        ui->comboBox_num->setVisible(true);
        ui->lineEdit_model->setVisible(true);
        ui->comboBox_model->setVisible(false);

        QList<QSharedPointer<ComponentBase>> components =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::FAN);
        for(auto &component: components)
        {
            if(auto fan = dynamic_cast<Fan*>(component.data()))
            {
                ui->comboBox_num->addItem(fan->number);
            }
        }
        ui->comboBox_num->setCurrentIndex(-1);
        ui->comboBox_num->blockSignals(false);
    }
    else if(index == 1)
    {
        ui->comboBox_model->blockSignals(true);
        ui->comboBox_num->clear();
        ui->lineEdit_model->clear();
        ui->comboBox_model->clear();
        ui->label_num->setVisible(false);
        ui->comboBox_num->setVisible(false);
        ui->lineEdit_model->setVisible(false);
        ui->comboBox_model->setVisible(true);

        QList<QSharedPointer<ComponentBase>> components =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::FANCOIL);
        for(auto &component: components)
        {
            if(auto fanCoil = dynamic_cast<FanCoil*>(component.data()))
            {
                ui->comboBox_model->addItem(fanCoil->model);
            }
        }
        ui->comboBox_model->setCurrentIndex(-1);
        ui->comboBox_model->blockSignals(false);
    }
    else if(index == 2 || index == 3)
    {
        ui->comboBox_num->blockSignals(true);
        ui->comboBox_num->clear();
        ui->lineEdit_model->clear();
        ui->comboBox_model->clear();
        ui->label_num->setVisible(true);
        ui->comboBox_num->setVisible(true);
        ui->lineEdit_model->setVisible(true);
        ui->comboBox_model->setVisible(false);

        QList<QSharedPointer<ComponentBase>> components =
                ComponentManager::getInstance().getComponentsByType(false, component_type_name::AIRCONDITION);
        for(auto &component: components)
        {
            if(auto aircondition = dynamic_cast<Aircondition*>(component.data()))
            {
                QString item = aircondition->send_number;
                if(index == 2 && aircondition->fan_counts == 1)
                {
                    ui->comboBox_num->addItem(item);
                }
                else if(index == 3 && aircondition->fan_counts == 2)
                {
                    item += (" & " + aircondition->exhaust_number);
                    ui->comboBox_num->addItem(item);
                }
            }
        }
        ui->comboBox_num->setCurrentIndex(-1);
        ui->comboBox_num->blockSignals(false);
    }
}


void dialog_add_system::on_comboBox_num_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_model->clear();
    if(ui->comboBox_type->currentIndex() == 0)
    {
        QList<QSharedPointer<ComponentBase>> components =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::FAN);
        for(auto &component: components)
        {
            if(auto fan = dynamic_cast<Fan*>(component.data()))
            {
                if(fan->number == arg1)
                {
                    ui->lineEdit_model->setText(fan->model);
                    componentUUID = fan->UUID;
                }
            }
        }
    }
    else if(ui->comboBox_type->currentIndex() == 2 || ui->comboBox_type->currentIndex() == 3)
    {
        QList<QSharedPointer<ComponentBase>> components =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::AIRCONDITION);
        for(auto &component: components)
        {
            if(auto aircondition = dynamic_cast<Aircondition*>(component.data()))
            {
                if(aircondition->send_number == arg1 || (aircondition->send_number + " & " + aircondition->exhaust_number) == arg1)
                {
                    ui->lineEdit_model->setText(aircondition->model);
                    componentUUID = aircondition->UUID;
                }
            }
        }
    }
}

void dialog_add_system::on_comboBox_model_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_type->currentIndex() == 1)
    {
        QList<QSharedPointer<ComponentBase>> components =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::FANCOIL);
        for(auto &component: components)
        {
            if(auto fanCoil = dynamic_cast<FanCoil*>(component.data()))
            {
                if(fanCoil->model == arg1)
                {
                    componentUUID = fanCoil->UUID;
                }
            }
        }
    }
}

