#ifndef ROOM_CAL_BASEWIDGET_H
#define ROOM_CAL_BASEWIDGET_H

#include <QWidget>
#include <QPointer>
#include "roomCal/roomcaltable.h"

namespace Ui {
class room_cal_baseWidget;
}

class room_cal_baseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit room_cal_baseWidget(QWidget *parent = nullptr);
    ~room_cal_baseWidget();

private slots:
    void handleAddBefore(int index);
    void handleAddAfter(int index);
    void handleDelete(int index);

    void addTable(int index, QString type = "");

    void on_pushButton_add_clicked();

    void onNoiSrcClicked();
    void onAirflowNoiClicked();
    void onAttenAirflowNoiClicked();
    void onAttenClicked();
    void onSoundPressureClicked();

private:
    Ui::room_cal_baseWidget *ui;
    QPointer<RoomCalTable> selectedTable;
};

#endif // ROOM_CAL_BASEWIDGET_H
