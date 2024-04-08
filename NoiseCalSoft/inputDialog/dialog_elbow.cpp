#include "inputDialog/dialog_elbow.h"
#include "ui_dialog_elbow.h"
#include <cmath>

Dialog_elbow::Dialog_elbow(QWidget *parent, int editRow,  const Elbow& data) :
    InputBaseDialog(parent),
    editRow(editRow),
    ui(new Ui::Dialog_elbow)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件

    atten_lineEdits = { ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500,
                      ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k};

    ui->radioButton_circle->setChecked(true);
    ui->radioButton_rect->setChecked(false);

    ui->stackedWidget_input->setCurrentWidget(ui->page_circle);

    if(editRow != -1)
    {
        if(data.elbow_shape == "圆形")
        {
            ui->radioButton_circle->setChecked(true);
            ui->radioButton_rect->setChecked(false);
            ui->lineEdit_diameter->setText(data.size);
        }
        else if(data.elbow_shape == "方形有内衬")
        {
            ui->radioButton_circle->setChecked(false);
            ui->radioButton_rect->setChecked(true);
            ui->radioButton_lined->setChecked(true);
            ui->radioButton_unlined->setChecked(false);
            ui->lineEdit_width->setText(data.size);
        }
        else if(data.elbow_shape == "方形无内衬")
        {
            ui->radioButton_circle->setChecked(false);
            ui->radioButton_rect->setChecked(true);
            ui->radioButton_lined->setChecked(false);
            ui->radioButton_unlined->setChecked(true);
            ui->lineEdit_width->setText(data.size);
        }

        for(size_t i = 0; i < atten_lineEdits.size(); i++)
        {
            atten_lineEdits[i]->setText(data.atten[i]);
        }
    }

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);

}

Dialog_elbow::~Dialog_elbow()
{
    delete ui;
}

void Dialog_elbow::on_close_clicked()
{
    this->close();
}

void* Dialog_elbow::getComponent()
{
    return component == nullptr ? nullptr : component;
}

void Dialog_elbow::on_pushButton_confirm_clicked()
{

    array<QString,8> atten;

    for(size_t i = 0; i < atten.size(); i++)
    {
        atten[i] = atten_lineEdits[i]->text();
    }

    if(ui->radioButton_circle->isChecked())
    {
        elbow_shape = "圆形";
        size = ui->lineEdit_diameter->text();
    }
    else if(ui->radioButton_rect->isChecked())
    {
        elbow_shape = "方形";
        size = ui->lineEdit_width->text();
        if(ui->radioButton_lined->isChecked())
        {
            elbow_shape += "有内衬";
        }
        else if(ui->radioButton_unlined->isChecked())
        {
            elbow_shape += "无内衬";
        }

    }

    /**点击确认前检查，数据是否填完整**/
    if(check_lineedit()==false){
        QMessageBox::information(this,"提示","内容未填写完整");
        return;
    }

    this->component = new Elbow(
                ui->lineEdit_model->text(),
                ui->lineEdit_brand->text(),
                table_id,
                UUID,
                ui->comboBox_data_source->currentText(),
                elbow_shape,
                size,
                atten);

    this->accept(); // 关闭对话框
}

bool Dialog_elbow::check_lineedit()
{
    for(size_t i = 0; i < atten_lineEdits.size(); i++){
        if(atten_lineEdits[i]->text().isEmpty())
            return false;
    }
    if(ui->lineEdit_model->text().isEmpty() ||
       ui->lineEdit_brand->text().isEmpty())
        return false;
    if((elbow_shape == "圆形" && ui->lineEdit_diameter->text().isEmpty()) ||
       (elbow_shape == "方形" && ui->lineEdit_width->text().isEmpty()))
        return false;
    return true;
}

void Dialog_elbow::on_radioButton_circle_clicked()
{
    ui->stackedWidget_input->setCurrentWidget(ui->page_circle);
    ui->lineEdit_63->clear();
    ui->lineEdit_125->clear();
    ui->lineEdit_250->clear();
    ui->lineEdit_500->clear();
    ui->lineEdit_1k->clear();
    ui->lineEdit_2k->clear();
    ui->lineEdit_4k->clear();
    ui->lineEdit_8k->clear();
    ui->lineEdit_width->clear();
}


void Dialog_elbow::on_radioButton_rect_clicked()
{
    ui->stackedWidget_input->setCurrentWidget(ui->page_rect);
    ui->lineEdit_63->clear();
    ui->lineEdit_125->clear();
    ui->lineEdit_250->clear();
    ui->lineEdit_500->clear();
    ui->lineEdit_1k->clear();
    ui->lineEdit_2k->clear();
    ui->lineEdit_4k->clear();
    ui->lineEdit_8k->clear();
    ui->lineEdit_diameter->clear();
}


void Dialog_elbow::on_pushButton_clicked()
{
    // 使用资源路径
    QString imagePath = ":/images/image/elbow_fw_value_table.png"; // 替换为你的资源路径
    // 获取或创建 ImageDialog 的实例
    ImageDialog* imageDialog = ImageDialog::getInstance(imagePath, this);
    imageDialog->updateImage(imagePath);
    imageDialog->exec();
}

QList<QStringList> Dialog_elbow::getComponentDataAsStringList() const
{
    QList<QStringList> dataLists;
    QStringList data = {
        component->table_id,
        component->model,
        component->brand,
        component->elbow_shape,
        component->size
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

