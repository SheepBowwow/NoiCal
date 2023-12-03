#include "inputDialog/dialog_reducer.h"
#include "ui_dialog_reducer.h"
#include <cmath>
#define Pi 3.14159265358979323846

Dialog_reducer::Dialog_reducer(QWidget *parent, int editRow,  const Reducer_atten& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_reducer)
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
        QString diameter_before = "";
        QString diameter_after = "";
        QString length_before = "";
        QString length_after = "";
        QString width_before = "";
        QString width_after = "";
        QStringList parts = data.model.split('-'); // 使用split函数按照斜杠分割字符串
        // parts中的第一个元素是前缀，第二个元素是后缀（如果有的话）
        QString prefix = parts.value(0).trimmed(); // 去除前缀两边的空格
        QString suffix = parts.value(1).trimmed(); // 去除后缀两边的空格
        if(data.type == "圆形")
        {
            for(int i = 0; i < prefix.length(); i++)
            {
                if(prefix[i] >= '0' && prefix[i] <= '9')
                {
                    diameter_before += prefix[i];
                }
            }
            diameter_after = suffix;
        }
        else if(data.type == "方形")
        {
            QStringList prefix_parts = prefix.split('X'); // 使用split函数按照X分割字符串
            QString length_parts = prefix_parts.value(0).trimmed();
            for(int i = 0; i < length_parts.length(); i++)
            {
                if(length_parts[i] >= '0' && length_parts[i] <= '9')
                {
                    length_before += length_parts[i];
                }
            }
            width_before = prefix_parts.value(1).trimmed();
            QStringList suffix_parts = suffix.split('X'); // 使用split函数按照X分割字符串
            length_parts = suffix_parts.value(0).trimmed();
            for(int i = 0; i < length_parts.length(); i++)
            {
                if(length_parts[i] >= '0' && length_parts[i] <= '9')
                {
                    length_after += length_parts[i];
                }
            }
            width_after = suffix_parts.value(1).trimmed();
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
                ui->lineEdit_diameter_before->setText(diameter_before);
                ui->lineEdit_diameter_after->setText(diameter_after);
            }
            else if(data.type == "方形")
            {
                ui->radioButton_circle->setChecked(false);
                ui->radioButton_rect->setChecked(true);
                ui->stackedWidget_input->setCurrentWidget(ui->page_rect_formula);
                ui->stackedWidget_descri->setCurrentWidget(ui->page_rect_descri);
                ui->lineEdit_length_before->setText(length_before);
                ui->lineEdit_length_after->setText(length_after);
                ui->lineEdit_width_before->setText(width_before);
                ui->lineEdit_width_after->setText(width_after);
            }
        }
    }

    //连接lineEdit信号与槽
    connect(ui->lineEdit_diameter_before, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->lineEdit_diameter_after, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->lineEdit_length_before, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->lineEdit_length_after, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->lineEdit_width_before, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->lineEdit_width_after, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_reducer::~Dialog_reducer()
{
    delete ui;
}

void Dialog_reducer::on_close_clicked()
{
    this->close();
}

void* Dialog_reducer::getNoi()
{
    return noi == nullptr ? nullptr : noi;
}

//计算总值 槽函数实现
void Dialog_reducer::calNoise() {
    double s1 = 0,s2 = 0,noise = 0;;
    if(ui->radioButton_circle->isChecked())
    {
        if(ui->lineEdit_diameter_before->text().isEmpty() || ui->lineEdit_diameter_after->text().isEmpty())
        {
            return;
        }
        s1 = Pi * (ui->lineEdit_diameter_before->text().toDouble() / 2000.0) * (ui->lineEdit_diameter_before->text().toDouble()/2000.0);
        s2 = Pi * (ui->lineEdit_diameter_after->text().toDouble()/2000.0) * (ui->lineEdit_diameter_after->text().toDouble()/2000.0);
    }
    else if(ui->radioButton_rect->isChecked())
    {
        if(ui->lineEdit_length_before->text().isEmpty() || ui->lineEdit_length_after->text().isEmpty()
                || ui->lineEdit_width_before->text().isEmpty() || ui->lineEdit_width_after->text().isEmpty())
        {
            return;
        }
        s1 = (ui->lineEdit_length_before->text().toDouble()/1000.0) * (ui->lineEdit_width_before->text().toDouble()/1000.0);
        s2 = (ui->lineEdit_length_after->text().toDouble()/1000.0) * (ui->lineEdit_width_after->text().toDouble()/1000.0);
    }


    noise = 10 * log10((pow((s2/s1 + 1),2)) / (4 * (s2 / s1)));

    QString str_noise = QString::number(noise, 'f', 1);
    ui->lineEdit_63->setText(str_noise);
    ui->lineEdit_125->setText(str_noise);
    ui->lineEdit_250->setText(str_noise);
    ui->lineEdit_500->setText(str_noise);
    ui->lineEdit_1k->setText(str_noise);
    ui->lineEdit_2k->setText(str_noise);
    ui->lineEdit_4k->setText(str_noise);
    ui->lineEdit_8k->setText(str_noise);
}

void Dialog_reducer::on_pushButton_confirm_clicked()
{
    this->noi = new Reducer_atten;

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
            noi->model = "D" + ui->lineEdit_diameter_before->text() + "-" + ui->lineEdit_diameter_after->text();
            noi->diameter_before = ui->lineEdit_diameter_before->text();
            noi->diameter_after = ui->lineEdit_diameter_after->text();
        }
        else if(noi->type == "方形")
        {
            noi->model = ui->lineEdit_length_before->text() + "X" + ui->lineEdit_width_before->text() + "-" + ui->lineEdit_length_after->text() + "X" + ui->lineEdit_width_after->text();
            noi->length_before = ui->lineEdit_length_before->text();
            noi->length_after = ui->lineEdit_length_after->text();
            noi->width_before = ui->lineEdit_width_before->text();
            noi->width_after = ui->lineEdit_width_after->text();
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


//通过厂家信息
void Dialog_reducer::on_radioButton_known_clicked()
{
    if(ui->radioButton_circle->isChecked())
    {
        ui->lineEdit_diameter_before->clear();
        ui->lineEdit_diameter_after->clear();
    }
    else if(ui->radioButton_rect->isChecked())
    {
        ui->lineEdit_length_before->clear();
        ui->lineEdit_length_after->clear();
        ui->lineEdit_width_before->clear();
        ui->lineEdit_width_after->clear();
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
void Dialog_reducer::on_radioButton_formula_clicked()
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


void Dialog_reducer::on_radioButton_circle_clicked()
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
        ui->lineEdit_length_before->clear();
        ui->lineEdit_length_after->clear();
        ui->lineEdit_width_before->clear();
        ui->lineEdit_width_after->clear();
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


void Dialog_reducer::on_radioButton_rect_clicked()
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
        ui->lineEdit_diameter_before->clear();
        ui->lineEdit_diameter_after->clear();
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

