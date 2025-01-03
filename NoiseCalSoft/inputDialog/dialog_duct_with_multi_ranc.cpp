#include "inputDialog/dialog_duct_with_multi_ranc.h"
#include "ui_dialog_duct_with_multi_ranc.h"
#include <cmath>

Dialog_duct_with_multi_ranc::Dialog_duct_with_multi_ranc(QWidget *parent, int editRow,  const Multi_ranc& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_duct_with_multi_ranc)
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


Dialog_duct_with_multi_ranc::~Dialog_duct_with_multi_ranc()
{
    delete ui;
}

void Dialog_duct_with_multi_ranc::on_close_clicked()
{
    this->close();
}

void* Dialog_duct_with_multi_ranc::getComponent()
{
    return component == nullptr ? nullptr : component;
}

//点击确认键
void Dialog_duct_with_multi_ranc::on_pushButton_confirm_clicked()
{
    array<QString,8> atten;

    for(size_t i = 0; i < atten.size(); i++)
    {
        atten[i] = atten_lineEdits[i]->text().trimmed();
    }

    /**点击确认前检查，数据是否填完整**/
    if(check_lineedit()==false){
        QMessageBox::information(this,"提示","内容未填写完整");
        return;
    }

    this->component = new Multi_ranc(
                ui->lineEdit_model->text().trimmed(),
                ui->lineEdit_brand->text().trimmed(),
                table_id,
                UUID,
                ui->comboBox_data_source->currentText(),
                ui->lineEdit_q1->text().trimmed(),
                ui->lineEdit_q->text().trimmed(),
                atten);
    this->accept(); // 关闭对话框
}

bool Dialog_duct_with_multi_ranc::check_lineedit()
{
    for(size_t i = 0; i < atten_lineEdits.size(); i++){
        if(atten_lineEdits[i]->text().isEmpty())
            return false;
    }
    if(ui->lineEdit_model->text().isEmpty() ||
       ui->lineEdit_brand->text().isEmpty() ||
       ui->lineEdit_q1->text().isEmpty()    ||
       ui->lineEdit_q->text().isEmpty()
       )return false;

    return true;
}

QList<QStringList> Dialog_duct_with_multi_ranc::getComponentDataAsStringList() const
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
