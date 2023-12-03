#include "inputDialog/dialog_air_diff.h"
#include "ui_dialog_air_diff.h"
#include "databasemanager.h"
#include "global_constant.h"
#include "globle_var.h"
#include <QDir>
#include <cmath>
#include <QSet>
#define Pi 3.14159265358979323846



Dialog_air_diff::Dialog_air_diff(const QString &name, QWidget *parent, int editRow, const AirDiff_noise& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_air_diff)
{
    ui->setupUi(this);
    if(name == "布风器+散流器")
        ui->stackedWidget->setCurrentWidget(ui->page_air_diff);
    else if(name == "布风器")
        ui->stackedWidget->setCurrentWidget(ui->page_air);
    else if(name == "散流器")
        ui->stackedWidget->setCurrentWidget(ui->page_diff);

    setTopWidget(ui->widget_top);  // 设置顶部部件
    QSet<QString> addedPrefixes;
    DBManager->queryKnownData(airDiffModel, AIR_DIFF_MODEL, AIR_DIFF_TABLE);
    ui->radioButton_circle->setChecked(true);
    ui->stackedWidget_input->setCurrentWidget(ui->page_circle);
    ui->radioButton_formula->setChecked(true);

    /******设置combobox******/
    for(int i = 0; i < airDiffModel.size(); i++)
    {
        QString s = airDiffModel[i];
        // 检查是否已经添加过该前缀
        ui->comboBox_model->addItem(s);
    }
    /******设置combobox******/
    ui->comboBox_model->setCurrentIndex(-1);


    if(editRow != -1)
    {
        QString diameter = "";
        QString length = "";
        QString width = "";
        if(data.type == "圆形")
        {
            diameter = data.size;
            ui->radioButton_circle->setChecked(true);
            ui->lineEdit_diameter->setText(diameter);
            ui->stackedWidget_input->setCurrentWidget(ui->page_circle);
        }
        else if(data.type == "方形")
        {
            ui->radioButton_rect->setChecked(true);
            QStringList parts = data.size.split('X'); // 使用split函数按照斜杠分割字符串
            // parts中的第一个元素是前缀，第二个元素是后缀（如果有的话）
            length = parts.value(0).trimmed(); // 去除前缀两边的空格
            width = parts.value(1).trimmed(); // 去除后缀两边的空格
            ui->lineEdit_length->setText(length);
            ui->lineEdit_width->setText(width);
            ui->stackedWidget_input->setCurrentWidget(ui->page_rect);
        }


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
        ui->lineEdit_total->setText(data.noi_total);

        ui->lineEdit_63_atten->setText(data.atten_63);
        ui->lineEdit_125_atten->setText(data.atten_125);
        ui->lineEdit_250_atten->setText(data.atten_250);
        ui->lineEdit_500_atten->setText(data.atten_500);
        ui->lineEdit_1k_atten->setText(data.atten_1k);
        ui->lineEdit_2k_atten->setText(data.atten_2k);
        ui->lineEdit_4k_atten->setText(data.atten_4k);
        ui->lineEdit_8k_atten->setText(data.atten_8k);

        ui->lineEdit_63_refl->setText(data.refl_63);
        ui->lineEdit_125_refl->setText(data.refl_125);
        ui->lineEdit_250_refl->setText(data.refl_250);
        ui->lineEdit_500_refl->setText(data.refl_500);
        ui->lineEdit_1k_refl->setText(data.refl_1k);
        ui->lineEdit_2k_refl->setText(data.refl_2k);
        ui->lineEdit_4k_refl->setText(data.refl_4k);
        ui->lineEdit_8k_refl->setText(data.refl_8k);
    }

    //连接lineEdit信号与槽
    connect(ui->lineEdit_63, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_125, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_250, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_500, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_1k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_2k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_4k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_8k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));

    //连接lineEdit信号与槽
    connect(ui->lineEdit_diameter, SIGNAL(textChanged(QString)), this, SLOT(calReflNoi()));
    connect(ui->lineEdit_length, SIGNAL(textChanged(QString)), this, SLOT(calReflNoi()));
    connect(ui->lineEdit_width, SIGNAL(textChanged(QString)), this, SLOT(calReflNoi()));

    connect(ui->pushButton_confirm, &QPushButton::clicked, ImageDialog::getInstance(":/images/image/refl.jpg",this), &ImageDialog::closeImageDialog);

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_air_diff::~Dialog_air_diff()
{
    delete ui;
}

//后缀选择后显示八分频噪音
void Dialog_air_diff::on_comboBox_model_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_total->clear();
    int band[8] = {63 , 125, 250, 500, 1000, 2000, 4000, 8000};
    QVector<QLineEdit*> v_lineEdit_noi = {ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500, ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k};
    QVector<QLineEdit*> v_lineEdit_atten = {ui->lineEdit_63_atten, ui->lineEdit_125_atten, ui->lineEdit_250_atten, ui->lineEdit_500_atten, ui->lineEdit_1k_atten, ui->lineEdit_2k_atten, ui->lineEdit_4k_atten, ui->lineEdit_8k_atten};
    QMap<int,QString> eightNoi;
    QMap<int,QString> eightAtten;
    QString modelName = QString(ui->comboBox_model->currentText());
    DBManager->queryEightNoi(eightNoi, AIR_DIFF_TABLE, AIR_DIFF_MODEL, modelName);
    DBManager->queryEightNoi(eightAtten, TERMINAL_TABLE, TERMINAL_MODEL, modelName);
    for(int i = 0; i < 8; i++)
    {
        v_lineEdit_noi[i]->setText(eightNoi[band[i]]);
        v_lineEdit_atten[i]->setText(eightAtten[band[i]]);
    }
    total_noi();
}

