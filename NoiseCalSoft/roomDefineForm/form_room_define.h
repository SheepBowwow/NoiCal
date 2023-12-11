#ifndef FORM_ROOM_DEFINE_H
#define FORM_ROOM_DEFINE_H

#include <QTableWidget>
#include <QWidget>
#include <QPushButton>
#include <QTreeWidget>
#include "roomDefineForm/dialog_addroom.h"


namespace Ui {
class Form_room_define;
}

class Form_room_define : public QWidget
{
    Q_OBJECT

public:
    explicit Form_room_define(QWidget *parent = nullptr);
    ~Form_room_define();

    void setjiabanItem(QTreeWidgetItem *item);

    void buttonToHeader(QTableWidget *tableWidget, QWidget *buttonWidget, const char *addButtonSlot, const char *delButtonSlot);

signals:
    void roomadd(QTreeWidgetItem *item,QString name,int num);
    void roomdel(QTreeWidgetItem*,QString roomid);
private slots:
    void jieshou(QString name,int num);

    void on_buttonadd_clicked();

    void on_buttondel_clicked();

private:
    Ui::Form_room_define *ui;
    QTreeWidgetItem *_jiabanitem;
    Dialog_addroom *dialog;
};

#endif // FORM_ROOM_DEFINE_H
