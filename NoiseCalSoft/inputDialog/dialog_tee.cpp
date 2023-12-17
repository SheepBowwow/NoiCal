#include "inputDialog/dialog_tee.h"
#include "ui_dialog_tee.h"
#include <cmath>

Dialog_tee::Dialog_tee(QWidget *parent, int editRow,  const Tee_atten& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_tee)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件

    ui->radioButton_formula->setChecked(true);
    ui->radioButton_known->setChecked(false);

    ui->stackedWidget_input->setCurrentWidget(ui->page_formula);
    ui->stackedWidget_descri->setCurrentWidget(ui->page_descri);

    if(editRow != -1)
    {
        QStringList parts = data.model.split('/'); // 使用split函数按照斜杠分割字符串

        // parts中的第一个元素是前缀，第二个元素是后缀（如果有的话）
        QString suffix = parts.value(0).trimmed(); // 去除前缀两边的空格
        QString q1 = "";
        for(int i = 0; i < suffix.length(); i++)
        {
            if(suffix[i] >= '0' && suffix[i] <= '9')
            {
                q1 += suffix[i];
            }
        }
        QString q = parts.value(1).trimmed(); // 去除后缀两边的空格

        ui->lineEdit_brand->setText(data.brand);
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
            ui->stackedWidget_input->setCurrentWidget(ui->page_formula);
            ui->stackedWidget_descri->setCurrentWidget(ui->page_descri);
            ui->lineEdit_q->setText(q);
            ui->lineEdit_q1->setText(q1);
        }
    }

    //连接lineEdit信号与槽
    connect(ui->lineEdit_q, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->lineEdit_q1, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_tee::~Dialog_tee()
{
    delete ui;
}


void Dialog_tee::on_close_clicked()
{
    this->close();
}

void* Dialog_tee::getNoi()
{
    return noi == nullptr ? nullptr : noi;
}

//点击确认键
void Dialog_tee::on_pushButton_confirm_clicked()
{
    this->noi = new Tee_atten;

    // 获取对应行的数据，将界面上的数据保存到对应行中
    noi->brand = ui->lineEdit_brand->text();
    noi->noi_63 = ui->lineEdit_63->text();
    noi->noi_125 = ui->lineEdit_125->text();
    noi->noi_250 = ui->lineEdit_250->text();
    noi->noi_500 = ui->lineEdit_500->text();
    noi->noi_1k = ui->lineEdit_1k->text();
    noi->noi_2k = ui->lineEdit_2k->text();
    noi->noi_4k = ui->lineEdit_4k->text();
    noi->noi_8k = ui->lineEdit_8k->text();
    if(ui->radioButton_formula->isChecked())
    {
        noi->getMode = "公式";
        noi->model = "ST" + ui->lineEdit_q1->text() + "/" + ui->lineEdit_q->text();
    }
    else if(ui->radioButton_known->isChecked())
    {
        noi->getMode = "厂家";
        noi->model = ui->lineEdit_model->text();
    }

    noi->identifier = noi->model;
    this->accept(); // 关闭对话框
}

//计算总值 槽函数实现
void Dialog_tee::calNoise() {
    // 检查所有八个QLineEdit是否都有数据
    if(ui->lineEdit_q->text().isEmpty() || ui->lineEdit_q1->text().isEmpty()) {
        // 如果有任何一个QLineEdit没有数据，不执行计算
        return;
    }

    double noise;
    double q1 = ui->lineEdit_q1->text().toDouble();
    double q =  ui->lineEdit_q->text().toDouble();

    noise = 10 * log10(q1 / q);
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

//通过厂家信息
void Dialog_tee::on_radioButton_known_clicked()
{
    ui->lineEdit_q->clear();
    ui->lineEdit_q1->clear();
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
void Dialog_tee::on_radioButton_formula_clicked()
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

    ui->stackedWidget_input->setCurrentWidget(ui->page_formula);
    ui->stackedWidget_descri->setCurrentWidget(ui->page_descri);

    ui->lineEdit_63->setReadOnly(true);
    ui->lineEdit_125->setReadOnly(true);
    ui->lineEdit_250->setReadOnly(true);
    ui->lineEdit_500->setReadOnly(true);
    ui->lineEdit_1k->setReadOnly(true);
    ui->lineEdit_2k->setReadOnly(true);
    ui->lineEdit_4k->setReadOnly(true);
    ui->lineEdit_8k->setReadOnly(true);
}

