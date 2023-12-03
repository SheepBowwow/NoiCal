#include "inputDialog/dialog_pipe.h"
#include "ui_dialog_pipe.h"
#include <cmath>
#define Pi 3.14159265358979323846
Dialog_pipe::Dialog_pipe(QWidget *parent, int editRow, const Pipe_atten& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_pipe)
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
        QString length = "";
        QString width = "";
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
            QStringList parts = data.model.split('X'); // 使用split函数按照斜杠分割字符串
            // parts中的第一个元素是前缀，第二个元素是后缀（如果有的话）
            QString suffix = parts.value(0).trimmed(); // 去除前缀两边的空格
            for(int i = 0; i < suffix.length(); i++)
            {
                if(suffix[i] >= '0' && suffix[i] <= '9')
                {
                    length += suffix[i];
                }
            }
            width = parts.value(1).trimmed(); // 去除后缀两边的空格
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
                ui->radioButton_circle->setChecked(false);
                ui->radioButton_rect->setChecked(true);
                ui->stackedWidget_input->setCurrentWidget(ui->page_rect_formula);
                ui->stackedWidget_descri->setCurrentWidget(ui->page_rect_descri);
                ui->lineEdit_width->setText(width);
                ui->lineEdit_length->setText(length);
            }
        }
    }

    //连接lineEdit信号与槽
    connect(ui->lineEdit_diameter, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->lineEdit_width, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->lineEdit_length, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));


    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_pipe::~Dialog_pipe()
{
    delete ui;
}

void* Dialog_pipe::getNoi()
{
    return noi == nullptr ? nullptr : noi;
}

void Dialog_pipe::on_close_clicked()
{
    this->close();
}

//计算总值 槽函数实现
void Dialog_pipe::calNoise()
{
    if(ui->radioButton_circle->isChecked() && ui->lineEdit_diameter->text().isEmpty())
    {
        return;
    }
    else if(ui->radioButton_rect->isChecked() && (ui->lineEdit_length->text().isEmpty() || ui->lineEdit_width->text().isEmpty()))
    {
        return;
    }
    double pa = 0, c = 0, s = 0;
    double noi[8];
    QVector<QLineEdit*> v_lineEdit = {ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500, ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k};
    if(ui->radioButton_circle->isChecked())
    {
        c = 2 * Pi * ui->lineEdit_diameter->text().toDouble();
        s = Pi * ui->lineEdit_diameter->text().toDouble() * ui->lineEdit_diameter->text().toDouble();

        pa = c / s;
    }
    else if(ui->radioButton_rect->isChecked())
    {
        c = 2 * (ui->lineEdit_length->text().toDouble() + ui->lineEdit_width->text().toDouble());
        s = ui->lineEdit_length->text().toDouble() * ui->lineEdit_width->text().toDouble();

        pa = c / s;
    }

    if(pa <= 0.002)
    {
        noi[0] = 165 * pa;
        noi[1] = 165 * pa;
        noi[2] = 80 * pa;
        noi[3] = 35 * pa;
        noi[4] = 35 * pa;
        noi[5] = 35 * pa;
        noi[6] = 35 * pa;
        noi[7] = 35 * pa;
    }
    else if(pa > 0.002 && pa <= 0.003)
    {
        noi[0] = 160 * pa + 0.01;
        noi[1] = 0.33;
        noi[2] = 70 * pa + 0.02;
        noi[3] = 0.07;
        noi[4] = 0.07;
        noi[5] = 0.07;
        noi[6] = 0.07;
        noi[7] = 0.07;
    }
    else if(pa > 0.003 && pa <= 0.007)
    {
        noi[0] = 82.5 * pa + 0.2425;
        noi[1] = 82.5 * pa;
        noi[2] = 25 * pa + 0.155;
        noi[3] = 7.5 * pa + 0.0475;
        noi[4] = 7.5 * pa + 0.0475;
        noi[5] = 7.5 * pa + 0.0475;
        noi[6] = 7.5 * pa + 0.0475;
        noi[7] = 7.5 * pa + 0.0475;
    }
    else if(pa > 0.007 && pa <= 0.01)
    {
        noi[0] = 170 * pa - 0.39;
        noi[1] = 0.66;
        noi[2] = 0.33;
        noi[3] = 20 * pa + 0.04;
        noi[4] = 20 * pa + 0.04;
        noi[5] = 20 * pa + 0.04;
        noi[6] = 20 * pa + 0.04;
        noi[7] = 20 * pa + 0.04;
    }
    else if(pa > 0.01 && pa <= 0.013)
    {
        noi[0] = -53.3 * pa + 1.84;
        noi[1] = 0.66;
        noi[2] = 0.33;
        noi[3] = 13.3 * pa + 0.027;
        noi[4] = 13.3 * pa + 0.027;
        noi[5] = 13.3 * pa + 0.027;
        noi[6] = 13.3 * pa + 0.027;
        noi[7] = 13.3 * pa + 0.027;
    }
    else if(pa > 0.013 && pa <= 0.026)
    {
        noi[0] = -13.1 * pa + 1.32;
        noi[1] = 0.66;
        noi[2] = 0.33;
        noi[3] = 10 * pa + 0.07;
        noi[4] = 10 * pa + 0.07;
        noi[5] = 10 * pa + 0.07;
        noi[6] = 10 * pa + 0.07;
        noi[7] = 10 * pa + 0.07;
    }
    else if(pa > 0.026)
    {
        noi[0] = 0.98;
        noi[1] = 0.66;
        noi[2] = 0.33;
        noi[3] = 0.33;
        noi[4] = 0.33;
        noi[5] = 0.33;
        noi[6] = 0.33;
        noi[7] = 0.33;
    }

    for(int i = 0; i < 8; i++)
    {
        v_lineEdit[i]->setText(QString::number(noi[i],'f',2));
    }
}


void Dialog_pipe::on_radioButton_circle_clicked()
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
        ui->lineEdit_length->clear();
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


void Dialog_pipe::on_radioButton_rect_clicked()
{
    ui->lineEdit_63->clear();
    ui->lineEdit_125->clear();
    ui->lineEdit_250->clear();
    ui->lineEdit_500->clear();
    ui->lineEdit_1k->clear();
    ui->lineEdit_2k->clear();
    ui->lineEdit_4k->clear();
    ui->lineEdit_8k->clear();
    ui->lineEdit_length->clear();
    ui->lineEdit_width->clear();
    ui->lineEdit_diameter->clear();

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


void Dialog_pipe::on_pushButton_confirm_clicked()
{
    this->noi = new Pipe_atten;

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
            noi->model = "PD" + ui->lineEdit_diameter->text();
            noi->diameter = ui->lineEdit_diameter->text();
        }
        else if(noi->type == "方形")
        {
            noi->model = "P" + ui->lineEdit_length->text() + "X" + ui->lineEdit_width->text();
            noi->width = ui->lineEdit_width->text();
            noi->length = ui->lineEdit_length->text();
        }
    }
    else if(ui->radioButton_known->isChecked())
    {
        noi->getMode = "厂家";
        noi->model = ui->lineEdit_model->text();
    }

    noi->number = noi->model;

    this->accept(); // 关闭对话框
}


void Dialog_pipe::on_radioButton_known_clicked()
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


void Dialog_pipe::on_radioButton_formula_clicked()
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

