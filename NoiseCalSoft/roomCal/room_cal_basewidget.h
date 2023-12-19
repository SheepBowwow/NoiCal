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
    QString roomName;
    bool isAllCollapsed;
    void handleMenuAction(QString actionName);
};

#endif // ROOM_CAL_BASEWIDGET_H
