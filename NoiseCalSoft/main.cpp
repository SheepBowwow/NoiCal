#include "widget.h"
#include <QApplication>
#include <QCoreApplication>
#include "globle_var.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    a.setStyleSheet("QMessageBox[customMsgBox=true] { font-family: '黑体'; font-size: 10pt; }"
                    "QMessageBox[customMsgBox=true] QPushButton { font-family: '黑体'; font-size: 10pt; }");

    Widget w;

    // 隐藏标题栏
    w.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    w.show();

    return a.exec();
}
