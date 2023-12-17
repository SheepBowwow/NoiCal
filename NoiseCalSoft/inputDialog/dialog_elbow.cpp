#include "inputDialog/dialog_elbow.h"
#include "ui_dialog_elbow.h"
#include <cmath>

Dialog_elbow::Dialog_elbow(QWidget *parent, int editRow,  const Elbow_atten& data) :
    InputBaseDialog(parent),
    editRow(editRow),
    ui(new Ui::Dialog_elbow)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件
    ui->radioButton_formula->setChecked(true);
    ui->radioButton_known->setChecked(false);

    ui->radioButton_circle->setChecked(true);
    ui->radioButton_rect->setChecked(false);

    ui->stackedWidget_input->setCurrentWidget(ui->page_circle_formula);
    ui->stackedWidget_descri->setCurrentWidget(ui->page_circle_descri);

    if(editRow != -1)
    {
        QString diameter = "";
        QString width = "";
        QString isLined = "";
        if(data.type == "圆形")
        {
            for(int i = 0; i < data.model.length(); i++)
            {
                if(data.model[i] >= '0' && data.model[i] <= '9')
                {
                    diameter += data.model[i];
                }
            }
        }
        else if(data.type == "方形")
        {
            isLined = "无内衬";
            for(int i = 0; i < data.model.length(); i++)
            {
                if(data.model[i] == 'L')
                {
                    isLined = "有内衬";
                }
                if(data.model[i] >= '0' && data.model[i] <= '9')
                {
                    width += data.model[i];
                }
            }
        }
        ui->lineEdit_63->setText(data.noi_63);
        ui->lineEdit_125->setText(data.noi_125);
        ui->lineEdit_250->setText(data.noi_250);
        ui->lineEdit_500->setText(data.noi_500);
        ui->lineEdit_1k->setText(data.noi_1k);
        ui->lineEdit_2k->setText(data.noi_2k);
        ui->lineEdit_4k->setText(data.noi_4k);
        ui->lineEdit_8k->setText(data.noi_8k);
        if(data.getMode == "厂家")
        {
            if(data.type == "圆形")
            {
                ui->radioButton_circle->setChecked(true);
                ui->radioButton_rect->setChecked(false);
            }
            else if(data.type == "方形")
            {
                ui->radioButton_circle->setChecked(false);
                ui->radioButton_rect->setChecked(true);
            }
            ui->radioButton_formula->setChecked(false);
            ui->radioButton_known->setChecked(true);
            ui->stackedWidget_input->setCurrentWidget(ui->page_known);
            ui->stackedWidget_descri->setCurrentWidget(ui->page_white);
            ui->lineEdit_model->setText(data.model);

            ui->lineEdit_63->setReadOnly(false);
            ui->lineEdit_125->setReadOnly(false);
            ui->lineEdit_250->setReadOnly(false);
            ui->lineEdit_500->setReadOnly(false);
            ui->lineEdit_1k->setReadOnly(false);
            ui->lineEdit_2k->setReadOnly(false);
            ui->lineEdit_4k->setReadOnly(false);
            ui->lineEdit_8k->setReadOnly(false);
        }
        else if(data.getMode == "公式")
        {
            ui->radioButton_formula->setChecked(true);
            ui->radioButton_known->setChecked(false);
            if(data.type == "圆形")
            {
                ui->radioButton_circle->setChecked(true);
                ui->radioButton_rect->setChecked(false);
                ui->stackedWidget_input->setCurrentWidget(ui->page_circle_formula);
                ui->stackedWidget_descri->setCurrentWidget(ui->page_circle_descri);
                ui->lineEdit_diameter->setText(diameter);
            }
            else if(data.type == "方形")
            {
                if(isLined == "有内衬")
                {
                    ui->radioButton_lined->setChecked(true);
                    ui->radioButton_unlined->setChecked(false);
                }
                else if(isLined == "无内衬")
                {
                    ui->radioButton_lined->setChecked(false);
                    ui->radioButton_unlined->setChecked(true);
                }
                ui->radioButton_circle->setChecked(false);
                ui->radioButton_rect->setChecked(true);
                ui->stackedWidget_input->setCurrentWidget(ui->page_rect_formula);
                ui->stackedWidget_descri->setCurrentWidget(ui->page_rect_descri);
                ui->lineEdit_width->setText(width);
            }
        }
    }

    //连接lineEdit信号与槽
    connect(ui->lineEdit_diameter, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->lineEdit_width, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->radioButton_lined, SIGNAL(clicked()), this, SLOT(calNoise()));
    connect(ui->radioButton_unlined, SIGNAL(clicked()), this, SLOT(calNoise()));

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);

}

