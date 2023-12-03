#include "create_prj.h"
#include "ui_create_prj.h"
#include "globle_var.h"
#include <QDebug>

create_prj::create_prj(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::create_prj)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

create_prj::~create_prj()
{
    delete ui;
}
//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对三个鼠标事件的重写
void create_prj::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton&&!this->isMaximized())
    {
            //获取相当于窗口的位置                标题栏的高度
        if(mapFromGlobal(QCursor().pos()).y()<ui->widget_top->height())
        {
            last = e->globalPos();
            canmove=1;
        }
    }
}
void create_prj::mouseMoveEvent(QMouseEvent *e)
{
    if(canmove==1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        last = e->globalPos();
        move(x()+dx, y()+dy);
    }
}
void create_prj::mouseReleaseEvent(QMouseEvent *e)
{
    if(canmove==1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        move(x()+dx, y()+dy);
        canmove=0;
    }
}

void create_prj::on_close_clicked()
{
    this->close();
}

void create_prj::on_pushButton_create_prj_clicked()
{
    QString projectName = ui->lineEdit_prj_num->text();
    project.prj_name = projectName;

    // 关闭新建项目窗口
    this->close();

}

