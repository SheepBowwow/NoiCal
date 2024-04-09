#include "dialog_add_system.h"
#include "ui_dialog_add_system.h"
#include "Component/ComponentStructs.h"
#include "Component/ComponentManager.h"
#include "roomDefineForm/systemmanager.h"
#include <QMessageBox>

dialog_add_system::dialog_add_system(QWidget *parent) :
    InputBaseDialog(parent),
    ui(new Ui::dialog_add_system)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);  // 写在窗口类构造函数里，隐藏边框
    setTopWidget(ui->widget_top);
    ui->comboBox_type->setCurrentIndex(-1);
    ui->comboBox_type->lineEdit()->setPlaceholderText(QStringLiteral("请选择设备类型"));
    ui->comboBox_numOrModel->lineEdit()->setPlaceholderText(QStringLiteral("请选择设备编号"));
}


dialog_add_system::~dialog_add_system()
{
    delete ui;
}

void dialog_add_system::setvalues(int type_index, QString number, QString model)
{
    ui->comboBox_type->setCurrentIndex(type_index);
    ui->comboBox_numOrModel->setCurrentText(number);
    ui->lineEdit_model->setText(model);
}

QString dialog_add_system::getType()
{
    if(ui->comboBox_type->currentIndex()!=-1)
        return ui->comboBox_type->currentText();
    return QString("");
}

QString dialog_add_system::getNumber()
{
    if(getType() == noise_src_component::FANCOIL)
        return ui->lineEdit_model->text();

    if(ui->comboBox_numOrModel->currentIndex()!=-1)
        return ui->comboBox_numOrModel->currentText();
    return QString("");
}

QString dialog_add_system::getModel()
{
    if(getType() == noise_src_component::FANCOIL)
        return ui->comboBox_numOrModel->currentText();
    return ui->lineEdit_model->text();
}

QString dialog_add_system::getComponentUUID() const
{
    return componentUUID;
}

void dialog_add_system::on_pushButton_queding_clicked()
{
    if(ui->comboBox_numOrModel->currentIndex() == -1 || ui->comboBox_type->currentIndex() == -1)
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

    if(index == 0 || index == 1)
    {
        ui->comboBox_numOrModel->clear();
        ui->label_model->setText("型号");
        ui->label_num->setText("编号");
        ui->lineEdit_model->setStyleSheet("");
        ui->lineEdit_model->clear();
        ui->comboBox_numOrModel->lineEdit()->setPlaceholderText(QStringLiteral("请选择设备编号"));

        QList<QSharedPointer<ComponentBase>> components =
                ComponentManager::getInstance().getComponentsByType(component_type_name::AIRCONDITION);
        for(auto &component: components)
        {
            if(auto aircondition = dynamic_cast<Aircondition*>(component.data()))
            {
                if(index == 0)
                    ui->comboBox_numOrModel->addItem(aircondition->send_number);
                else if(index == 1 && aircondition->fan_counts == 2)
                    ui->comboBox_numOrModel->addItem(aircondition->exhaust_number);
            }
        }
        ui->comboBox_numOrModel->setCurrentIndex(-1);
    }
    else if(index == 2)
    {
        ui->comboBox_numOrModel->clear();
        ui->label_model->setText("型号");
        ui->label_num->setText("编号");
        ui->lineEdit_model->setStyleSheet("");
        ui->lineEdit_model->clear();
        ui->comboBox_numOrModel->lineEdit()->setPlaceholderText(QStringLiteral("请选择设备编号"));

        QList<QSharedPointer<ComponentBase>> components =
                ComponentManager::getInstance().getComponentsByType(component_type_name::FAN);
        for(auto &component: components)
        {
            if(auto fan = dynamic_cast<Fan*>(component.data()))
            {
                ui->comboBox_numOrModel->addItem(fan->number);
            }
        }
        ui->comboBox_numOrModel->setCurrentIndex(-1);
    }
    else if(index == 3)
    {
        ui->comboBox_numOrModel->clear();
        ui->label_model->setText("编号");
        ui->label_num->setText("型号");
        ui->lineEdit_model->setStyleSheet("QLineEdit{background-color: rgb(240, 240, 240); "
                                          "border: 1px solid #9C9C9C;}");
        ui->lineEdit_model->setReadOnly(true);
        ui->lineEdit_model->setText("-");
        ui->comboBox_numOrModel->lineEdit()->setPlaceholderText(QStringLiteral("请选择设备型号"));

        QList<QSharedPointer<ComponentBase>> components =
                ComponentManager::getInstance().getComponentsByType(component_type_name::FANCOIL);
        for(auto &component: components)
        {
            if(auto fanCoil = dynamic_cast<FanCoil*>(component.data()))
            {
                ui->comboBox_numOrModel->addItem(fanCoil->model);
            }
        }
        ui->comboBox_numOrModel->setCurrentIndex(-1);
    }
    ui->lineEdit_model->clear();
}


void dialog_add_system::on_comboBox_numOrModel_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_model->clear();
    if(ui->comboBox_type->currentIndex() == 0 || ui->comboBox_type->currentIndex() == 1)
    {
        QList<QSharedPointer<ComponentBase>> components =
                ComponentManager::getInstance().getComponentsByType(component_type_name::AIRCONDITION);
        for(auto &component: components)
        {
            if(auto aircondition = dynamic_cast<Aircondition*>(component.data()))
            {
                if(aircondition->send_number == arg1 || aircondition->exhaust_number == arg1)
                {
                    ui->lineEdit_model->setText(aircondition->model);
                    componentUUID = aircondition->UUID;
                }
            }
        }
    }
    if(ui->comboBox_type->currentIndex() == 2)
    {
        QList<QSharedPointer<ComponentBase>> components =
                ComponentManager::getInstance().getComponentsByType(component_type_name::FAN);
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
    if(ui->comboBox_type->currentIndex() == 3)
    {
        QList<QSharedPointer<ComponentBase>> components =
                ComponentManager::getInstance().getComponentsByType(component_type_name::FANCOIL);
        for(auto &component: components)
        {
            if(auto fanCoil = dynamic_cast<FanCoil*>(component.data()))
            {
                if(fanCoil->model == arg1)
                {
                    componentUUID = fanCoil->UUID;
                    ui->lineEdit_model->setText("-");
                }
            }
        }
    }
}
