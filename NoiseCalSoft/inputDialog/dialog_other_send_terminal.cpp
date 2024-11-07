#include "inputDialog/dialog_other_send_terminal.h"
#include "ui_dialog_other_send_terminal.h"
#include <cmath>
#include "calFunction/cal_function.h"

Dialog_other_send_terminal::Dialog_other_send_terminal(QWidget *parent, int editRow,  const Other_send_terminal& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_other_send_terminal)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件

    ui->radioButton_circle->setChecked(true);
    ui->stackedWidget_input->setCurrentWidget(ui->page_circle);

    noi_lineEdits = { ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500,
                       ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k,
                       ui->lineEdit_total };

    atten_lineEdits = { ui->lineEdit_63_atten, ui->lineEdit_125_atten, ui->lineEdit_250_atten, ui->lineEdit_500_atten,
                       ui->lineEdit_1k_atten, ui->lineEdit_2k_atten, ui->lineEdit_4k_atten, ui->lineEdit_8k_atten };

    refl_lineEdits = { ui->lineEdit_63_refl, ui->lineEdit_125_refl, ui->lineEdit_250_refl, ui->lineEdit_500_refl,
                       ui->lineEdit_1k_refl, ui->lineEdit_2k_refl, ui->lineEdit_4k_refl, ui->lineEdit_8k_refl };

    if(editRow != -1)
    {
        QString diameter = "";
        QString length = "";
        QString width = "";
        if(data.terminal_shape == "圆形")
        {
            diameter = data.terminal_size;
            ui->radioButton_circle->setChecked(true);
            ui->lineEdit_diameter->setText(diameter);
            ui->stackedWidget_input->setCurrentWidget(ui->page_circle);
        }
        else if(data.terminal_shape == "方形")
        {
            ui->radioButton_rect->setChecked(true);
            QStringList parts = data.terminal_size.split('x'); // 使用split函数按照斜杠分割字符串
            // parts中的第一个元素是前缀，第二个元素是后缀（如果有的话）
            length = parts.value(0).trimmed(); // 去除前缀两边的空格
            width = parts.value(1).trimmed(); // 去除后缀两边的空格
            ui->lineEdit_length->setText(length);
            ui->lineEdit_width->setText(width);
            ui->stackedWidget_input->setCurrentWidget(ui->page_rect);
        }
        ui->lineEdit_brand->setText(data.brand);
        ui->lineEdit_model->setText(data.model);
        ui->comboBox_noi_data_source->setCurrentText(data.noi_data_source);
        ui->comboBox_atten_data_source->setCurrentText(data.atten_data_source);
        ui->comboBox_refl_data_source->setCurrentText(data.refl_data_source);
        ui->lineEdit_remark->setText(data.remark);
        table_id = data.table_id;
        UUID = data.UUID;

        for (size_t i = 0; i < data.noi.size(); ++i) {
            noi_lineEdits[i]->setText(data.noi[i]);
        }

        for (size_t i = 0; i < data.atten.size(); ++i) {
            atten_lineEdits[i]->setText(data.atten[i]);
        }

        for (size_t i = 0; i < data.refl.size(); ++i) {
            refl_lineEdits[i]->setText(data.refl[i]);
        }
    }


    //连接lineEdit信号与槽
    for(auto& lineEdit : noi_lineEdits)
    {
        connect(lineEdit, &QLineEdit::textChanged, this, &Dialog_other_send_terminal::calTotalNoise);
    }

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_other_send_terminal::~Dialog_other_send_terminal()
{
    delete ui;
}


//获得noi
void* Dialog_other_send_terminal::getComponent()
{
    return component == nullptr ? nullptr : component;
}

// 槽函数实现
void Dialog_other_send_terminal::calTotalNoise() {
    // 处理进口噪声的计算和显示
    if(std::all_of(noi_lineEdits.begin(), noi_lineEdits.end() - 1, [](QLineEdit* lineEdit)
        { return !lineEdit->text().isEmpty(); }))
    {
        double totalValueIn = calNoiseTotalValue(noi_lineEdits);
        noi_lineEdits[8]->setText(QString::number(totalValueIn, 'f', 1));
    }
}

//关闭窗口按钮
void Dialog_other_send_terminal::on_close_clicked()
{
    this->close();
}

