#include "inputDialog/dialog_pipe.h"
#include "ui_dialog_pipe.h"

Dialog_pipe::Dialog_pipe(QWidget *parent, int editRow, const Pipe& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_pipe)
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
        ui->lineEdit_model->setText(data.model);
        ui->lineEdit_brand->setText(data.brand);
        ui->comboBox_data_source->setCurrentText(data.data_source);
        table_id = data.table_id;
        UUID = data.UUID;

        QString diameter = "";
        QString length = "";
        QString width = "";
        if(data.pipe_shape == "圆形")
        {
            for(int i = 0; i < data.size.length(); i++)
            {
                if(data.model[i] >= '0' && data.model[i] <= '9')
                {
                    diameter += data.model[i];
                }
            }

            ui->radioButton_circle->setChecked(true);
            ui->radioButton_rect->setChecked(false);
            ui->stackedWidget_input->setCurrentWidget(ui->page_circle);
            ui->lineEdit_diameter->setText(diameter);
        }
        else if(data.pipe_shape == "方形")
        {
            QStringList parts = data.size.split('x'); // 使用split函数按照斜杠分割字符串
            // parts中的第一个元素是前缀，第二个元素是后缀（如果有的话）
            QString suffix = parts.value(0).trimmed(); // 去除前缀两边的空格
            for(int i = 0; i < suffix.length(); i++)
            {
                if(suffix[i] >= '0' && suffix[i] <= '9')
                {
                    length += suffix[i];
                }
            }
            width = parts.value(1).trimmed(); // 去除后缀两边的空格

            ui->radioButton_circle->setChecked(false);
            ui->radioButton_rect->setChecked(true);
            ui->stackedWidget_input->setCurrentWidget(ui->page_rect);
            ui->lineEdit_width->setText(width);
            ui->lineEdit_length->setText(length);
        }

        for(size_t i = 0; i < atten_lineEdits.size(); i++)
        {
            atten_lineEdits[i]->setText(data.atten[i]);
        }
    }


    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_pipe::~Dialog_pipe()
{
    delete ui;
}

void* Dialog_pipe::getComponent()
{
    return component == nullptr ? nullptr : component;
}

void Dialog_pipe::on_close_clicked()
{
    this->close();
}

void Dialog_pipe::on_radioButton_circle_clicked()
{
    ui->lineEdit_63->clear();
    ui->lineEdit_125->clear();
    ui->lineEdit_250->clear();
    ui->lineEdit_500->clear();
    ui->lineEdit_1k->clear();
    ui->lineEdit_2k->clear();
    ui->lineEdit_4k->clear();
    ui->lineEdit_8k->clear();


    ui->lineEdit_length->clear();
    ui->lineEdit_width->clear();
    ui->stackedWidget_input->setCurrentWidget(ui->page_circle);
}


void Dialog_pipe::on_radioButton_rect_clicked()
{
    ui->lineEdit_63->clear();
    ui->lineEdit_125->clear();
    ui->lineEdit_250->clear();
    ui->lineEdit_500->clear();
    ui->lineEdit_1k->clear();
    ui->lineEdit_2k->clear();
    ui->lineEdit_4k->clear();
    ui->lineEdit_8k->clear();
    ui->lineEdit_length->clear();
    ui->lineEdit_width->clear();
    ui->lineEdit_diameter->clear();


    ui->lineEdit_diameter->clear();
    ui->stackedWidget_input->setCurrentWidget(ui->page_rect);
}


void Dialog_pipe::on_pushButton_confirm_clicked()
{

    array<QString,8> atten;

    for(size_t i = 0; i < atten.size(); i++)
    {
        atten[i] = atten_lineEdits[i]->text();
    }

    if(ui->radioButton_circle->isChecked())
    {
        pipe_shape = "圆形";
        size = ui->lineEdit_diameter->text();
    }
    else if(ui->radioButton_rect->isChecked())
    {
        pipe_shape = "方形";
        size = ui->lineEdit_length->text() + "x" + ui->lineEdit_width->text();
    }

    /**点击确认前检查，数据是否填完整**/
    if(check_lineedit()==false){
        QMessageBox::information(this,"提示","内容未填写完整");
        return;
    }

    this->component = new Pipe(
                ui->lineEdit_model->text(),
                ui->lineEdit_brand->text(),
                table_id,
                UUID,
                ui->comboBox_data_source->currentText(),
                pipe_shape,
                size,
                atten);

    this->accept(); // 关闭对话框
}

bool Dialog_pipe::check_lineedit()
{
    for(size_t i = 0; i < atten_lineEdits.size(); i++){
        if(atten_lineEdits[i]->text().isEmpty())
            return false;
    }

    if(ui->lineEdit_model->text().isEmpty()||
       ui->lineEdit_brand->text().isEmpty())
        return false;

    if((pipe_shape == "圆形" && ui->lineEdit_diameter->text().isEmpty())||
       (pipe_shape == "方形" && (ui->lineEdit_length->text().isEmpty()  ||
         ui->lineEdit_width->text().isEmpty()))
       )return false;

    return true;
}

QList<QStringList> Dialog_pipe::getComponentDataAsStringList() const
{
    QList<QStringList> dataLists;
    QStringList data = {
        component->table_id,
        component->model,
        component->brand,
        component->pipe_shape,
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

