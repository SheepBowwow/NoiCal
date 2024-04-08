#include "inputDialog/dialog_fanCoil.h"
#include "ui_dialog_fanCoil.h"
#include "calFunction/cal_function.h"
#include "Component/ComponentStructs.h"
#include <cmath>

Dialog_fanCoil::Dialog_fanCoil(QWidget *parent, int editRow,  const FanCoil& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_fanCoil)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件

    noi_in_lineEdits = { ui->lineEdit_in_63, ui->lineEdit_in_125, ui->lineEdit_in_250, ui->lineEdit_in_500,
                       ui->lineEdit_in_1k, ui->lineEdit_in_2k, ui->lineEdit_in_4k, ui->lineEdit_in_8k,
                       ui->lineEdit_in_total };

    noi_out_lineEdits = { ui->lineEdit_out_63, ui->lineEdit_out_125, ui->lineEdit_out_250, ui->lineEdit_out_500,
                       ui->lineEdit_out_1k, ui->lineEdit_out_2k, ui->lineEdit_out_4k, ui->lineEdit_out_8k,
                       ui->lineEdit_out_total };

    if(editRow != -1)
    {
        ui->lineEdit_type->setText(data.type);
        ui->lineEdit_brand->setText(data.brand);
        ui->lineEdit_model->setText(data.model);
        ui->lineEdit_air_volume->setText(data.air_volume);
        ui->lineEdit_static_pressure->setText(data.static_pressure);
        table_id = data.table_id;
        UUID = data.UUID;
        for (size_t i = 0; i < data.noi_in.size(); ++i) {
            noi_in_lineEdits[i]->setText(data.noi_in[i]);
        }

        for (size_t i = 0; i < data.noi_out.size(); ++i) {
            noi_out_lineEdits[i]->setText(data.noi_out[i]);
        }
    }

    //连接lineEdit信号与槽
    //进口
    for(auto& lineEdit : noi_in_lineEdits)
    {
        connect(lineEdit, &QLineEdit::textChanged, this, &Dialog_fanCoil::calTotalNoise);
    }
    //出口
    for(auto& lineEdit : noi_out_lineEdits)
    {
        connect(lineEdit, &QLineEdit::textChanged, this, &Dialog_fanCoil::calTotalNoise);
    }

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_fanCoil::~Dialog_fanCoil()
{
    delete ui;
}

//获得noi
void* Dialog_fanCoil::getComponent()
{
    return component == nullptr ? nullptr : component;
}

// 槽函数实现
void Dialog_fanCoil::calTotalNoise() {
    // 处理进口噪声的计算和显示
    if(std::all_of(noi_in_lineEdits.begin(), noi_in_lineEdits.end() - 1, [](QLineEdit* lineEdit)
        { return !lineEdit->text().isEmpty(); }))
    {
        double totalValueIn = calNoiseTotalValue(noi_in_lineEdits);
        noi_in_lineEdits[8]->setText(QString::number(totalValueIn, 'f', 1));
    }

    // 处理出口噪声的计算和显示
    if(std::all_of(noi_out_lineEdits.begin(), noi_out_lineEdits.end() - 1, [](QLineEdit* lineEdit)
        { return !lineEdit->text().isEmpty(); }))
    {
        double totalValueOut = calNoiseTotalValue(noi_out_lineEdits);
        noi_out_lineEdits[8]->setText(QString::number(totalValueOut, 'f', 1));
    }
}

//关闭窗口按钮
void Dialog_fanCoil::on_close_clicked()
{
    this->close();
}

QList<QStringList> Dialog_fanCoil::getComponentDataAsStringList() const
{
    QList<QStringList> dataLists;
    QStringList data_in = {
        component->table_id,
        component->type,
        component->model,
        component->brand,
        component->air_volume,
        component->static_pressure,
        "进口"
    };

    for (const auto& noi_value : component->noi_in) {
        data_in.push_back(noi_value);
    }

    data_in.push_back(component->data_source);
    data_in.push_back(component->UUID);

    QStringList data_out = {
        component->table_id,
        component->type,
        component->model,
        component->brand,
        component->air_volume,
        component->static_pressure,
        "出口"
    };

    // 迭代 noi_out 数组来填充 QStringList
    for (const auto& noi_value : component->noi_out) {
        data_out.push_back(noi_value);
    }

    data_out.push_back(component->data_source);
    data_out.push_back(component->UUID);

    dataLists.append(data_in);
    dataLists.append(data_out);

    return dataLists;
}

//确认按钮
void Dialog_fanCoil::on_pushButton_confirm_clicked()
{
    array<QString,9> noi_in;
    array<QString,9> noi_out;

    for(size_t i = 0; i < noi_in.size(); i++)
    {
        noi_in[i] = noi_in_lineEdits[i]->text();
        noi_out[i] = noi_out_lineEdits[i]->text();
    }

    /**点击确认前检查，数据是否填完整**/
    if(check_lineedit()==false){
        QMessageBox::information(this,"提示","内容未填写完整");
        return;
    }

    this->component = new FanCoil(
                ui->lineEdit_model->text(),
                ui->lineEdit_brand->text(),
                table_id,
                UUID,
                ui->comboBox_data_source->currentText(),
                ui->lineEdit_air_volume->text(),
                ui->lineEdit_static_pressure->text(),
                ui->lineEdit_type->text(),
                noi_in,
                noi_out);

    this->accept(); // 关闭对话框
}

bool Dialog_fanCoil::check_lineedit()
{
    for(size_t i = 0; i < noi_in_lineEdits.size(); i++){
        if(noi_in_lineEdits[i]->text().isEmpty()    ||
           noi_out_lineEdits[i]->text().isEmpty())
            return false;
    }
    if(ui->lineEdit_model->text().isEmpty() ||
        ui->lineEdit_brand->text().isEmpty()||
        ui->lineEdit_air_volume->text().isEmpty()||
        ui->lineEdit_static_pressure->text().isEmpty()||
        ui->lineEdit_type->text().isEmpty())
        return false;
    return true;
}
