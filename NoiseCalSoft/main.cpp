#include "widget.h"
#include "roomCal/room_cal_basewidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Widget w;
//    //隐藏标题栏
//    w.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
//    w.show();

    room_cal_baseWidget* r = new room_cal_baseWidget;
    r->show();

    return a.exec();
}