//计算总值
void Dialog_air_diff::total_noi()
{
    // 检查所有八个QLineEdit是否都有数据
    if(ui->lineEdit_63->text().isEmpty() || ui->lineEdit_125->text().isEmpty()
       || ui->lineEdit_250->text().isEmpty() || ui->lineEdit_500->text().isEmpty() ||
            ui->lineEdit_1k->text().isEmpty() || ui->lineEdit_2k->text().isEmpty() ||
            ui->lineEdit_4k->text().isEmpty() || ui->lineEdit_8k->text().isEmpty()) {
        // 如果有任何一个QLineEdit没有数据，不执行计算
        return;
    }
    double noi_63 = ui->lineEdit_63->text().toDouble();
    double noi_125 = ui->lineEdit_125->text().toDouble();
    double noi_250 = ui->lineEdit_250->text().toDouble();
    double noi_500 = ui->lineEdit_500->text().toDouble();
    double noi_1k = ui->lineEdit_1k->text().toDouble();
    double noi_2k = ui->lineEdit_2k->text().toDouble();
    double noi_4k = ui->lineEdit_4k->text().toDouble();
    double noi_8k = ui->lineEdit_8k->text().toDouble();
    double temp = pow(10, noi_63 / 10) + pow(10, noi_125 / 10) + pow(10, noi_250 / 10) + pow(10, noi_500 / 10)
            + pow(10, noi_1k / 10) + pow(10, noi_2k / 10) + pow(10, noi_4k / 10) + pow(10, noi_8k / 10);
    double total = 10 * log10(temp);
    QString totalString = QString::number(total,'f',1); // 将total转换为字符串
    ui->lineEdit_total->setText(totalString); // 设置lineEdit的文本为total的字符串表示
}

//计算总值 槽函数实现
void Dialog_air_diff::calTotalNoise() {
    // 检查所有八个QLineEdit是否都有数据
    if(ui->lineEdit_63->text().isEmpty() || ui->lineEdit_125->text().isEmpty()
       || ui->lineEdit_250->text().isEmpty() || ui->lineEdit_500->text().isEmpty() ||
            ui->lineEdit_1k->text().isEmpty() || ui->lineEdit_2k->text().isEmpty() ||
            ui->lineEdit_4k->text().isEmpty() || ui->lineEdit_8k->text().isEmpty()) {
        // 如果有任何一个QLineEdit没有数据，不执行计算
        return;
    }

    total_noi();
}

//计算总值 槽函数实现
void Dialog_air_diff::calReflNoi()
{
    double D = 0;
    if(ui->radioButton_circle->isChecked())
    {
        if(ui->lineEdit_diameter->text().isEmpty())
            return;
        D = ui->lineEdit_diameter->text().toDouble() / 1000.0;
    }
    else if(ui->radioButton_rect->isChecked())
    {
        if(ui->lineEdit_length->text().isEmpty() || ui->lineEdit_width->text().isEmpty())
            return;
        D = sqrt((4 * ui->lineEdit_length->text().toDouble() / 1000.0) * (ui->lineEdit_width->text().toDouble() / 1000.0) / Pi);
    }

    double f[] = {63,125,250,500,1000,2000,4000,8000};
    QVector<QLineEdit*> v_lineEdit = {ui->lineEdit_63_refl, ui->lineEdit_125_refl, ui->lineEdit_250_refl, ui->lineEdit_500_refl, ui->lineEdit_1k_refl, ui->lineEdit_2k_refl, ui->lineEdit_4k_refl, ui->lineEdit_8k_refl};
    for(int i = 0; i < 8; i++)
    {
        double atten = 0;
        atten = 10 * log10(1 + pow(((0.7 * 340) / (Pi * f[i] * D)),2));
        v_lineEdit[i]->setText(QString::number(atten,'f',2));
    }
}

void* Dialog_air_diff::getNoi()
{
    return noi == nullptr ? nullptr : noi;
}

