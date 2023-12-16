#ifndef ROOM_CAL_TOTAL_H
#define ROOM_CAL_TOTAL_H

#include <QWidget>

namespace Ui {
class room_cal_total;
}

class room_cal_total : public QWidget
{
    Q_OBJECT

public:
    explicit room_cal_total(QWidget *parent = nullptr);
    ~room_cal_total();

private:
    Ui::room_cal_total *ui;
};

#endif // ROOM_CAL_TOTAL_H
