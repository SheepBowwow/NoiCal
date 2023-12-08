#include "widget.h"
#include "roomCal/roomcaltable.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    //隐藏标题栏
    w.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    w.show();
//    RoomCalTable r;
//    r.show();

    return a.exec();
}
