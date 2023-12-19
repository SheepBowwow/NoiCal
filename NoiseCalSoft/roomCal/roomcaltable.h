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
    void setCollapsed();
    void setIsCollapsed(bool isCollapsed);
    void setSerialNum(int num);
    bool isValid;
    QString type;       //表格类型，如生源噪音

signals:
    void addBeforeClicked(int index);
    void addAfterClicked(int index);
    void deleteClicked(int index);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_comboBox_sound_type_currentTextChanged(const QString &arg1);

    void on_comboBox_unit_name_currentTextChanged(const QString &arg1);

    void on_comboBox_sound_type_room_currentTextChanged(const QString &arg1);

    void on_stackedWidget_table_currentChanged(int arg1);

    void on_pushButton_number_clicked();

private:
    Ui::RoomCalTable *ui;
    void clearPage(QWidget *widget);
    bool isCollapsed;
};

#endif // ROOMCALTABLE_H
