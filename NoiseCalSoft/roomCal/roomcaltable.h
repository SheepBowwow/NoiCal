#ifndef ROOMCALTABLE_H
#define ROOMCALTABLE_H

#include <QWidget>

namespace Ui {
class RoomCalTable;
}

class RoomCalTable : public QWidget
{
    Q_OBJECT

public:
    explicit RoomCalTable(QWidget *parent = nullptr, QString type = "");
    ~RoomCalTable();
    int getIndexInLayout() const;

signals:
    void addBeforeClicked(int index);
    void addAfterClicked(int index);
    void deleteClicked(int index);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void on_comboBox_sound_type_currentTextChanged(const QString &arg1);

    void on_comboBox_unit_name_currentTextChanged(const QString &arg1);

    void on_comboBox_sound_type_room_currentTextChanged(const QString &arg1);

    void on_stackedWidget_table_currentChanged(int arg1);

private:
    Ui::RoomCalTable *ui;

    void clearPage(QWidget *widget);
};

#endif // ROOMCALTABLE_H
