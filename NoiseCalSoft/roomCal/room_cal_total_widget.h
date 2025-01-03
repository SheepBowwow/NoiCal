#ifndef ROOM_CAL_TOTAL_WIDGET_H
#define ROOM_CAL_TOTAL_WIDGET_H

#include <QWidget>

namespace Ui {
class Room_cal_total_widget;
}

class Room_cal_total_widget : public QWidget
{
    Q_OBJECT

public:
    explicit Room_cal_total_widget(QWidget *parent = nullptr);
    ~Room_cal_total_widget();

    void change_outer_cal();

    void setInfo(QString MVZName, QString deck,
                 QString numer, QString noiseLimit, QString ductNum);    //设置基本信息，主竖区、甲板、房间编号、噪声限值、主风管数量

    void setDuctNum(QString ductNum);

    void setMVZName(QString MVZName);

    void addDuctToTable(QString ductNumber, double test_point_distance, double A_widget_noise);

    void handle_duct_number_revise(QString origin_number, QString new_number);
    void handle_duct_remove(QString remove_number);

private:
    Ui::Room_cal_total_widget *ui;

    void initTableWidget();
    void calTotalNoise();
};

#endif // ROOM_CAL_TOTAL_WIDGET_H