Dialog_elbow::~Dialog_elbow()
{
    delete ui;
}

void Dialog_elbow::on_close_clicked()
{
    this->close();
}

void* Dialog_elbow::getNoi()
{
    return noi == nullptr ? nullptr : noi;
}

//计算总值 槽函数实现
void Dialog_elbow::calNoise() {
    double s1 = 0,s2 = 0,noise = 0;
    double f[] = {0.063, 0.125, 0.250, 0.500, 1, 2, 4, 8};
    QLineEdit* groupNoiseEdit[] = { ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500, ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k};
    if(ui->radioButton_circle->isChecked())
    {
        if(ui->lineEdit_diameter->text().isEmpty())
        {
            return;
        }
        for(int i = 0; i < 8; i++)
        {
            double fw = f[i] * ui->lineEdit_diameter->text().toDouble();
            double result = 0;
            if(fw < 48)
            {
                result = 0;
            }
            else if(fw >= 48 && fw < 96)
            {
                result = 1;
            }
            else if(fw >= 96 && fw < 190)
            {
                result = 2;
            }
            else if(fw >= 190)
            {
                result = 3;
            }
            groupNoiseEdit[i]->setText(QString::number(result,'f',1));
        }
    }
    else if(ui->radioButton_rect->isChecked())
    {
        if(ui->lineEdit_width->text().isEmpty())
        {
            return;
        }
        for(int i = 0; i < 8; i++)
        {
            double fw = f[i] * ui->lineEdit_width->text().toDouble();
            double result = 0;
            if(ui->radioButton_lined->isChecked())
            {
                if(fw < 48)
                {
                    result = 0;
                }
                else if(fw >= 48 && fw < 96)
                {
                    result = 1;
                }
                else if(fw >= 96 && fw < 190)
                {
                    result = 6;
                }
                else if(fw >= 190 && fw < 380)
                {
                    result = 11;
                }
                else if(fw >= 380 && fw < 760)
                {
                    result = 10;
                }
                else if(fw > 760)
                {
                    result = 10;
                }
            }
            else if(ui->radioButton_unlined->isChecked())
            {
                if(fw < 48)
                {
                    result = 0;
                }
                else if(fw >= 48 && fw < 96)
                {
                    result = 1;
                }
                else if(fw >= 96 && fw < 190)
                {
                    result = 5;
                }
                else if(fw >= 190 && fw < 380)
                {
                    result = 8;
                }
                else if(fw >= 380 && fw < 760)
                {
                    result = 4;
                }
                else if(fw > 760)
                {
                    result = 3;
                }
            }
            groupNoiseEdit[i]->setText(QString::number(result,'f',1));
        }
    }
}

void Dialog_elbow::on_pushButton_confirm_clicked()
{
    this->noi = new Elbow_atten;

    // 获取对应行的数据，将界面上的数据保存到对应行中
    noi->noi_63 = ui->lineEdit_63->text();
    noi->noi_125 = ui->lineEdit_125->text();
    noi->noi_250 = ui->lineEdit_250->text();
    noi->noi_500 = ui->lineEdit_500->text();
    noi->noi_1k = ui->lineEdit_1k->text();
    noi->noi_2k = ui->lineEdit_2k->text();
    noi->noi_4k = ui->lineEdit_4k->text();
    noi->noi_8k = ui->lineEdit_8k->text();
    if(ui->radioButton_circle->isChecked())
    {
        noi->type = "圆形";
    }
    else if(ui->radioButton_rect->isChecked())
    {
        noi->type = "方形";
    }
    if(ui->radioButton_formula->isChecked())
    {
        noi->getMode = "公式";
        if(noi->type == "圆形")
        {
            noi->model = "ED" + ui->lineEdit_diameter->text();
            noi->diameter = ui->lineEdit_diameter->text();
        }
        else if(noi->type == "方形")
        {
            noi->model = "EW" + ui->lineEdit_width->text();
            noi->isLined = "无内衬";
            if(ui->radioButton_lined->isChecked())
            {
                noi->model = "EWL" + ui->lineEdit_width->text();
                noi->isLined = "有内衬";
            }
            noi->width = ui->lineEdit_width->text();
        }
    }
    else if(ui->radioButton_known->isChecked())
    {
        noi->getMode = "厂家";
        noi->model = ui->lineEdit_model->text();
    }

    noi->identifier = noi->model;

    this->accept(); // 关闭对话框
}


