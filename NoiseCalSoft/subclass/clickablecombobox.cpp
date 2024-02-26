#include "clickablecombobox.h"

ClickableComboBox::ClickableComboBox(QWidget *parent) : QComboBox(parent)
{
}

void ClickableComboBox::mousePressEvent(QMouseEvent *event)
{
    QComboBox::mousePressEvent(event);
    emit clicked();
}
