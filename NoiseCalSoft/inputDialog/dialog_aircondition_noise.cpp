#include "inputDialog/dialog_aircondition_noise.h"
#include "ui_dialog_aircondition_noise.h"
#include <cmath>

Dialog_aircondition_noise::Dialog_aircondition_noise(QWidget *parent, int editRow, int fan,  const Aircondition_noise& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_aircondition_noise)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件
    ui->radioButton_single->setChecked(true);

    if(fan == 0)
    {
        ui->radioButton_single->setChecked(true);
        on_radioButton_single_clicked();
    }
    else if(fan == 1)
    {
        ui->radioButton_double->setChecked(true);
        on_radioButton_double_clicked();
    }

    if(editRow != -1)
    {
        if(data.type == "单风机")
        {
            ui->radioButton_single->setChecked(true);
            on_radioButton_single_clicked();
        }
        else if(data.type == "双风机")
        {
            ui->radioButton_double->setChecked(true);
            on_radioButton_double_clicked();
            ui->lineEdit_exhaust_in_63->setText(data.noi_exhaust_in_63);
            ui->lineEdit_exhaust_in_125->setText(data.noi_exhaust_in_125);
            ui->lineEdit_exhaust_in_250->setText(data.noi_exhaust_in_250);
            ui->lineEdit_exhaust_in_500->setText(data.noi_exhaust_in_500);
            ui->lineEdit_exhaust_in_1k->setText(data.noi_exhaust_in_1k);
            ui->lineEdit_exhaust_in_2k->setText(data.noi_exhaust_in_2k);
            ui->lineEdit_exhaust_in_4k->setText(data.noi_exhaust_in_4k);
            ui->lineEdit_exhaust_in_8k->setText(data.noi_exhaust_in_8k);
            ui->lineEdit_exhaust_in_total->setText(data.noi_exhaust_in_total);

            ui->lineEdit_exhaust_out_63->setText(data.noi_exhaust_out_63);
            ui->lineEdit_exhaust_out_125->setText(data.noi_exhaust_out_125);
            ui->lineEdit_exhaust_out_250->setText(data.noi_exhaust_out_250);
            ui->lineEdit_exhaust_out_500->setText(data.noi_exhaust_out_500);
            ui->lineEdit_exhaust_out_1k->setText(data.noi_exhaust_out_1k);
            ui->lineEdit_exhaust_out_2k->setText(data.noi_exhaust_out_2k);
            ui->lineEdit_exhaust_out_4k->setText(data.noi_exhaust_out_4k);
            ui->lineEdit_exhaust_out_8k->setText(data.noi_exhaust_out_8k);
            ui->lineEdit_exhaust_out_total->setText(data.noi_exhaust_out_total);
        }

        ui->lineEdit_number->setText(data.number);
        ui->lineEdit_brand->setText(data.brand);
        ui->lineEdit_model->setText(data.model);
        ui->lineEdit_air_volume->setText(data.air_volume);
        ui->lineEdit_static_pressure->setText(data.static_pressure);

        ui->lineEdit_send_in_63->setText(data.noi_send_in_63);
        ui->lineEdit_send_in_125->setText(data.noi_send_in_125);
        ui->lineEdit_send_in_250->setText(data.noi_send_in_250);
        ui->lineEdit_send_in_500->setText(data.noi_send_in_500);
        ui->lineEdit_send_in_1k->setText(data.noi_send_in_1k);
        ui->lineEdit_send_in_2k->setText(data.noi_send_in_2k);
        ui->lineEdit_send_in_4k->setText(data.noi_send_in_4k);
        ui->lineEdit_send_in_8k->setText(data.noi_send_in_8k);
        ui->lineEdit_send_in_total->setText(data.noi_send_in_total);

        ui->lineEdit_send_out_63->setText(data.noi_send_out_63);
        ui->lineEdit_send_out_125->setText(data.noi_send_out_125);
        ui->lineEdit_send_out_250->setText(data.noi_send_out_250);
        ui->lineEdit_send_out_500->setText(data.noi_send_out_500);
        ui->lineEdit_send_out_1k->setText(data.noi_send_out_1k);
        ui->lineEdit_send_out_2k->setText(data.noi_send_out_2k);
        ui->lineEdit_send_out_4k->setText(data.noi_send_out_4k);
        ui->lineEdit_send_out_8k->setText(data.noi_send_out_8k);
        ui->lineEdit_send_out_total->setText(data.noi_send_out_total);

    }

    //连接lineEdit信号与槽
    //进口
    connect(ui->lineEdit_send_in_63, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_in_125, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_in_250, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_in_500, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_in_1k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_in_2k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_in_4k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_in_8k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    //出口
    connect(ui->lineEdit_send_out_63, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_out_125, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_out_250, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_out_500, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_out_1k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_out_2k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_out_4k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_send_out_8k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));

    //进口
    connect(ui->lineEdit_exhaust_in_63, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_in_125, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_in_250, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_in_500, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_in_1k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_in_2k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_in_4k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_in_8k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    //出口
    connect(ui->lineEdit_exhaust_out_63, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_out_125, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_out_250, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_out_500, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_out_1k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_out_2k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_out_4k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_exhaust_out_8k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_aircondition_noise::~Dialog_aircondition_noise()
{
    delete ui;
}

void Dialog_aircondition_noise::on_pushButton_confirm_clicked()
{
    this->noi = new Aircondition_noise;

    // 获取对应行的数据，将界面上的数据保存到对应行中
    noi->number = ui->lineEdit_number->text();
    if(ui->radioButton_single->isChecked())
        noi->type = "单风机";
    else if(ui->radioButton_double->isChecked())
        noi->type = "双风机";

    noi->brand = ui->lineEdit_brand->text();
    noi->model = ui->lineEdit_model->text();
    noi->air_volume = ui->lineEdit_air_volume->text();
    noi->static_pressure = ui->lineEdit_static_pressure->text();
    noi->noi_send_in_63 = ui->lineEdit_send_in_63->text();
    noi->noi_send_in_125 = ui->lineEdit_send_in_125->text();
    noi->noi_send_in_250 = ui->lineEdit_send_in_250->text();
    noi->noi_send_in_500 = ui->lineEdit_send_in_500->text();
    noi->noi_send_in_1k = ui->lineEdit_send_in_1k->text();
    noi->noi_send_in_2k = ui->lineEdit_send_in_2k->text();
    noi->noi_send_in_4k = ui->lineEdit_send_in_4k->text();
    noi->noi_send_in_8k = ui->lineEdit_send_in_8k->text();
    noi->noi_send_in_total = ui->lineEdit_send_in_total->text();

    noi->noi_send_out_63 = ui->lineEdit_send_out_63->text();
    noi->noi_send_out_125 = ui->lineEdit_send_out_125->text();
    noi->noi_send_out_250 = ui->lineEdit_send_out_250->text();
    noi->noi_send_out_500 = ui->lineEdit_send_out_500->text();
    noi->noi_send_out_1k = ui->lineEdit_send_out_1k->text();
    noi->noi_send_out_2k = ui->lineEdit_send_out_2k->text();
    noi->noi_send_out_4k = ui->lineEdit_send_out_4k->text();
    noi->noi_send_out_8k = ui->lineEdit_send_out_8k->text();
    noi->noi_send_out_total = ui->lineEdit_send_out_total->text();

    noi->noi_exhaust_in_63 = ui->lineEdit_exhaust_in_63->text();
    noi->noi_exhaust_in_125 = ui->lineEdit_exhaust_in_125->text();
    noi->noi_exhaust_in_250 = ui->lineEdit_exhaust_in_250->text();
    noi->noi_exhaust_in_500 = ui->lineEdit_exhaust_in_500->text();
    noi->noi_exhaust_in_1k = ui->lineEdit_exhaust_in_1k->text();
    noi->noi_exhaust_in_2k = ui->lineEdit_exhaust_in_2k->text();
    noi->noi_exhaust_in_4k = ui->lineEdit_exhaust_in_4k->text();
    noi->noi_exhaust_in_8k = ui->lineEdit_exhaust_in_8k->text();
    noi->noi_exhaust_in_total = ui->lineEdit_exhaust_in_total->text();

    noi->noi_exhaust_out_63 = ui->lineEdit_exhaust_out_63->text();
    noi->noi_exhaust_out_125 = ui->lineEdit_exhaust_out_125->text();
    noi->noi_exhaust_out_250 = ui->lineEdit_exhaust_out_250->text();
    noi->noi_exhaust_out_500 = ui->lineEdit_exhaust_out_500->text();
    noi->noi_exhaust_out_1k = ui->lineEdit_exhaust_out_1k->text();
    noi->noi_exhaust_out_2k = ui->lineEdit_exhaust_out_2k->text();
    noi->noi_exhaust_out_4k = ui->lineEdit_exhaust_out_4k->text();
    noi->noi_exhaust_out_8k = ui->lineEdit_exhaust_out_8k->text();
    noi->noi_exhaust_out_total = ui->lineEdit_exhaust_out_total->text();

    noi->identifier = noi->number;
    this->accept(); // 关闭对话框
}

void* Dialog_aircondition_noise::getNoi()
{
    return noi == nullptr ? nullptr : noi;
}

void Dialog_aircondition_noise::on_close_clicked()
{
    this->close();
}

//计算总值 槽函数实现
void Dialog_aircondition_noise::calTotalNoise() {
    // 检查所有八个QLineEdit是否都有数据
    if(!(ui->lineEdit_send_in_63->text().isEmpty() || ui->lineEdit_send_in_125->text().isEmpty()
       || ui->lineEdit_send_in_250->text().isEmpty() || ui->lineEdit_send_in_500->text().isEmpty() ||
            ui->lineEdit_send_in_1k->text().isEmpty() || ui->lineEdit_send_in_2k->text().isEmpty() ||
            ui->lineEdit_send_in_4k->text().isEmpty() || ui->lineEdit_send_in_8k->text().isEmpty())) {
        double noi_63 = ui->lineEdit_send_in_63->text().toDouble();
        double noi_125 = ui->lineEdit_send_in_125->text().toDouble();
        double noi_250 = ui->lineEdit_send_in_250->text().toDouble();
        double noi_500 = ui->lineEdit_send_in_500->text().toDouble();
        double noi_1k = ui->lineEdit_send_in_1k->text().toDouble();
        double noi_2k = ui->lineEdit_send_in_2k->text().toDouble();
        double noi_4k = ui->lineEdit_send_in_4k->text().toDouble();
        double noi_8k = ui->lineEdit_send_in_8k->text().toDouble();
        double temp = pow(10, noi_63 / 10) + pow(10, noi_125 / 10) + pow(10, noi_250 / 10) + pow(10, noi_500 / 10)
                + pow(10, noi_1k / 10) + pow(10, noi_2k / 10) + pow(10, noi_4k / 10) + pow(10, noi_8k / 10);
        double total = 10 * log10(temp);
        QString totalString = QString::number(total,'f',1); // 将total转换为字符串
        ui->lineEdit_send_in_total->setText(totalString); // 设置lineEdit的文本为total的字符串表示
    }

    if(!(ui->lineEdit_send_out_63->text().isEmpty() || ui->lineEdit_send_out_125->text().isEmpty()
       || ui->lineEdit_send_out_250->text().isEmpty() || ui->lineEdit_send_out_500->text().isEmpty() ||
            ui->lineEdit_send_out_1k->text().isEmpty() || ui->lineEdit_send_out_2k->text().isEmpty() ||
            ui->lineEdit_send_out_4k->text().isEmpty() || ui->lineEdit_send_out_8k->text().isEmpty())) {
        double noi_63 = ui->lineEdit_send_out_63->text().toDouble();
        double noi_125 = ui->lineEdit_send_out_125->text().toDouble();
        double noi_250 = ui->lineEdit_send_out_250->text().toDouble();
        double noi_500 = ui->lineEdit_send_out_500->text().toDouble();
        double noi_1k = ui->lineEdit_send_out_1k->text().toDouble();
        double noi_2k = ui->lineEdit_send_out_2k->text().toDouble();
        double noi_4k = ui->lineEdit_send_out_4k->text().toDouble();
        double noi_8k = ui->lineEdit_send_out_8k->text().toDouble();
        double temp = pow(10, noi_63 / 10) + pow(10, noi_125 / 10) + pow(10, noi_250 / 10) + pow(10, noi_500 / 10)
                + pow(10, noi_1k / 10) + pow(10, noi_2k / 10) + pow(10, noi_4k / 10) + pow(10, noi_8k / 10);
        double total = 10 * log10(temp);
        QString totalString = QString::number(total,'f',1); // 将total转换为字符串
        ui->lineEdit_send_out_total->setText(totalString); // 设置lineEdit的文本为total的字符串表示
    }

    // 检查所有八个QLineEdit是否都有数据
    if(!(ui->lineEdit_exhaust_in_63->text().isEmpty() || ui->lineEdit_exhaust_in_125->text().isEmpty()
       || ui->lineEdit_exhaust_in_250->text().isEmpty() || ui->lineEdit_exhaust_in_500->text().isEmpty() ||
            ui->lineEdit_exhaust_in_1k->text().isEmpty() || ui->lineEdit_exhaust_in_2k->text().isEmpty() ||
            ui->lineEdit_exhaust_in_4k->text().isEmpty() || ui->lineEdit_exhaust_in_8k->text().isEmpty())) {
        double noi_63 = ui->lineEdit_exhaust_in_63->text().toDouble();
        double noi_125 = ui->lineEdit_exhaust_in_125->text().toDouble();
        double noi_250 = ui->lineEdit_exhaust_in_250->text().toDouble();
        double noi_500 = ui->lineEdit_exhaust_in_500->text().toDouble();
        double noi_1k = ui->lineEdit_exhaust_in_1k->text().toDouble();
        double noi_2k = ui->lineEdit_exhaust_in_2k->text().toDouble();
        double noi_4k = ui->lineEdit_exhaust_in_4k->text().toDouble();
        double noi_8k = ui->lineEdit_exhaust_in_8k->text().toDouble();
        double temp = pow(10, noi_63 / 10) + pow(10, noi_125 / 10) + pow(10, noi_250 / 10) + pow(10, noi_500 / 10)
                + pow(10, noi_1k / 10) + pow(10, noi_2k / 10) + pow(10, noi_4k / 10) + pow(10, noi_8k / 10);
        double total = 10 * log10(temp);
        QString totalString = QString::number(total,'f',1); // 将total转换为字符串
        ui->lineEdit_exhaust_in_total->setText(totalString); // 设置lineEdit的文本为total的字符串表示
    }

    if(!(ui->lineEdit_exhaust_out_63->text().isEmpty() || ui->lineEdit_exhaust_out_125->text().isEmpty()
       || ui->lineEdit_exhaust_out_250->text().isEmpty() || ui->lineEdit_exhaust_out_500->text().isEmpty() ||
            ui->lineEdit_exhaust_out_1k->text().isEmpty() || ui->lineEdit_exhaust_out_2k->text().isEmpty() ||
            ui->lineEdit_exhaust_out_4k->text().isEmpty() || ui->lineEdit_exhaust_out_8k->text().isEmpty())) {
        double noi_63 = ui->lineEdit_exhaust_out_63->text().toDouble();
        double noi_125 = ui->lineEdit_exhaust_out_125->text().toDouble();
        double noi_250 = ui->lineEdit_exhaust_out_250->text().toDouble();
        double noi_500 = ui->lineEdit_exhaust_out_500->text().toDouble();
        double noi_1k = ui->lineEdit_exhaust_out_1k->text().toDouble();
        double noi_2k = ui->lineEdit_exhaust_out_2k->text().toDouble();
        double noi_4k = ui->lineEdit_exhaust_out_4k->text().toDouble();
        double noi_8k = ui->lineEdit_exhaust_out_8k->text().toDouble();
        double temp = pow(10, noi_63 / 10) + pow(10, noi_125 / 10) + pow(10, noi_250 / 10) + pow(10, noi_500 / 10)
                + pow(10, noi_1k / 10) + pow(10, noi_2k / 10) + pow(10, noi_4k / 10) + pow(10, noi_8k / 10);
        double total = 10 * log10(temp);
        QString totalString = QString::number(total,'f',1); // 将total转换为字符串
        ui->lineEdit_exhaust_out_total->setText(totalString); // 设置lineEdit的文本为total的字符串表示
    }
}

void Dialog_aircondition_noise::on_radioButton_single_clicked()
{
    QList<QLineEdit*> exhaustInEdit = {ui->lineEdit_exhaust_in_63, ui->lineEdit_exhaust_in_125, ui->lineEdit_exhaust_in_250, ui->lineEdit_exhaust_in_500,
                                     ui->lineEdit_exhaust_in_1k,ui->lineEdit_exhaust_in_2k, ui->lineEdit_exhaust_in_4k, ui->lineEdit_exhaust_in_8k};
    QList<QLineEdit*> exhaustOutEdit = {ui->lineEdit_exhaust_out_63, ui->lineEdit_exhaust_out_125, ui->lineEdit_exhaust_out_250, ui->lineEdit_exhaust_out_500,
                                     ui->lineEdit_exhaust_out_1k,ui->lineEdit_exhaust_out_2k, ui->lineEdit_exhaust_out_4k, ui->lineEdit_exhaust_out_8k};

    for(int i = 0; i < exhaustInEdit.size(); i++)
    {
        exhaustInEdit[i]->clear();
        exhaustOutEdit[i]->clear();
        exhaustInEdit[i]->setStyleSheet("background-color: rgb(240, 240, 240); border: 1px solid #9C9C9C;");
        exhaustOutEdit[i]->setStyleSheet("background-color: rgb(240, 240, 240); border: 1px solid #9C9C9C;");
        exhaustInEdit[i]->setReadOnly(true);
        exhaustOutEdit[i]->setReadOnly(true);
    }

    ui->lineEdit_exhaust_in_total->clear();
    ui->lineEdit_exhaust_in_total->setStyleSheet("background-color: rgb(240, 240, 240); border: 1px solid #9C9C9C;");
    ui->lineEdit_exhaust_out_total->clear();
    ui->lineEdit_exhaust_out_total->setStyleSheet("background-color: rgb(240, 240, 240); border: 1px solid #9C9C9C;");
}

void Dialog_aircondition_noise::on_radioButton_double_clicked()
{
    QList<QLineEdit*> exhaustInEdit = {ui->lineEdit_exhaust_in_63, ui->lineEdit_exhaust_in_125, ui->lineEdit_exhaust_in_250, ui->lineEdit_exhaust_in_500,
                                     ui->lineEdit_exhaust_in_1k,ui->lineEdit_exhaust_in_2k, ui->lineEdit_exhaust_in_4k, ui->lineEdit_exhaust_in_8k, ui->lineEdit_exhaust_in_total};
    QList<QLineEdit*> exhaustOutEdit = {ui->lineEdit_exhaust_out_63, ui->lineEdit_exhaust_out_125, ui->lineEdit_exhaust_out_250, ui->lineEdit_exhaust_out_500,
                                     ui->lineEdit_exhaust_out_1k,ui->lineEdit_exhaust_out_2k, ui->lineEdit_exhaust_out_4k, ui->lineEdit_exhaust_out_8k, ui->lineEdit_exhaust_out_total};

    for(int i = 0; i < exhaustInEdit.size(); i++)
    {
        exhaustInEdit[i]->setStyleSheet("");
        exhaustOutEdit[i]->setStyleSheet("");
        exhaustInEdit[i]->setReadOnly(false);
        exhaustOutEdit[i]->setReadOnly(false);
    }

    ui->lineEdit_exhaust_in_total->setReadOnly(true);
    ui->lineEdit_exhaust_out_total->setReadOnly(true);
}

