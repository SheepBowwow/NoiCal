#include "widget.h"
#include <QApplication>
#include <QCoreApplication>
#include "globle_var.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    DBManager = new DatabaseManager(QString(QCoreApplication::applicationDirPath() + "\\database\\noise_calculation_system.db"));

    Widget w;

    //隐藏标题栏
    w.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    w.show();

    return a.exec();
}
