#include "inputDialog/dialog_reducer.h"
#include "ui_dialog_reducer.h"
#include <cmath>
#define Pi 3.14159265358979323846

Dialog_reducer::Dialog_reducer(QWidget *parent, int editRow,  const Reducer& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_reducer)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件

    ui->radioButton_circle_to_circle->setChecked(true);

    atten_lineEdits = { ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500,
                      ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k};

    if(editRow != -1)
    {
        ui->lineEdit_model->setText(data.model);
        ui->lineEdit_brand->setText(data.brand);
        ui->comboBox_data_source->setCurrentText(data.data_source);
        table_id = data.table_id;
        UUID = data.UUID;


        if(data.reducer_type == "圆-圆")
        {
            ui->radioButton_circle_to_circle->setChecked(true);
            ui->stackedWidget_reducer_before->setCurrentWidget(ui->page_reducer_before_circle);
            ui->stackedWidget_reducer_after->setCurrentWidget(ui->page_reducer_after_circle);

            ui->lineEdit_before_diameter->setText(data.reducer_before_size);
            ui->lineEdit_after_diameter->setText(data.reducer_after_size);
        }
        else if(data.reducer_type == "方-方")
        {
            ui->radioButton_rect_to_rect->setChecked(true);
            ui->stackedWidget_reducer_before->setCurrentWidget(ui->page_reducer_before_rect);
            ui->stackedWidget_reducer_after->setCurrentWidget(ui->page_reducer_after_rect);

            QStringList parts_before = data.reducer_before_size.split('x', Qt::SkipEmptyParts); // 分割字符串
            QString before_length = parts_before.value(0).trimmed(); // 获取长度并去除空格
            QString before_width = parts_before.value(1).trimmed(); // 获取宽度并去除空格

            QStringList parts_after = data.reducer_after_size.split('x', Qt::SkipEmptyParts); // 分割字符串
            QString after_length = parts_after.value(0).trimmed(); // 获取长度并去除空格
            QString after_width = parts_after.value(1).trimmed(); // 获取宽度并去除空格


            ui->lineEdit_before_length->setText(before_length);
            ui->lineEdit_before_width->setText(before_width);
            ui->lineEdit_after_length->setText(after_length);
            ui->lineEdit_after_width->setText(after_width);
        }
        else if(data.reducer_type == "圆-方")
        {
            ui->radioButton_circle_to_rect->setChecked(true);
            ui->stackedWidget_reducer_before->setCurrentWidget(ui->page_reducer_before_circle);
            ui->stackedWidget_reducer_after->setCurrentWidget(ui->page_reducer_after_rect);

            QStringList parts_after = data.reducer_after_size.split('x', Qt::SkipEmptyParts); // 分割字符串
            QString after_length = parts_after.value(0).trimmed(); // 获取长度并去除空格
            QString after_width = parts_after.value(1).trimmed(); // 获取宽度并去除空格


            ui->lineEdit_before_diameter->setText(data.reducer_before_size);
            ui->lineEdit_after_length->setText(after_length);
            ui->lineEdit_after_width->setText(after_width);
        }
        else if(data.reducer_type == "方-圆")
        {
            ui->radioButton_circle_to_rect->setChecked(true);
            ui->stackedWidget_reducer_before->setCurrentWidget(ui->page_reducer_before_rect);
            ui->stackedWidget_reducer_after->setCurrentWidget(ui->page_reducer_after_circle);

            QStringList parts_before = data.reducer_before_size.split('x', Qt::SkipEmptyParts); // 分割字符串
            QString before_length = parts_before.value(0).trimmed(); // 获取长度并去除空格
            QString before_width = parts_before.value(1).trimmed(); // 获取宽度并去除空格


            ui->lineEdit_before_length->setText(before_length);
            ui->lineEdit_before_width->setText(before_width);
            ui->lineEdit_after_diameter->setText(data.reducer_after_size);
        }

        for(size_t i = 0; i < atten_lineEdits.size(); i++)
        {
            atten_lineEdits[i]->setText(data.atten[i]);
        }

    }

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_reducer::~Dialog_reducer()
{
    delete ui;
}

void Dialog_reducer::on_close_clicked()
{
    this->close();
}

void* Dialog_reducer::getComponent()
{
    return component == nullptr ? nullptr : component;
}

