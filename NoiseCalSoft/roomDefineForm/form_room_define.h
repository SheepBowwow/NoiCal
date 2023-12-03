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
    void formsent(QTreeWidgetItem *item,QString name,int num);
private slots:
    void addroom();
    void delroom();
    void jieshou(QString name,int num);
private:
    Ui::Form_room_define *ui;
    QTreeWidgetItem *_item;
    Dialog_addroom *dialog;
};

#endif // FORM_ROOM_DEFINE_H
