#include "inputbasedialog.h"

ImageDialog* ImageDialog::instance = nullptr;

void ImageDialog::closeImageDialog() {
    close();  // 关闭 ImageDialog
}

InputBaseDialog::InputBaseDialog(QWidget *parent, QWidget *topWidget) : QDialog(parent), canmove(0), widgetTop(topWidget)
{
}

InputBaseDialog::~InputBaseDialog()
{
}

void InputBaseDialog::setTopWidget(QWidget *topWidget)
{
    this->widgetTop = topWidget;
}

void InputBaseDialog::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && !this->isMaximized())
    {
        if (mapFromGlobal(QCursor().pos()).y() < widgetTop->height())
        {
            last = e->globalPos();
            canmove = 1;
        }
    }
}

void InputBaseDialog::mouseMoveEvent(QMouseEvent *e)
{
    if (canmove == 1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        last = e->globalPos();
        move(x() + dx, y() + dy);
    }
}

void InputBaseDialog::mouseReleaseEvent(QMouseEvent *e)
{
    if (canmove == 1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        move(x() + dx, y() + dy);
        canmove = 0;
    }
}