//通过厂家信息
void Dialog_elbow::on_radioButton_known_clicked()
{
    if(ui->radioButton_circle->isChecked())
    {
        ui->lineEdit_diameter->clear();
    }
    else if(ui->radioButton_rect->isChecked())
    {
        ui->lineEdit_width->clear();
    }
    ui->lineEdit_63->clear();
    ui->lineEdit_125->clear();
    ui->lineEdit_250->clear();
    ui->lineEdit_500->clear();
    ui->lineEdit_1k->clear();
    ui->lineEdit_2k->clear();
    ui->lineEdit_4k->clear();
    ui->lineEdit_8k->clear();

    ui->stackedWidget_input->setCurrentWidget(ui->page_known);
    ui->stackedWidget_descri->setCurrentWidget(ui->page_white);

    ui->lineEdit_63->setReadOnly(false);
    ui->lineEdit_125->setReadOnly(false);
    ui->lineEdit_250->setReadOnly(false);
    ui->lineEdit_500->setReadOnly(false);
    ui->lineEdit_1k->setReadOnly(false);
    ui->lineEdit_2k->setReadOnly(false);
    ui->lineEdit_4k->setReadOnly(false);
    ui->lineEdit_8k->setReadOnly(false);
}

//通过经验公式
void Dialog_elbow::on_radioButton_formula_clicked()
{
    ui->lineEdit_model->clear();
    ui->lineEdit_63->clear();
    ui->lineEdit_125->clear();
    ui->lineEdit_250->clear();
    ui->lineEdit_500->clear();
    ui->lineEdit_1k->clear();
    ui->lineEdit_2k->clear();
    ui->lineEdit_4k->clear();
    ui->lineEdit_8k->clear();

    if(ui->radioButton_circle->isChecked())
    {
        ui->stackedWidget_input->setCurrentWidget(ui->page_circle_formula);
        ui->stackedWidget_descri->setCurrentWidget(ui->page_circle_descri);
    }
    else if(ui->radioButton_rect->isChecked())
    {
        ui->stackedWidget_input->setCurrentWidget(ui->page_rect_formula);
        ui->stackedWidget_descri->setCurrentWidget(ui->page_rect_descri);
    }


    ui->lineEdit_63->setReadOnly(true);
    ui->lineEdit_125->setReadOnly(true);
    ui->lineEdit_250->setReadOnly(true);
    ui->lineEdit_500->setReadOnly(true);
    ui->lineEdit_1k->setReadOnly(true);
    ui->lineEdit_2k->setReadOnly(true);
    ui->lineEdit_4k->setReadOnly(true);
    ui->lineEdit_8k->setReadOnly(true);
}


void Dialog_elbow::on_radioButton_circle_clicked()
{
    ui->lineEdit_63->clear();
    ui->lineEdit_125->clear();
    ui->lineEdit_250->clear();
    ui->lineEdit_500->clear();
    ui->lineEdit_1k->clear();
    ui->lineEdit_2k->clear();
    ui->lineEdit_4k->clear();
    ui->lineEdit_8k->clear();
    if(ui->radioButton_formula->isChecked())
    {
        ui->lineEdit_width->clear();
        ui->stackedWidget_input->setCurrentWidget(ui->page_circle_formula);
        ui->stackedWidget_descri->setCurrentWidget(ui->page_circle_descri);
    }
    else if(ui->radioButton_known->isChecked())
    {
        ui->lineEdit_model->clear();
        ui->stackedWidget_input->setCurrentWidget(ui->page_known);
        ui->stackedWidget_descri->setCurrentWidget(ui->page_white);
    }
}


void Dialog_elbow::on_radioButton_rect_clicked()
{
    ui->lineEdit_63->clear();
    ui->lineEdit_125->clear();
    ui->lineEdit_250->clear();
    ui->lineEdit_500->clear();
    ui->lineEdit_1k->clear();
    ui->lineEdit_2k->clear();
    ui->lineEdit_4k->clear();
    ui->lineEdit_8k->clear();
    if(ui->radioButton_formula->isChecked())
    {
        ui->lineEdit_diameter->clear();
        ui->stackedWidget_input->setCurrentWidget(ui->page_rect_formula);
        ui->stackedWidget_descri->setCurrentWidget(ui->page_rect_descri);
    }
    else if(ui->radioButton_known->isChecked())
    {
        ui->lineEdit_model->clear();
        ui->stackedWidget_input->setCurrentWidget(ui->page_known);
        ui->stackedWidget_descri->setCurrentWidget(ui->page_white);
    }
}

