#ifndef ROOM_CAL_BASEWIDGET_H
#define ROOM_CAL_BASEWIDGET_H

#include <QWidget>
#include <QPointer>
#include <QMenu>
#include "roomCal/roomcaltable.h"

namespace Ui {
class Room_cal_baseWidget;
}

class Room_cal_baseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Room_cal_baseWidget(QWidget *parent = nullptr, bool named = false);
    ~Room_cal_baseWidget();
    bool isNamed;
    void addMenuAction(QString itemName);
    void addTable(int index, QString type = "");
    void addTable(const RoomCalTableType &type, const QJsonObject &jsonObj);
    void setInfo(QString MVZName, QString deck, QString roomOrOuterNumber, QString noiseLimit, QString ductNum);    //设置基本信息，主竖区、甲板、房间编号、噪声限值、主风管数量
    void setDuctNumber(QString number); //设置主风管编号
    void setAirVolume(QString airVolume); //设置风量
    void setSystemName(QString systemName); //设置系统名
    void setDuctNum(QString ductNum);   //设置主风管数量
    void setMVZName(QString MVZName);   //设置主竖区名
    bool getIsOuter();

    void switch_outer_cal();
    QString title_label="主风管";

private slots:
    void handleAddBefore(int index);
    void handleAddAfter(int index);
    void handleDelete(int index);
    void updateAllTables();

    void on_pushButton_add_clicked();

    void on_pushButton_fold_clicked();

    void on_pushButton_confirm_clicked();

private:
    Ui::Room_cal_baseWidget *ui;
    QPointer<RoomCalTable> selectedTable;
    QString _systemName;       //系统名称
    QString _roomOrOuterNumber;
    QString _ductNumber;
    QString _MVZName;
    bool _isOuter;
    bool isAllCollapsed;
    void handleMenuAction(QString actionName);
};

#endif // ROOM_CAL_BASEWIDGET_H
