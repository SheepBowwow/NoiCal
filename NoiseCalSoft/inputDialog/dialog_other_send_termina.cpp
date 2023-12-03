#include "inputDialog/dialog_other_send_termina.h"
#include "ui_dialog_other_send_terminal.h"
#include <cmath>

Dialog_other_send_terminal::Dialog_other_send_terminal(QWidget *parent, int editRow,  const Other_send_terminal_noise& data) :
    QDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_other_send_terminal)
{
    ui->setupUi(this);
    if(editRow != -1)
    {
        ui->lineEdit_brand->setText(data.brand);
        ui->lineEdit_model->setText(data.model);
        ui->lineEdit_63->setText(data.noi_63);
        ui->lineEdit_125->setText(data.noi_125);
        ui->lineEdit_250->setText(data.noi_250);
        ui->lineEdit_500->setText(data.noi_500);
        ui->lineEdit_1k->setText(data.noi_1k);
        ui->lineEdit_2k->setText(data.noi_2k);
        ui->lineEdit_4k->setText(data.noi_4k);
        ui->lineEdit_8k->setText(data.noi_8k);
        ui->lineEdit_total->setText(data.noi_total);
    }

    //连接lineEdit信号与槽
    connect(ui->lineEdit_63, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_125, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_250, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_500, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_1k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_2k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_4k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));
    connect(ui->lineEdit_8k, SIGNAL(textChanged(QString)), this, SLOT(calTotalNoise()));

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_other_send_terminal::~Dialog_other_send_terminal()
{
    delete ui;
}


//获得noi
Other_send_terminal_noise* Dialog_other_send_terminal::getNoi()
{
    return noi == nullptr ? nullptr : noi;
}

//计算总值
void Dialog_other_send_terminal::total_noi()
{
    double noi_63 = ui->lineEdit_63->text().toDouble();
    double noi_125 = ui->lineEdit_125->text().toDouble();
    double noi_250 = ui->lineEdit_250->text().toDouble();
    double noi_500 = ui->lineEdit_500->text().toDouble();
    double noi_1k = ui->lineEdit_1k->text().toDouble();
    double noi_2k = ui->lineEdit_2k->text().toDouble();
    double noi_4k = ui->lineEdit_4k->text().toDouble();
    double noi_8k = ui->lineEdit_8k->text().toDouble();
    double temp = pow(10, noi_63 / 10) + pow(10, noi_125 / 10) + pow(10, noi_250 / 10) + pow(10, noi_500 / 10)
            + pow(10, noi_1k / 10) + pow(10, noi_2k / 10) + pow(10, noi_4k / 10) + pow(10, noi_8k / 10);
    double total = 10 * log10(temp);
    QString totalString = QString::number(total,'f',1); // 将total转换为字符串
    ui->lineEdit_total->setText(totalString); // 设置lineEdit的文本为total的字符串表示
}

// 槽函数实现
void Dialog_other_send_terminal::calTotalNoise() {
    // 检查所有八个QLineEdit是否都有数据
    if(ui->lineEdit_63->text().isEmpty() || ui->lineEdit_125->text().isEmpty()
       || ui->lineEdit_250->text().isEmpty() || ui->lineEdit_500->text().isEmpty() ||
            ui->lineEdit_1k->text().isEmpty() || ui->lineEdit_2k->text().isEmpty() ||
            ui->lineEdit_4k->text().isEmpty() || ui->lineEdit_8k->text().isEmpty()) {
        // 如果有任何一个QLineEdit没有数据，不执行计算
        return;
    }

    total_noi();
}

//关闭窗口按钮
void Dialog_other_send_terminal::on_close_clicked()
{
    this->close();
}

//确认按钮
void Dialog_other_send_terminal::on_pushButton_confirm_clicked()
{
    this->noi = new Other_send_terminal_noise;

    // 获取对应行的数据，将界面上的数据保存到对应行中
    noi->brand = ui->lineEdit_brand->text();
    noi->model = ui->lineEdit_model->text();
    noi->noi_63 = ui->lineEdit_63->text();
    noi->noi_125 = ui->lineEdit_125->text();
    noi->noi_250 = ui->lineEdit_250->text();
    noi->noi_500 = ui->lineEdit_500->text();
    noi->noi_1k = ui->lineEdit_1k->text();
    noi->noi_2k = ui->lineEdit_2k->text();
    noi->noi_4k = ui->lineEdit_4k->text();
    noi->noi_8k = ui->lineEdit_8k->text();
    noi->noi_total = ui->lineEdit_total->text();
    noi->number = noi->model;
    this->accept(); // 关闭对话框
}

//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对三个鼠标事件的重写
void Dialog_other_send_terminal::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton&&!this->isMaximized())
    {
            //获取相当于窗口的位置                标题栏的高度
        if(mapFromGlobal(QCursor().pos()).y()<ui->widget_top->height())
        {
            last = e->globalPos();
            canmove=1;
        }
    }
}
void Dialog_other_send_terminal::mouseMoveEvent(QMouseEvent *e)
{
    if(canmove == 1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        last = e->globalPos();
        move(x()+dx, y()+dy);
    }
}
void Dialog_other_send_terminal::mouseReleaseEvent(QMouseEvent *e)
{
    if(canmove == 1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        move(x()+dx, y()+dy);
        canmove=0;
    }
}
