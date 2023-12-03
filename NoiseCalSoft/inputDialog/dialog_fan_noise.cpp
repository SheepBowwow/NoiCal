#include "inputDialog/dialog_fan_noise.h"
#include "ui_dialog_fan_noise.h"
#include <cmath>

Dialog_fan_noise::Dialog_fan_noise(QWidget *parent, int editRow,  const Fan_noise& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_fan_noise)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件
    if(editRow != -1)
    {
        ui->lineEdit_number->setText(data.number);
        ui->lineEdit_brand->setText(data.brand);
        ui->lineEdit_model->setText(data.model);
        ui->lineEdit_air_volume->setText(data.air_volume);
        ui->lineEdit_static_pressure->setText(data.static_pressure);

        ui->lineEdit_in_63->setText(data.noi_in_63);
        ui->lineEdit_in_125->setText(data.noi_in_125);
        ui->lineEdit_in_250->setText(data.noi_in_250);
        ui->lineEdit_in_500->setText(data.noi_in_500);
        ui->lineEdit_in_1k->setText(data.noi_in_1k);
        ui->lineEdit_in_2k->setText(data.noi_in_2k);
        ui->lineEdit_in_4k->setText(data.noi_in_4k);
        ui->lineEdit_in_8k->setText(data.noi_in_8k);
        ui->lineEdit_in_total->setText(data.noi_in_total);

        ui->lineEdit_out_63->setText(data.noi_out_63);
        ui->lineEdit_out_125->setText(data.noi_out_125);
        ui->lineEdit_out_250->setText(data.noi_out_250);
        ui->lineEdit_out_500->setText(data.noi_out_500);
        ui->lineEdit_out_1k->setText(data.noi_out_1k);
        ui->lineEdit_out_2k->setText(data.noi_out_2k);
        ui->lineEdit_out_4k->setText(data.noi_out_4k);
        ui->lineEdit_out_8k->setText(data.noi_out_8k);
        ui->lineEdit_out_total->setText(data.noi_out_total);
    }

    //连接lineEdit信号与槽
    //进口
    connect(ui->lineEdit_in_63, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_in_125, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_in_250, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_in_500, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_in_1k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_in_2k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_in_4k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_in_8k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    //出口
    connect(ui->lineEdit_out_63, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_out_125, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_out_250, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_out_500, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_out_1k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_out_2k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_out_4k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_out_8k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_fan_noise::~Dialog_fan_noise()
{
    delete ui;
}

void Dialog_fan_noise::on_pushButton_confirm_clicked()
{
    this->noi = new Fan_noise;

    // 获取对应行的数据，将界面上的数据保存到对应行中
    noi->number = ui->lineEdit_number->text();
    noi->brand = ui->lineEdit_brand->text();
    noi->model = ui->lineEdit_model->text();
    noi->air_volume = ui->lineEdit_air_volume->text();
    noi->static_pressure = ui->lineEdit_static_pressure->text();
    noi->noi_in_63 = ui->lineEdit_in_63->text();
    noi->noi_in_125 = ui->lineEdit_in_125->text();
    noi->noi_in_250 = ui->lineEdit_in_250->text();
    noi->noi_in_500 = ui->lineEdit_in_500->text();
    noi->noi_in_1k = ui->lineEdit_in_1k->text();
    noi->noi_in_2k = ui->lineEdit_in_2k->text();
    noi->noi_in_4k = ui->lineEdit_in_4k->text();
    noi->noi_in_8k = ui->lineEdit_in_8k->text();
    noi->noi_in_total = ui->lineEdit_in_total->text();

    noi->noi_out_63 = ui->lineEdit_out_63->text();
    noi->noi_out_125 = ui->lineEdit_out_125->text();
    noi->noi_out_250 = ui->lineEdit_out_250->text();
    noi->noi_out_500 = ui->lineEdit_out_500->text();
    noi->noi_out_1k = ui->lineEdit_out_1k->text();
    noi->noi_out_2k = ui->lineEdit_out_2k->text();
    noi->noi_out_4k = ui->lineEdit_out_4k->text();
    noi->noi_out_8k = ui->lineEdit_out_8k->text();
    noi->noi_out_total = ui->lineEdit_out_total->text();
    this->accept(); // 关闭对话框
}

void* Dialog_fan_noise::getNoi()
{
    return noi == nullptr ? nullptr : noi;
}

void Dialog_fan_noise::on_close_clicked()
{
    this->close();
}

//计算总值 槽函数实现
void Dialog_fan_noise::calTotalNoise() {
    // 检查所有八个QLineEdit是否都有数据
    if(!(ui->lineEdit_in_63->text().isEmpty() || ui->lineEdit_in_125->text().isEmpty()
       || ui->lineEdit_in_250->text().isEmpty() || ui->lineEdit_in_500->text().isEmpty() ||
            ui->lineEdit_in_1k->text().isEmpty() || ui->lineEdit_in_2k->text().isEmpty() ||
            ui->lineEdit_in_4k->text().isEmpty() || ui->lineEdit_in_8k->text().isEmpty())) {
        double noi_63 = ui->lineEdit_in_63->text().toDouble();
        double noi_125 = ui->lineEdit_in_125->text().toDouble();
        double noi_250 = ui->lineEdit_in_250->text().toDouble();
        double noi_500 = ui->lineEdit_in_500->text().toDouble();
        double noi_1k = ui->lineEdit_in_1k->text().toDouble();
        double noi_2k = ui->lineEdit_in_2k->text().toDouble();
        double noi_4k = ui->lineEdit_in_4k->text().toDouble();
        double noi_8k = ui->lineEdit_in_8k->text().toDouble();
        double temp = pow(10, noi_63 / 10) + pow(10, noi_125 / 10) + pow(10, noi_250 / 10) + pow(10, noi_500 / 10)
                + pow(10, noi_1k / 10) + pow(10, noi_2k / 10) + pow(10, noi_4k / 10) + pow(10, noi_8k / 10);
        double total = 10 * log10(temp);
        QString totalString = QString::number(total,'f',1); // 将total转换为字符串
        ui->lineEdit_in_total->setText(totalString); // 设置lineEdit的文本为total的字符串表示
    }

    if(!(ui->lineEdit_out_63->text().isEmpty() || ui->lineEdit_out_125->text().isEmpty()
       || ui->lineEdit_out_250->text().isEmpty() || ui->lineEdit_out_500->text().isEmpty() ||
            ui->lineEdit_out_1k->text().isEmpty() || ui->lineEdit_out_2k->text().isEmpty() ||
            ui->lineEdit_out_4k->text().isEmpty() || ui->lineEdit_out_8k->text().isEmpty())) {
        double noi_63 = ui->lineEdit_out_63->text().toDouble();
        double noi_125 = ui->lineEdit_out_125->text().toDouble();
        double noi_250 = ui->lineEdit_out_250->text().toDouble();
        double noi_500 = ui->lineEdit_out_500->text().toDouble();
        double noi_1k = ui->lineEdit_out_1k->text().toDouble();
        double noi_2k = ui->lineEdit_out_2k->text().toDouble();
        double noi_4k = ui->lineEdit_out_4k->text().toDouble();
        double noi_8k = ui->lineEdit_out_8k->text().toDouble();
        double temp = pow(10, noi_63 / 10) + pow(10, noi_125 / 10) + pow(10, noi_250 / 10) + pow(10, noi_500 / 10)
                + pow(10, noi_1k / 10) + pow(10, noi_2k / 10) + pow(10, noi_4k / 10) + pow(10, noi_8k / 10);
        double total = 10 * log10(temp);
        QString totalString = QString::number(total,'f',1); // 将total转换为字符串
        ui->lineEdit_out_total->setText(totalString); // 设置lineEdit的文本为total的字符串表示
    }
}

