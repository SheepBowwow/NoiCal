#ifndef ROOM_CAL_BASEWIDGET_H
#define ROOM_CAL_BASEWIDGET_H

#include <QWidget>
#include <QPointer>
#include <QMenu>
#include "roomCal/roomcaltable.h"

namespace Ui {
class room_cal_baseWidget;
}

class room_cal_baseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit room_cal_baseWidget(QWidget *parent = nullptr,QString m_roomName = "");
    ~room_cal_baseWidget();
    int flag_firstopen = 1;
    void addMenuAction(QString itemName);
    void setInfo(QString zhushuqu, QString deck, QString room_number, QString noise_limit, QString duct_num);    //设置基本信息，主竖区、甲板、房间编号、噪声限值、主风管数量
    void setMainDuctNumber(QString number); //设置主风管编号
    void setFanQuantity(QString fanQuantity); //设置风量
    void setSystemName(QString systemName); //设置系统名

private slots:
    void handleAddBefore(int index);
    void handleAddAfter(int index);
    void handleDelete(int index);

    void addTable(int index, QString type = "");

    void on_pushButton_add_clicked();

    void on_pushButton_fold_clicked();

    void on_pushButton_confirm_clicked();

private:
    Ui::room_cal_baseWidget *ui;
    QPointer<RoomCalTable> selectedTable;
    QString roomName;       //典型房间名，如果不是典型房间就为空
    QString systemName;       //系统名称
    bool isAllCollapsed;
    void handleMenuAction(QString actionName);
};

#endif // ROOM_CAL_BASEWIDGET_H
