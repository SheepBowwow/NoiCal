#include "inputDialog/dialog_aircondition.h"
#include "ui_dialog_aircondition.h"
#include "calFunction/cal_function.h"
#include <cmath>

Dialog_aircondition::Dialog_aircondition(QWidget *parent, int editRow, int fan_counts, const Aircondition& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    fan_counts(fan_counts),
    ui(new Ui::Dialog_aircondition)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件

    noi_send_in_lineEdits = { ui->lineEdit_send_in_63, ui->lineEdit_send_in_125, ui->lineEdit_send_in_250, ui->lineEdit_send_in_500,
                       ui->lineEdit_send_in_1k, ui->lineEdit_send_in_2k, ui->lineEdit_send_in_4k, ui->lineEdit_send_in_8k,
                       ui->lineEdit_send_in_total };

    noi_send_out_lineEdits = { ui->lineEdit_send_out_63, ui->lineEdit_send_out_125, ui->lineEdit_send_out_250, ui->lineEdit_send_out_500,
                       ui->lineEdit_send_out_1k, ui->lineEdit_send_out_2k, ui->lineEdit_send_out_4k, ui->lineEdit_send_out_8k,
                       ui->lineEdit_send_out_total };

    noi_exhaust_in_lineEdits = { ui->lineEdit_exhaust_in_63, ui->lineEdit_exhaust_in_125, ui->lineEdit_exhaust_in_250, ui->lineEdit_exhaust_in_500,
                       ui->lineEdit_exhaust_in_1k, ui->lineEdit_exhaust_in_2k, ui->lineEdit_exhaust_in_4k, ui->lineEdit_exhaust_in_8k,
                       ui->lineEdit_exhaust_in_total };

    noi_exhaust_out_lineEdits = { ui->lineEdit_exhaust_out_63, ui->lineEdit_exhaust_out_125, ui->lineEdit_exhaust_out_250, ui->lineEdit_exhaust_out_500,
                       ui->lineEdit_exhaust_out_1k, ui->lineEdit_exhaust_out_2k, ui->lineEdit_exhaust_out_4k, ui->lineEdit_exhaust_out_8k,
                       ui->lineEdit_exhaust_out_total };

    if(fan_counts == 1)
    {
        ui->label_title->setText("空调器(单风机)");
        ui->label_title->adjustSize();

        ui->widget_exhaust->hide();
        ui->pushButton_confirm->move(ui->pushButton_confirm->x(),ui->pushButton_confirm->y() - ui->widget_exhaust->height());
        ui->widget_add_air_noise->setFixedHeight(ui->widget_add_air_noise->height() - ui->widget_exhaust->height());
        ui->widget_2->setFixedHeight(ui->widget_2->height() - ui->widget_exhaust->height());
        this->setFixedHeight(this->height() - ui->widget_exhaust->height());
    }
    else if(fan_counts == 2)
    {
        ui->label_title->setText("空调器(双风机)");
        ui->label_title->adjustSize();
    }

    if(editRow != -1)
    {
        ui->lineEdit_model->setText(data.model);
        ui->lineEdit_brand->setText(data.brand);
        ui->comboBox_data_source->setCurrentText(data.data_source);

        ui->lineEdit_send_number->setText(data.send_number);
        ui->lineEdit_send_air_volume->setText(data.send_air_volume);
        ui->lineEdit_send_static_pressure->setText(data.send_static_pressure);

        table_id = data.table_id;
        UUID = data.UUID;

        for (size_t i = 0; i < data.noi_send_in.size(); ++i) {
            noi_send_in_lineEdits[i]->setText(data.noi_send_in[i]);
        }

        for (size_t i = 0; i < data.noi_send_out.size(); ++i) {
            noi_send_out_lineEdits[i]->setText(data.noi_send_out[i]);
        }

        if(data.fan_counts == 2)
        {
            ui->lineEdit_exhaust_number->setText(data.exhaust_number);
            ui->lineEdit_exhaust_air_volume->setText(data.exhaust_air_volume);
            ui->lineEdit_exhaust_static_pressure->setText(data.exhaust_static_pressure);

            for (size_t i = 0; i < data.noi_exhaust_in.size(); ++i) {
                noi_exhaust_in_lineEdits[i]->setText(data.noi_exhaust_in[i]);
            }

            for (size_t i = 0; i < data.noi_exhaust_out.size(); ++i) {
                noi_exhaust_out_lineEdits[i]->setText(data.noi_exhaust_out[i]);
            }
        }
    }

    //连接lineEdit信号与槽
    //进口
    for(auto& lineEdit : noi_send_in_lineEdits)
    {
        connect(lineEdit, &QLineEdit::textChanged, this, &Dialog_aircondition::calTotalNoise);
    }
    //出口
    for(auto& lineEdit : noi_send_out_lineEdits)
    {
        connect(lineEdit, &QLineEdit::textChanged, this, &Dialog_aircondition::calTotalNoise);
    }
    //进口
    for(auto& lineEdit : noi_exhaust_in_lineEdits)
    {
        connect(lineEdit, &QLineEdit::textChanged, this, &Dialog_aircondition::calTotalNoise);
    }
    //出口
    for(auto& lineEdit : noi_exhaust_out_lineEdits)
    {
        connect(lineEdit, &QLineEdit::textChanged, this, &Dialog_aircondition::calTotalNoise);
    }

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_aircondition::~Dialog_aircondition()
{
    delete ui;
}

void Dialog_aircondition::on_pushButton_confirm_clicked()
{

    array<QString,9> noi_send_in;
    array<QString,9> noi_send_out;

    for(size_t i = 0; i < noi_send_in.size(); i++)
    {
        noi_send_in[i] = noi_send_in_lineEdits[i]->text().trimmed();
        noi_send_out[i] = noi_send_out_lineEdits[i]->text().trimmed();
    }
    if(fan_counts == 2)
    {
        array<QString,9> noi_exhaust_in;
        array<QString,9> noi_exhaust_out;

        for(size_t i = 0; i < noi_exhaust_in.size(); i++)
        {
            noi_exhaust_in[i] = noi_exhaust_in_lineEdits[i]->text().trimmed();
            noi_exhaust_out[i] = noi_exhaust_out_lineEdits[i]->text().trimmed();
        }

        /**点击确认前检查，数据是否填完整**/
        if(check_lineedit(fan_counts)==false){
            QMessageBox::information(this,"提示","内容未填写完整");
            return;
        }

        this->component = new Aircondition(
                    ui->lineEdit_model->text().trimmed(),
                    ui->lineEdit_brand->text().trimmed(),
                    table_id,
                    UUID,
                    ui->comboBox_data_source->currentText(),
                    fan_counts,
                    ui->lineEdit_send_number->text().trimmed(),
                    ui->lineEdit_send_air_volume->text().trimmed(),
                    ui->lineEdit_send_static_pressure->text().trimmed(),
                    ui->lineEdit_exhaust_number->text().trimmed(),
                    ui->lineEdit_exhaust_air_volume->text().trimmed(),
                    ui->lineEdit_exhaust_static_pressure->text().trimmed(),
                    noi_send_in,
                    noi_send_out,
                    noi_exhaust_in,
                    noi_exhaust_out);
    }
    else
    {
        /**点击确认前检查，数据是否填完整**/
        if(check_lineedit(fan_counts)==false){
            QMessageBox::information(this,"提示","内容未填写完整");
            return;
        }
        this->component = new Aircondition(
                    ui->lineEdit_model->text().trimmed(),
                    ui->lineEdit_brand->text().trimmed(),
                    table_id,
                    UUID,
                    ui->comboBox_data_source->currentText(),
                    fan_counts,
                    ui->lineEdit_send_number->text().trimmed(),
                    ui->lineEdit_send_air_volume->text().trimmed(),
                    ui->lineEdit_send_static_pressure->text().trimmed(),
                    noi_send_in,
                    noi_send_out);
    }


    this->accept(); // 关闭对话框
}

bool Dialog_aircondition::check_lineedit(int counts)
{
    if(ui->lineEdit_model->text().isEmpty()             ||
       ui->lineEdit_brand->text().isEmpty()             ||
       ui->lineEdit_send_number->text().isEmpty()       ||
       ui->lineEdit_send_air_volume->text().isEmpty()   ||
       ui->lineEdit_send_static_pressure->text().isEmpty()
       )return false;

    for(size_t i = 0; i < noi_send_in_lineEdits.size(); i++){
        if(noi_send_in_lineEdits[i]->text().isEmpty()       ||
           noi_send_out_lineEdits[i]->text().isEmpty())
            return false;
    }

    if(counts == 2){
        for(size_t i = 0; i < noi_exhaust_in_lineEdits.size(); i++){
            if(noi_exhaust_in_lineEdits[i]->text().isEmpty() ||
            noi_exhaust_out_lineEdits[i]->text().isEmpty())
                return false;
        }
        if(ui->lineEdit_exhaust_number->text().isEmpty()    ||
           ui->lineEdit_exhaust_air_volume->text().isEmpty() ||
           ui->lineEdit_exhaust_static_pressure->text().isEmpty()
          )return false;
    }

    return true;
}

void* Dialog_aircondition::getComponent()
{
    return component == nullptr ? nullptr : component;
}

void Dialog_aircondition::switchToCompontDB(bool inDB)
{
    if(inDB)
    {
        ui->lineEdit_send_number->setText("-");
        ui->widget_send_number->hide();
        ui->widget_send_other->move(QPoint(ui->widget_send_number->x(), ui->widget_send_number->y()));

        ui->lineEdit_exhaust_number->setText("-");
        ui->widget_exhaust_number->hide();
        ui->widget_exhaust_other->move(QPoint(ui->widget_exhaust_number->x(), ui->widget_exhaust_number->y()));
    }
}

void Dialog_aircondition::on_close_clicked()
{
    this->close();
}

QList<QStringList> Dialog_aircondition::getComponentDataAsStringList() const
{
    QList<QStringList> dataLists;

    QStringList data_send_in = {
        component->table_id,
        component->send_number,
        component->model,
        component->brand,
        component->send_air_volume,
        component->send_static_pressure,
        "送风机",
        "进口",
    };

    for (const auto& noi_value : component->noi_send_in) {
        data_send_in.push_back(noi_value);
    }

    data_send_in.push_back(component->data_source);
    data_send_in.push_back(component->UUID);

    QStringList data_send_out = {
        component->table_id,
        component->send_number,
        component->model,
        component->brand,
        component->send_air_volume,
        component->send_static_pressure,
        "送风机",
        "出口",
    };

    for (const auto& noi_value : component->noi_send_out) {
        data_send_out.push_back(noi_value);
    }

    data_send_out.push_back(component->data_source);
    data_send_out.push_back(component->UUID);

    dataLists.append(data_send_in);
    dataLists.append(data_send_out);

    if(fan_counts == 2)
    {
        QStringList data_exhaust_in = {
            component->table_id,
            component->exhaust_number,
            component->model,
            component->brand,
            component->exhaust_air_volume,
            component->exhaust_static_pressure,
            "排风机",
            "进口",
        };

        for (const auto& noi_value : component->noi_exhaust_in) {
            data_exhaust_in.push_back(noi_value);
        }

        data_exhaust_in.push_back(component->data_source);
        data_exhaust_in.push_back(component->UUID);

        QStringList data_exhaust_out = {
            component->table_id,
            component->exhaust_number,
            component->model,
            component->brand,
            component->exhaust_air_volume,
            component->exhaust_static_pressure,
            "排风机",
            "出口",
        };

        for (const auto& noi_value : component->noi_exhaust_out) {
            data_exhaust_out.push_back(noi_value);
        }

        data_exhaust_out.push_back(component->data_source);
        data_exhaust_out.push_back(component->UUID);

        dataLists.append(data_exhaust_in);
        dataLists.append(data_exhaust_out);
    }

    return dataLists;
}

//计算总值 槽函数实现
void Dialog_aircondition::calTotalNoise() {
    // 处理送风机进口噪声的计算和显示
    if(std::all_of(noi_send_in_lineEdits.begin(), noi_send_in_lineEdits.end() - 1, [](QLineEdit* lineEdit)
        { return !lineEdit->text().isEmpty(); }))
    {
        double totalValueIn = calNoiseTotalValue(noi_send_in_lineEdits);
        noi_send_in_lineEdits[8]->setText(QString::number(totalValueIn, 'f', 1));
    }

    // 处理送风机出口噪声的计算和显示
    if(std::all_of(noi_send_out_lineEdits.begin(), noi_send_out_lineEdits.end() - 1, [](QLineEdit* lineEdit)
        { return !lineEdit->text().isEmpty(); }))
    {
        double totalValueOut = calNoiseTotalValue(noi_send_out_lineEdits);
        noi_send_out_lineEdits[8]->setText(QString::number(totalValueOut, 'f', 1));
    }

    if(fan_counts == 1)
        return;

    // 处理排风机进口噪声的计算和显示
    if(std::all_of(noi_exhaust_in_lineEdits.begin(), noi_exhaust_in_lineEdits.end() - 1, [](QLineEdit* lineEdit)
        { return !lineEdit->text().isEmpty(); }))
    {
        double totalValueIn = calNoiseTotalValue(noi_exhaust_in_lineEdits);
        noi_exhaust_in_lineEdits[8]->setText(QString::number(totalValueIn, 'f', 1));
    }

    // 处理排风机出口噪声的计算和显示
    if(std::all_of(noi_exhaust_out_lineEdits.begin(), noi_exhaust_out_lineEdits.end() - 1, [](QLineEdit* lineEdit)
        { return !lineEdit->text().isEmpty(); }))
    {
        double totalValueOut = calNoiseTotalValue(noi_exhaust_out_lineEdits);
        noi_exhaust_out_lineEdits[8]->setText(QString::number(totalValueOut, 'f', 1));
    }
}

