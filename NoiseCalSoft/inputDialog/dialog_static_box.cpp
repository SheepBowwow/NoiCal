#include "inputDialog/dialog_static_box.h"
#include "ui_dialog_static_box.h"
#include <cmath>

Dialog_static_box::Dialog_static_box(QWidget *parent, int editRow,  const Static_box& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_static_box)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件

    atten_lineEdits = { ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500,
                      ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k};

    if(editRow != -1)
    {
        ui->lineEdit_brand->setText(data.brand);
        ui->lineEdit_model->setText(data.model);
        ui->lineEdit_q->setText(data.q);
        ui->lineEdit_q1->setText(data.q1);
        ui->comboBox_data_source->setCurrentText(data.data_source);
        table_id = data.table_id;
        UUID = data.UUID;

        for(size_t i = 0; i < atten_lineEdits.size(); i++)
        {
            atten_lineEdits[i]->setText(data.atten[i]);
        }
    }

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_static_box::~Dialog_static_box()
{
    delete ui;
}


//获得noi
void* Dialog_static_box::getComponent()
{
    return component == nullptr ? nullptr : component;
}


//关闭窗口按钮
void Dialog_static_box::on_close_clicked()
{
    this->close();
}

QList<QStringList> Dialog_static_box::getComponentDataAsStringList() const
{
    QList<QStringList> dataLists;
    QStringList data = {
        component->table_id,
        component->model,
        component->brand,
        component->q,
        component->q1
    };

    // 迭代 noi_out 数组来填充 QStringList
    for (const auto& noi_value : component->atten) {
        data.push_back(noi_value);
    }

    data.push_back(component->data_source);
    data.push_back(component->UUID);

    dataLists.append(data);
    return dataLists;
}

//确认按钮
void Dialog_static_box::on_pushButton_confirm_clicked()
{
    array<QString,8> atten;

    for(size_t i = 0; i < atten.size(); i++)
    {
        atten[i] = atten_lineEdits[i]->text();
    }

    this->component = new Static_box(
                ui->lineEdit_model->text(),
                ui->lineEdit_brand->text(),
                table_id,
                UUID,
                ui->comboBox_data_source->currentText(),
                ui->lineEdit_q1->text(),
                ui->lineEdit_q->text(),
                atten);
    this->accept(); // 关闭对话框
}
