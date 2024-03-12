#include "inputDialog/dialog_silencer.h"
#include "ui_dialog_silencer.h"
#include "databasemanager.h"
#include "global_constant.h"
#include "globle_var.h"
#include <QDir>
#include <cmath>
#include <QSet>

Dialog_silencer::Dialog_silencer(QString type, QWidget *parent, int editRow,  const Silencer_atten& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_silencer)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件

    if(type == "圆形消音器")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_cs);
    }
    else if(type == "矩形消音器")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_rs);
    }
    else if(type == "圆形消音弯头")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_cse);
    }
    else if(type == "矩形消音弯头")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_rse);
    }

    DBManager->executeSqlQuery(roundSilencerModel,"SELECT " + QString(SILENCER_MODEL) + " FROM " + SILENCER_TABLE + " WHERE silencer_type = '圆形消声器'");
    DBManager->executeSqlQuery(rectSilencerModel,"SELECT " + QString(SILENCER_MODEL) + " FROM " + SILENCER_TABLE + " WHERE silencer_type = '方形消音器'");
    DBManager->executeSqlQuery(pipeSilencerModel,"SELECT " + QString(SILENCER_MODEL) + " FROM " + SILENCER_TABLE + " WHERE silencer_type = '消音管'");

    setModelComboBoxItems();

    if(editRow != -1)
    {

        ui->lineEdit_brand->setText(data.brand);
        ui->comboBox_model->setCurrentText(data.model);
        ui->lineEdit_63->setText(data.noi_63);
        ui->lineEdit_125->setText(data.noi_125);
        ui->lineEdit_250->setText(data.noi_250);
        ui->lineEdit_500->setText(data.noi_500);
        ui->lineEdit_1k->setText(data.noi_1k);
        ui->lineEdit_2k->setText(data.noi_2k);
        ui->lineEdit_4k->setText(data.noi_4k);
        ui->lineEdit_8k->setText(data.noi_8k);
    }


    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_silencer::~Dialog_silencer()
{
    delete ui;
}

void* Dialog_silencer::getNoi()
{
    return noi == nullptr ? nullptr : noi;
}


//点击确认键
void Dialog_silencer::on_pushButton_confirm_clicked()
{
    this->noi = new Silencer_atten;

    // 获取对应行的数据，将界面上的数据保存到对应行中
    noi->brand = ui->lineEdit_brand->text();
    noi->model = ui->comboBox_model->currentText();
    noi->noi_63 = ui->lineEdit_63->text();
    noi->noi_125 = ui->lineEdit_125->text();
    noi->noi_250 = ui->lineEdit_250->text();
    noi->noi_500 = ui->lineEdit_500->text();
    noi->noi_1k = ui->lineEdit_1k->text();
    noi->noi_2k = ui->lineEdit_2k->text();
    noi->noi_4k = ui->lineEdit_4k->text();
    noi->noi_8k = ui->lineEdit_8k->text();
    noi->identifier = noi->model;

    if(ui->stackedWidget_title->currentWidget() == ui->page_cs)
    {
        noi->type = "圆形消音器";
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_rs)
    {
        noi->type = "矩形消音器";
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_cse)
    {
        noi->type = "圆形消音弯头";
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_rse)
    {
        noi->type = "矩形消音弯头";
    }

    this->accept(); // 关闭对话框
}

void Dialog_silencer::on_close_clicked()
{
    this->close();
}

void Dialog_silencer::setModelComboBoxItems()
{
    ui->comboBox_model->clear();
    if(ui->stackedWidget_title->currentWidget() == ui->page_cs)
    {
        /******设置combobox******/
        for(int i = 0; i < roundSilencerModel.size(); i++)
        {
            QString s = roundSilencerModel[i];
                ui->comboBox_model->addItem(s);
        }
        /******设置combobox******/
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_rs)
    {
        /******设置combobox******/
        for(int i = 0; i < rectSilencerModel.size(); i++)
        {
            QString s = rectSilencerModel[i];
            ui->comboBox_model->addItem(s);
        }
        /******设置combobox******/
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_cse)
    {
        /******设置combobox******/
        for(int i = 0; i < pipeSilencerModel.size(); i++)
        {
            QString s = pipeSilencerModel[i];
            ui->comboBox_model->addItem(s);
        }
        /******设置combobox******/
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_rse)
    {
        /******设置combobox******/
        for(int i = 0; i < pipeSilencerModel.size(); i++)
        {
            QString s = pipeSilencerModel[i];
            ui->comboBox_model->addItem(s);
        }
        /******设置combobox******/
    }
}

