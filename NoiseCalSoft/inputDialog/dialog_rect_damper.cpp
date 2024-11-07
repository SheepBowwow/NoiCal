#include "inputDialog/dialog_rect_damper.h"
#include "ui_dialog_rect_damper.h"
#include "calFunction/cal_function.h"

Dialog_rect_damper::Dialog_rect_damper(QWidget *parent, int editRow, const Rect_damper& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_rect_damper)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件

    noi_lineEdits = { ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500,
                      ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k,
                      ui->lineEdit_total };

    if(editRow != -1)
    {
        ui->lineEdit_model->setText(data.model);
        ui->lineEdit_length->setText(data.length);
        ui->lineEdit_brand->setText(data.brand);
        ui->lineEdit_air_volume->setText(data.air_volume);
        ui->comboBox_angle->setCurrentText(data.angle);
        ui->comboBox_data_source->setCurrentText(data.data_source);
        table_id = data.table_id;
        UUID= data.UUID;

        for(size_t i = 0; i < noi_lineEdits.size(); i++)
        {
            noi_lineEdits[i]->setText(data.noi[i]);
        }
    }

    //进口
    for(auto& lineEdit : noi_lineEdits)
    {
        connect(lineEdit, &QLineEdit::textChanged, this, &Dialog_rect_damper::calTotalNoise);
    }

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_rect_damper::~Dialog_rect_damper()
{
    delete ui;
}

void* Dialog_rect_damper::getComponent()
{
    return component == nullptr ? nullptr : component;
}

void Dialog_rect_damper::calTotalNoise() {
    if(std::all_of(noi_lineEdits.begin(), noi_lineEdits.end() - 1, [](QLineEdit* lineEdit)
        { return !lineEdit->text().isEmpty(); }))
    {
        double totalValueIn = calNoiseTotalValue(noi_lineEdits);
        noi_lineEdits[8]->setText(QString::number(totalValueIn, 'f', 1));
    }
}

QList<QStringList> Dialog_rect_damper::getComponentDataAsStringList() const
{
    QList<QStringList> dataLists;
    QStringList data = {
        component->table_id,
        component->model,
        component->brand,
        component->length + "x" + component->width,
        component->air_volume,
        component->angle,
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

void Dialog_rect_damper::on_close_clicked()
{
    this->close();
}


void Dialog_rect_damper::on_pushButton_confirm_clicked()
{
    array<QString,9> noi;

    for(size_t i = 0; i < noi.size(); i++)
    {
        noi[i] = noi_lineEdits[i]->text().trimmed();
    }

    /**点击确认前检查，数据是否填完整**/
    if(check_lineedit()==false){
        QMessageBox::information(this,"提示","内容未填写完整");
        return;
    }

    this->component = new Rect_damper(
                ui->lineEdit_model->text().trimmed(),
                ui->lineEdit_brand->text().trimmed(),
                table_id,
                UUID,
                ui->comboBox_data_source->currentText(),
                ui->comboBox_angle->currentText(),
                ui->lineEdit_air_volume->text().trimmed(),
                noi,
                ui->lineEdit_length->text().trimmed(),
                ui->lineEdit_width->text().trimmed());

    this->accept(); // 关闭对话框
}

bool Dialog_rect_damper::check_lineedit()
{
    for(size_t i = 0; i < noi_lineEdits.size(); i++){
        if(noi_lineEdits[i]->text().isEmpty())
            return false;
    }

    if(ui->lineEdit_model->text().isEmpty()||
        ui->lineEdit_brand->text().isEmpty()||
        ui->lineEdit_air_volume->text().isEmpty()||
        ui->lineEdit_length->text().isEmpty()||
        ui->lineEdit_width->text().isEmpty()
        )return false;

    return true;
}