//点击确认键
void Dialog_air_diff::on_pushButton_confirm_clicked()
{
    this->noi = new AirDiff_noise;

    if(ui->radioButton_formula->isChecked())
    {
        noi->getMode = "公式";
    }
    else if(ui->radioButton_known->isChecked())
    {
        noi->getMode = "厂家";
    }

    if(ui->radioButton_circle->isChecked())
    {
        noi->diameter = ui->lineEdit_diameter->text();
        noi->size = noi->diameter;
        noi->type = "圆形";
    }
    if(ui->radioButton_rect->isChecked())
    {
        noi->length = ui->lineEdit_length->text();
        noi->width = ui->lineEdit_width->text();
        noi->size = noi->length + "X" + noi->width;
        noi->type = "方形";
    }

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
    noi->noi_total = ui->lineEdit_total->text();
    noi->model = ui->comboBox_model->currentText();

    noi->atten_63 = ui->lineEdit_63_atten->text();
    noi->atten_125 = ui->lineEdit_125_atten->text();
    noi->atten_250 = ui->lineEdit_250_atten->text();
    noi->atten_500 = ui->lineEdit_500_atten->text();
    noi->atten_1k = ui->lineEdit_1k_atten->text();
    noi->atten_2k = ui->lineEdit_2k_atten->text();
    noi->atten_4k = ui->lineEdit_4k_atten->text();
    noi->atten_8k = ui->lineEdit_8k_atten->text();

    noi->refl_63 = ui->lineEdit_63_refl->text();
    noi->refl_125 = ui->lineEdit_125_refl->text();
    noi->refl_250 = ui->lineEdit_250_refl->text();
    noi->refl_500 = ui->lineEdit_500_refl->text();
    noi->refl_1k = ui->lineEdit_1k_refl->text();
    noi->refl_2k = ui->lineEdit_2k_refl->text();
    noi->refl_4k = ui->lineEdit_4k_refl->text();
    noi->refl_8k = ui->lineEdit_8k_refl->text();

    noi->number = noi->model;

    this->accept(); // 关闭对话框
}

void Dialog_air_diff::on_close_clicked()
{
    this->close();
}


void Dialog_air_diff::on_radioButton_circle_clicked()
{
    ui->lineEdit_63_refl->clear();
    ui->lineEdit_125_refl->clear();
    ui->lineEdit_250_refl->clear();
    ui->lineEdit_500_refl->clear();
    ui->lineEdit_1k_refl->clear();
    ui->lineEdit_2k_refl->clear();
    ui->lineEdit_4k_refl->clear();
    ui->lineEdit_8k_refl->clear();

    ui->lineEdit_length->clear();
    ui->lineEdit_width->clear();
    ui->stackedWidget_input->setCurrentWidget(ui->page_circle);
}


void Dialog_air_diff::on_radioButton_rect_clicked()
{
    ui->lineEdit_63_refl->clear();
    ui->lineEdit_125_refl->clear();
    ui->lineEdit_250_refl->clear();
    ui->lineEdit_500_refl->clear();
    ui->lineEdit_1k_refl->clear();
    ui->lineEdit_2k_refl->clear();
    ui->lineEdit_4k_refl->clear();
    ui->lineEdit_8k_refl->clear();

    ui->lineEdit_diameter->clear();
    ui->stackedWidget_input->setCurrentWidget(ui->page_rect);
}


void Dialog_air_diff::on_radioButton_known_clicked()
{
    ui->lineEdit_63_refl->clear();
    ui->lineEdit_125_refl->clear();
    ui->lineEdit_250_refl->clear();
    ui->lineEdit_500_refl->clear();
    ui->lineEdit_1k_refl->clear();
    ui->lineEdit_2k_refl->clear();
    ui->lineEdit_4k_refl->clear();
    ui->lineEdit_8k_refl->clear();

    ui->lineEdit_63_refl->setReadOnly(false);
    ui->lineEdit_125_refl->setReadOnly(false);
    ui->lineEdit_250_refl->setReadOnly(false);
    ui->lineEdit_500_refl->setReadOnly(false);
    ui->lineEdit_1k_refl->setReadOnly(false);
    ui->lineEdit_2k_refl->setReadOnly(false);
    ui->lineEdit_4k_refl->setReadOnly(false);
    ui->lineEdit_8k_refl->setReadOnly(false);
}


void Dialog_air_diff::on_radioButton_formula_clicked()
{
    ui->lineEdit_63_refl->clear();
    ui->lineEdit_125_refl->clear();
    ui->lineEdit_250_refl->clear();
    ui->lineEdit_500_refl->clear();
    ui->lineEdit_1k_refl->clear();
    ui->lineEdit_2k_refl->clear();
    ui->lineEdit_4k_refl->clear();
    ui->lineEdit_8k_refl->clear();

    ui->lineEdit_63_refl->setReadOnly(true);
    ui->lineEdit_125_refl->setReadOnly(true);
    ui->lineEdit_250_refl->setReadOnly(true);
    ui->lineEdit_500_refl->setReadOnly(true);
    ui->lineEdit_1k_refl->setReadOnly(true);
    ui->lineEdit_2k_refl->setReadOnly(true);
    ui->lineEdit_4k_refl->setReadOnly(true);
    ui->lineEdit_8k_refl->setReadOnly(true);
}


void Dialog_air_diff::on_pushButton_clicked()
{
    // 使用资源路径
    QString imagePath = ":/images/image/refl.jpg"; // 替换为你的资源路径
    // 获取或创建 ImageDialog 的实例
    ImageDialog* imageDialog = ImageDialog::getInstance(imagePath, this);
    imageDialog->exec();
}

