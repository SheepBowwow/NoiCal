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
    explicit RoomCalTable(QString systemName, QWidget *parent = nullptr, QString currentTableType = "");
    ~RoomCalTable();
    int getIndexInLayout() const;
    void setCollapsed();
    void setIsCollapsed(bool isCollapsed);
    void setSerialNum(int num);
    bool isValid;
    QString currentTableType;       //表格类型，如声源噪音
    QString currentUnitType;       //部件类型，如风机
    QVector<QString> noise_before_cal;        //当前计算前(增加或衰减)的噪音量
    QVector<QString> variations;        //噪音变化量,和下一个表格相关
    QVector<QString> noise_after_cal;        //当前计算后(增加或衰减)的噪音量

signals:
    void addBeforeClicked(int index);
    void addAfterClicked(int index);
    void deleteClicked(int index);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:


    void on_stackedWidget_table_currentChanged(int arg1);

    void on_comboBox_sound_type_currentTextChanged(const QString &arg1);

    void on_comboBox_unit_name_currentTextChanged(const QString &arg1);

    void on_comboBox_sound_type_room_currentTextChanged(const QString &arg1);

    void on_pushButton_number_clicked();

    void on_comboBox_room_type_currentTextChanged(const QString &arg1);

    void calVariations();

private:
    Ui::RoomCalTable *ui;
    void clearPage(QWidget *widget);
    void connectLineEditsToCalSlot(const QVector<QLineEdit*>& lineEdits);
    void disconnectLineEditsToCalSlot(const QVector<QLineEdit*>& lineEdits);
    bool isCollapsed;
    void updateComboBoxItems();
    QString systemName;       //所属系统名称

private:
    QVector<QLineEdit*> noi_after_cal_lineEdits;
    QVector<QLineEdit*> noi_lineEdits;       //噪音
    QVector<QLineEdit*> terminal_atten_lineEdits;       //末端衰减
    QVector<QLineEdit*> terminal_refl_lineEdits;       //末端反射衰减
    QVector<QLineEdit*> terminal_noi_lineEdits;       //末端气流噪音
    QVector<QLineEdit*> each_atten_lineEdits;       //1米直管或单个弯头衰减
    QVector<QLineEdit*> sum_atten_lineEdits;       //衰减汇总
    QVector<QLineEdit*> atten_lineEdits;       //衰减
};

#endif // ROOMCALTABLE_H
