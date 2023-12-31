#include "dialog_prj_manager.h"
#include "ui_dialog_prj_manager.h"
#include "globle_var.h"

Dialog_prj_manager::Dialog_prj_manager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_prj_manager)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_prj_manager::~Dialog_prj_manager()
{
    delete ui;
}

//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对三个鼠标事件的重写
void Dialog_prj_manager::mousePressEvent(QMouseEvent *e)
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
void Dialog_prj_manager::mouseMoveEvent(QMouseEvent *e)
{
    if(canmove==1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        last = e->globalPos();
        move(x()+dx, y()+dy);
    }
}
void Dialog_prj_manager::mouseReleaseEvent(QMouseEvent *e)
{
    if(canmove==1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        move(x()+dx, y()+dy);
        canmove=0;
    }
}

void Dialog_prj_manager::on_close_clicked()
{
    this->close();
}

void Dialog_prj_manager::on_pushButton_create_prj_clicked()
{
    QString projectName = ui->lineEdit_prj_num->text();
    project.prj_name = projectName;

    // 发射新的信号，传递项目名称
    emit createProjectClicked(projectName);

    // 关闭新建项目窗口
    this->close();
}
