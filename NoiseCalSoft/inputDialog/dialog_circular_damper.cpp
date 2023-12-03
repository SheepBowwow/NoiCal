#include "inputDialog/Dialog_circular_damper.h"
#include "ui_dialog_circular_damper.h"
#include<cmath>
#define Pi 3.14159265358979323846

Dialog_circular_damper::Dialog_circular_damper(QWidget *parent, int editRow, const Circular_damper_noi& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_circular_damper)
{
    ui->setupUi(this);

    setTopWidget(ui->widget_top);  // 设置顶部部件

    this->fixNumber[0] = { -4, -5, -5, -9, -14, -19, -24, -29};
    this->fixNumber[45] = { -7, -5, -6, -9, -13, -12, -7, -13};
    this->fixNumber[65] = { -10, -7, -4, -5, -9, 0, -3, -10};

    ui->comboBox_angle->addItem("0°");
    ui->comboBox_angle->addItem("45°");
    ui->comboBox_angle->addItem("65°");
    //设置当前选择为空
    ui->comboBox_angle->setCurrentIndex(-1);

    ui->radioButton_formula->setChecked(true);
    ui->radioButton_known->setChecked(false);
    ui->stackedWidget_input->setCurrentWidget(ui->page_formula);
    ui->stackedWidget_descri->setCurrentWidget(ui->page_descri);

    if(editRow != -1)
    {
        QString diameter = "";
        for(int i = 0; i < data.model.length(); i++)
        {
            if(data.model[i] >= '0' && data.model[i] <= '9')
            {
                diameter += data.model[i];
            }
        }

        ui->lineEdit_brand->setText(data.brand);
        ui->lineEdit_air_volume->setText(data.air_volume);
        ui->comboBox_angle->setCurrentText(data.angle);
        ui->lineEdit_63->setText(data.noi_63);
        ui->lineEdit_125->setText(data.noi_125);
        ui->lineEdit_250->setText(data.noi_250);
        ui->lineEdit_500->setText(data.noi_500);
        ui->lineEdit_1k->setText(data.noi_1k);
        ui->lineEdit_2k->setText(data.noi_2k);
        ui->lineEdit_4k->setText(data.noi_4k);
        ui->lineEdit_8k->setText(data.noi_8k);
        ui->lineEdit_total->setText(data.noi_total);
        if(data.getMode == "厂家")
        {
            ui->radioButton_formula->setChecked(false);
            ui->radioButton_known->setChecked(true);
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
            ui->lineEdit_diameter->setText(diameter);
        }
    }

    //连接lineEdit信号与槽
    connect(ui->lineEdit_diameter, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->lineEdit_air_volume, SIGNAL(textChanged(QString)), this, SLOT(calNoise()));
    connect(ui->comboBox_angle, SIGNAL(currentTextChanged(QString)), this, SLOT(calNoise()));

    connect(ui->lineEdit_63, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_125, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_250, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_500, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_1k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_2k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_4k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_8k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_circular_damper::~Dialog_circular_damper()
{
    delete ui;
}

void* Dialog_circular_damper::getNoi()
{
    return noi == nullptr ? nullptr : noi;
}

void Dialog_circular_damper::calNoise()
{
    if(ui->lineEdit_diameter->text().isEmpty() || ui->lineEdit_air_volume->text().isEmpty() || ui->comboBox_angle->currentIndex() == -1)
    {
        return;
    }
    int L = 0;
    if(ui->comboBox_angle->currentText() == "0°") L = 30;
    else if(ui->comboBox_angle->currentText() == "45°") L = 42;
    else if(ui->comboBox_angle->currentText() == "65°") L = 51;

    double area = Pi * (ui->lineEdit_diameter->text().toDouble()/100.0 / 2) * (ui->lineEdit_diameter->text().toDouble()/100.0 / 2);
    area = floor(area * 100) / 100;

    // 使用std::reference_wrapper存储这两个对象的引用
    QLineEdit* groupNoiseEdit[] = { ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500, ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k};
    for(int i = 0; i < 8; i++)
    {
        double result = L + 10 * log10(area)
                + 55 * log10(ui->lineEdit_air_volume->text().toDouble() / area / 3600) + fixNumber[ui->comboBox_angle->currentText().toInt()][i];
        groupNoiseEdit[i]->setText(QString::number(result,'f',1));
    }
}

void Dialog_circular_damper::calTotalNoise() {
    // 检查所有八个QLineEdit是否都有数据
    if(!(ui->lineEdit_63->text().isEmpty() || ui->lineEdit_125->text().isEmpty()
       || ui->lineEdit_250->text().isEmpty() || ui->lineEdit_500->text().isEmpty() ||
            ui->lineEdit_1k->text().isEmpty() || ui->lineEdit_2k->text().isEmpty() ||
            ui->lineEdit_4k->text().isEmpty() || ui->lineEdit_8k->text().isEmpty())) {
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
}

void Dialog_circular_damper::on_close_clicked()
{
    this->close();
}


void Dialog_circular_damper::on_pushButton_confirm_clicked()
{
    this->noi = new Circular_damper_noi;

    // 获取对应行的数据，将界面上的数据保存到对应行中
    noi->brand = ui->lineEdit_brand->text();
    noi->angle = ui->comboBox_angle->currentText();
    noi->air_volume = ui->lineEdit_air_volume->text();
    noi->noi_63 = ui->lineEdit_63->text();
    noi->noi_125 = ui->lineEdit_125->text();
    noi->noi_250 = ui->lineEdit_250->text();
    noi->noi_500 = ui->lineEdit_500->text();
    noi->noi_1k = ui->lineEdit_1k->text();
    noi->noi_2k = ui->lineEdit_2k->text();
    noi->noi_4k = ui->lineEdit_4k->text();
    noi->noi_8k = ui->lineEdit_8k->text();
    noi->noi_total = ui->lineEdit_total->text();
    if(ui->radioButton_formula->isChecked())
    {
        noi->getMode = "公式";
        noi->diameter = ui->lineEdit_diameter->text();
        noi->model = "DN" + noi->diameter;
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
void Dialog_circular_damper::on_radioButton_known_clicked()
{
    ui->lineEdit_diameter->clear();
    ui->lineEdit_63->clear();
    ui->lineEdit_125->clear();
    ui->lineEdit_250->clear();
    ui->lineEdit_500->clear();
    ui->lineEdit_1k->clear();
    ui->lineEdit_2k->clear();
    ui->lineEdit_4k->clear();
    ui->lineEdit_8k->clear();
    ui->lineEdit_total->clear();

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
void Dialog_circular_damper::on_radioButton_formula_clicked()
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
    ui->lineEdit_total->clear();

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

