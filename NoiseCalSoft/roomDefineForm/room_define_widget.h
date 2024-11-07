#ifndef ROOM_DEFINE_WIDGET_H
#define ROOM_DEFINE_WIDGET_H

#include <QTableWidget>
#include <QWidget>
#include <QPushButton>
#include <QTreeWidget>
#include "roomDefineForm/dialog_add_room.h"
#include <mutex>


namespace Ui {
class Room_define_widget;
}

class Room_define_widget : public QWidget
{
    Q_OBJECT

public:
    explicit Room_define_widget(QString systemName = "", QWidget *parent = nullptr);
    ~Room_define_widget();
    void switchOuter();
    void setSystemItem(QTreeWidgetItem *item);
    void setTableRowDuctNum(QString room_or_outer_number, QString ductNum);
    void setTableHeaderColWidth();

signals:
    void roomAdd(QTreeWidgetItem* item,QString name,int num, QString jiaban, QString limit);
    void roomDel(QTreeWidgetItem* item,QString roomid);

private slots:

    void on_pushButton_add_clicked();

    void on_pushButton_del_clicked();

    void on_pushButton_revise_clicked();

public slots:
    void addRoomToTable(const QString&, const QString&, const QString&, const QString&,
                        const QString&, const QString&, const QString&, const QString&, const QString&, const QString&);

private:
    Ui::Room_define_widget *ui;
    QTreeWidgetItem *_systemItem;
    Dialog_add_room *dialog;
    QString system_name;
    QString mvz_name;
    std::once_flag _flag; // 静态 once_flag 变量
    bool isOuter{false};

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // ROOM_DEFINE_WIDGET_H
