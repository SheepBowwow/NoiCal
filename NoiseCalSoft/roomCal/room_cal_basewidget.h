#ifndef ROOM_CAL_BASEWIDGET_H
#define ROOM_CAL_BASEWIDGET_H

#include <QWidget>

namespace Ui {
class room_cal_baseWidget;
}

class room_cal_baseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit room_cal_baseWidget(QWidget *parent = nullptr);
    ~room_cal_baseWidget();

private:
    Ui::room_cal_baseWidget *ui;
};

#endif // ROOM_CAL_BASEWIDGET_H