//确认按钮
void Dialog_other_send_terminal::on_pushButton_confirm_clicked()
{
    array<QString,9> noi;
    array<QString,8> atten;
    array<QString,8> refl;

    for(size_t i = 0; i < noi.size(); i++)
    {
        noi[i] = noi_lineEdits[i]->text().trimmed();
    }

    for(size_t i = 0; i < atten.size(); i++)
    {
        atten[i] = atten_lineEdits[i]->text().trimmed();
    }

    for(size_t i = 0; i < refl.size(); i++)
    {
        refl[i] = refl_lineEdits[i]->text().trimmed();
    }


    if(ui->radioButton_circle->isChecked())
    {
        size = ui->lineEdit_diameter->text().trimmed();
        shape = "圆形";
    }
    if(ui->radioButton_rect->isChecked())
    {
        size = ui->lineEdit_length->text().trimmed() + "x" + ui->lineEdit_width->text().trimmed();
        shape = "方形";
    }

    /**点击确认前检查，数据是否填完整**/
    if(check_lineedit()==false){
        QMessageBox::information(this,"提示","内容未填写完整");
        return;
    }

    this->component = new Other_send_terminal(
                ui->lineEdit_model->text().trimmed(),
                ui->lineEdit_brand->text().trimmed(),
                table_id,
                UUID,
                ui->comboBox_noi_data_source->currentText(),
                ui->comboBox_atten_data_source->currentText(),
                ui->comboBox_refl_data_source->currentText(),
                shape,
                size,
                noi,
                atten,
                refl,
                ui->lineEdit_remark->text().trimmed());


    this->accept(); // 关闭对话框
}

bool Dialog_other_send_terminal::check_lineedit()
{
    for(size_t i = 0; i < noi_lineEdits.size(); i++){
        if(noi_lineEdits[i]->text().isEmpty())
            return false;
    }
    for(size_t i = 0; i < atten_lineEdits.size(); i++){
        if(atten_lineEdits[i]->text().isEmpty())
            return false;
    }
    for(size_t i = 0; i < refl_lineEdits.size(); i++){
        if(refl_lineEdits[i]->text().isEmpty())
            return false;
    }
    if(ui->lineEdit_model->text().isEmpty()||
       ui->lineEdit_brand->text().isEmpty())
        return false;

    if((shape == "圆形" && ui->lineEdit_diameter->text().isEmpty())||
       (shape == "方形" && (ui->lineEdit_length->text().isEmpty()  ||
         ui->lineEdit_width->text().isEmpty()))
       )return false;

    return true;
}

void Dialog_other_send_terminal::on_radioButton_circle_clicked()
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


void Dialog_other_send_terminal::on_radioButton_rect_clicked()
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

QList<QStringList> Dialog_other_send_terminal::getComponentDataAsStringList() const
{
    QList<QStringList> dataLists;
    QStringList data_noise = {
        component->table_id,
        component->model,
        component->brand,
        component->terminal_shape,
        component->terminal_size
    };

    // 迭代 noi_out 数组来填充 QStringList
    for (const auto& noi_value : component->noi) {
        data_noise.push_back(noi_value);
    }

    data_noise.push_back(component->noi_data_source);
    data_noise.push_back(component->remark);
    data_noise.push_back(component->UUID);


    QStringList data_atten = {
        component->table_id,
        component->model,
        component->brand,
        component->terminal_shape,
        component->terminal_size
    };

    // 迭代 noi_out 数组来填充 QStringList
    for (const auto& noi_value : component->atten) {
        data_atten.push_back(noi_value);
    }

    data_atten.push_back(component->atten_data_source);
    data_atten.push_back(component->remark);
    data_atten.push_back(component->UUID);

    QStringList data_refl = {
        component->table_id,
        component->model,
        component->brand,
        component->terminal_shape,
        component->terminal_size
    };

    // 迭代 noi_out 数组来填充 QStringList
    for (const auto& noi_value : component->refl) {
        data_refl.push_back(noi_value);
    }

    data_refl.push_back(component->refl_data_source);
    data_refl.push_back(component->remark);
    data_refl.push_back(component->UUID);

    dataLists.append(data_noise);
    dataLists.append(data_atten);
    dataLists.append(data_refl);

    return dataLists;
}



void Dialog_other_send_terminal::on_comboBox_refl_data_source_currentIndexChanged(int index)
{
    if(index != 2)
    {
        for(auto& lineEdit : refl_lineEdits)
        {
            lineEdit->clear();
            lineEdit->setReadOnly(false);
        }
    }
    else
    {
        for(auto& lineEdit : refl_lineEdits)
        {
            lineEdit->clear();
            lineEdit->setText("-");
            lineEdit->setReadOnly(true);
        }
    }
}