void Dialog_reducer::on_pushButton_confirm_clicked()
{

    array<QString,8> atten;

    for(size_t i = 0; i < atten.size(); i++)
    {
        atten[i] = atten_lineEdits[i]->text();
    }

    if(ui->radioButton_circle_to_circle->isChecked())
    {
        reducer_type = "圆-圆";
        reducer_before_size = ui->lineEdit_before_diameter->text();
        reducer_after_size = ui->lineEdit_after_diameter->text();
    }
    else if(ui->radioButton_circle_to_rect->isChecked())
    {
        reducer_type = "圆-方";
        reducer_before_size = ui->lineEdit_before_diameter->text();
        reducer_after_size = ui->lineEdit_after_length->text() + "x" + ui->lineEdit_after_width->text();
    }
    else if(ui->radioButton_rect_to_circle->isChecked())
    {
        reducer_type = "方-圆";
        reducer_before_size = ui->lineEdit_before_length->text() + "x" + ui->lineEdit_before_width->text();
        reducer_after_size = ui->lineEdit_after_diameter->text();
    }
    else if(ui->radioButton_rect_to_rect->isChecked())
    {
        reducer_type = "方-方";
        reducer_before_size = ui->lineEdit_before_length->text() + "x" + ui->lineEdit_before_width->text();
        reducer_after_size = ui->lineEdit_after_length->text() + "x" + ui->lineEdit_after_width->text();
    }

    /**点击确认前检查，数据是否填完整**/
    if(check_lineedit()==false){
        QMessageBox::information(this,"提示","内容未填写完整");
        return;
    }

    this->component = new Reducer(
                ui->lineEdit_model->text(),
                ui->lineEdit_brand->text(),
                table_id,
                UUID,
                ui->comboBox_data_source->currentText(),
                reducer_type,
                reducer_before_size,
                reducer_after_size,
                atten);

    this->accept(); // 关闭对话框
}

bool Dialog_reducer::check_lineedit()
{
    for(size_t i = 0; i < atten_lineEdits.size(); i++) {
        if(atten_lineEdits[i]->text().isEmpty())
            return false;
    }
    if(ui->lineEdit_model->text().isEmpty()||
       ui->lineEdit_brand->text().isEmpty())
        return false;

    if(reducer_type == "圆-圆")
        if(ui->lineEdit_before_diameter->text().isEmpty()||
           ui->lineEdit_after_diameter->text().isEmpty())
            return false;

    if(reducer_type == "圆-方")
        if(ui->lineEdit_before_diameter->text().isEmpty()||
            ui->lineEdit_after_length->text().isEmpty()||
            ui->lineEdit_after_width->text().isEmpty())
            return false;

    if(reducer_type == "方-圆")
        if(ui->lineEdit_before_length->text().isEmpty()||
             ui->lineEdit_before_width->text().isEmpty()   ||
            ui->lineEdit_after_diameter->text().isEmpty())
            return false;

    if(reducer_type == "方-方")
        if(ui->lineEdit_before_length->text().isEmpty()||
             ui->lineEdit_before_width->text().isEmpty()||
             ui->lineEdit_after_length->text().isEmpty()||
             ui->lineEdit_after_width->text().isEmpty())
            return false;

    return true;
}


void Dialog_reducer::on_radioButton_circle_to_circle_clicked()
{
    ui->lineEdit_before_diameter->clear();
    ui->lineEdit_before_length->clear();
    ui->lineEdit_before_width->clear();
    ui->lineEdit_after_diameter->clear();
    ui->lineEdit_after_length->clear();
    ui->lineEdit_after_width->clear();

    ui->stackedWidget_reducer_before->setCurrentWidget(ui->page_reducer_before_circle);
    ui->stackedWidget_reducer_after->setCurrentWidget(ui->page_reducer_after_circle);
}


void Dialog_reducer::on_radioButton_rect_to_rect_clicked()
{
    ui->lineEdit_before_diameter->clear();
    ui->lineEdit_before_length->clear();
    ui->lineEdit_before_width->clear();
    ui->lineEdit_after_diameter->clear();
    ui->lineEdit_after_length->clear();
    ui->lineEdit_after_width->clear();

    ui->stackedWidget_reducer_before->setCurrentWidget(ui->page_reducer_before_rect);
    ui->stackedWidget_reducer_after->setCurrentWidget(ui->page_reducer_after_rect);
}


void Dialog_reducer::on_radioButton_circle_to_rect_clicked()
{
    ui->lineEdit_before_diameter->clear();
    ui->lineEdit_before_length->clear();
    ui->lineEdit_before_width->clear();
    ui->lineEdit_after_diameter->clear();
    ui->lineEdit_after_length->clear();
    ui->lineEdit_after_width->clear();

    ui->stackedWidget_reducer_before->setCurrentWidget(ui->page_reducer_before_circle);
    ui->stackedWidget_reducer_after->setCurrentWidget(ui->page_reducer_after_rect);
}




void Dialog_reducer::on_radioButton_rect_to_circle_clicked()
{
    ui->lineEdit_before_diameter->clear();
    ui->lineEdit_before_length->clear();
    ui->lineEdit_before_width->clear();
    ui->lineEdit_after_diameter->clear();
    ui->lineEdit_after_length->clear();
    ui->lineEdit_after_width->clear();

    ui->stackedWidget_reducer_before->setCurrentWidget(ui->page_reducer_before_rect);
    ui->stackedWidget_reducer_after->setCurrentWidget(ui->page_reducer_after_circle);
}

QList<QStringList> Dialog_reducer::getComponentDataAsStringList() const
{
    QList<QStringList> dataLists;
    QStringList data = {
        component->table_id,
        component->model,
        component->brand,
        component->reducer_type,
        component->reducer_before_size,
        component->reducer_after_size
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

