#include "inputDialog/dialog_vav_terminal.h"
#include "ui_dialog_vav_terminal.h"
#include <cmath>
#include "calFunction/cal_function.h"

Dialog_VAV_terminal::Dialog_VAV_terminal(QWidget *parent, int editRow,  const VAV_terminal& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_VAV_terminal)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件

    noi_lineEdits = { ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500,
                      ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k,
                      ui->lineEdit_total };

    if(editRow != -1)
    {
        ui->lineEdit_number->setText(data.number);
        ui->lineEdit_brand->setText(data.brand);
        ui->lineEdit_model->setText(data.model);
        ui->comboBox_angle->setCurrentText(data.angle);
        ui->lineEdit_air_volume->setText(data.air_volume);
        ui->comboBox_data_source->setCurrentText(data.data_source);
        table_id = data.table_id;
        UUID = data.UUID;

        for(size_t i = 0; i < noi_lineEdits.size(); i++)
        {
            noi_lineEdits[i]->setText(data.noi[i]);
        }
    }

    //连接lineEdit信号与槽

    for(auto& lineEdit : noi_lineEdits)
    {
        connect(lineEdit, &QLineEdit::textChanged, this, &Dialog_VAV_terminal::calTotalNoise);
    }

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_VAV_terminal::~Dialog_VAV_terminal()
{
    delete ui;
}

//获得noi
void* Dialog_VAV_terminal::getComponent()
{
    return component == nullptr ? nullptr : component;
}

// 槽函数实现
void Dialog_VAV_terminal::calTotalNoise() {
    // 处理进口噪声的计算和显示
    if(std::all_of(noi_lineEdits.begin(), noi_lineEdits.end() - 1, [](QLineEdit* lineEdit)
        { return !lineEdit->text().isEmpty(); }))
    {
        double totalValueIn = calNoiseTotalValue(noi_lineEdits);
        noi_lineEdits[8]->setText(QString::number(totalValueIn, 'f', 1));
    }
}

//关闭窗口按钮
void Dialog_VAV_terminal::on_close_clicked()
{
    this->close();
}

QList<QStringList> Dialog_VAV_terminal::getComponentDataAsStringList() const
{
    QList<QStringList> dataLists;
    QStringList data = {
        component->table_id,
        component->number,
        component->model,
        component->brand,
        component->angle,
        component->air_volume
    };

    // 迭代 noi_out 数组来填充 QStringList
    for (const auto& noi_value : component->noi) {
        data.push_back(noi_value);
    }

    data.push_back(component->data_source);
    data.push_back(component->UUID);

    dataLists.append(data);
    return dataLists;
}

//确认按钮
void Dialog_VAV_terminal::on_pushButton_confirm_clicked()
{
    array<QString,9> noi;

    for(size_t i = 0; i < noi.size(); i++)
    {
        noi[i] = noi_lineEdits[i]->text();
    }

    this->component = new VAV_terminal(
                ui->lineEdit_model->text(),
                ui->lineEdit_brand->text(),
                table_id,
                UUID,
                ui->comboBox_data_source->currentText(),
                ui->comboBox_angle->currentText(),
                ui->lineEdit_air_volume->text(),
                noi,
                ui->lineEdit_number->text());

    this->accept(); // 关闭对话框
}
