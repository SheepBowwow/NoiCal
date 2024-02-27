#ifndef ROOMCALTABLE_H
#define ROOMCALTABLE_H

#include <QWidget>
#include <QComboBox>
#include <QWidget>


namespace Ui {
class RoomCalTable;
}

class RoomCalTable : public QWidget
{
    Q_OBJECT

public:
    explicit RoomCalTable(QString systemName, QWidget *parent = nullptr, QString type = "");
    ~RoomCalTable();
    int getIndexInLayout() const;
    void setCollapsed();
    void setIsCollapsed(bool isCollapsed);
    void setSerialNum(int num);
    bool isValid;
    QString type;       //表格类型，如生源噪音
    QVector<QString> noise_after_cal;        //当前计算后(增加或衰减)的噪音量
    QVector<QString> variations;        //噪音变化量,和下一个表格相关

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

    void on_comboBox_room_type_currentTextChanged(const QString &arg1);

    void on_comboBox_noi_src_num_currentTextChanged(const QString &arg1);

    void on_comboBox_noi_locate_currentTextChanged(const QString &arg1);


    void on_comboBox_atten_noi_end_size_currentTextChanged(const QString &arg1);

    void on_comboBox_pipe_elbow_type_currentTextChanged(const QString &arg1);

private:
    Ui::RoomCalTable *ui;
    void clearPage(QWidget *widget);
    bool isCollapsed;
    void updateComboBoxItems();
    QString currentTable;       //当前是什么表格
    QString systemName;       //所属系统名称

private:
    QVector<QLineEdit*> noi_after_cal_lineEdits;
};

#endif // ROOMCALTABLE_H
