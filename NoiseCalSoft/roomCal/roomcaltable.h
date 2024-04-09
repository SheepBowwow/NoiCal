#ifndef ROOMCALTABLE_H
#define ROOMCALTABLE_H

#include <QWidget>
#include <QComboBox>
#include <QWidget>
#include <QTimer>
#include "Component/ComponentStructs.h"
#include <QValidator>
#include <QString>

class DimensionValidator : public QValidator {
public:
    DimensionValidator(QObject *parent = nullptr) : QValidator(parent) {}

    State validate(QString &input, int &pos) const override {
        Q_UNUSED(pos)

        // 确保输入匹配 "数字x数字" 格式，其中x前后可以没有数字
        QRegExp rx("^\\d*x\\d*$");

        if (rx.exactMatch(input)) {
            return Acceptable;
        } else {
            // 如果不匹配，判断是否处于输入过程中的部分匹配状态
            rx.setPattern("^\\d*x?$");  // 允许"数字x"或仅数字
            if (rx.exactMatch(input)) {
                return Intermediate;
            }
        }

        return Invalid;
    }

    void fixup(QString &input) const override {
        // 自动修复逻辑可以根据需要进行调整
        if (!input.contains('x')) {
            // 如果输入中没有'x'，则在末尾添加'x'
            input += 'x';
        }
    }
};

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
    void calVariations();   //计算变化量
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
    void tableChanged();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void updateModelComboBoxItems(const QString& uuid);         //用于更新型号comboBox

    void sendTableChangedSignal();          //发送表格修改信号

    void onDebouncedChange();  // 新增的用于实际处理变化的槽函数

    void set_Noise_after_cal_Vector();

    void on_stackedWidget_table_currentChanged(int arg1);

    void on_comboBox_sound_type_currentTextChanged(const QString &arg1);

    void on_comboBox_unit_name_currentTextChanged(const QString &arg1);

    void on_comboBox_sound_type_room_currentTextChanged(const QString &arg1);

    void on_pushButton_number_clicked();

    void on_comboBox_room_type_currentTextChanged(const QString &arg1);

    void on_comboBox_fan_number_currentTextChanged(const QString &arg1);

    void on_comboBox_fanCoil_model_currentTextChanged(const QString &arg1);

    void on_comboBox_aircondition_number_currentTextChanged(const QString &arg1);

    void on_comboBox_VAV_terminal_number_currentTextChanged(const QString &arg1);

    void on_comboBox_circular_damper_diameter_currentTextChanged(const QString &arg1);

    void on_comboBox_circular_damper_angle_currentTextChanged(const QString &arg1);

    void on_comboBox_circular_damper_air_volume_currentTextChanged(const QString &arg1);

    void on_comboBox_rect_damper_size_currentTextChanged(const QString &arg1);

    void on_comboBox_rect_damper_angle_currentTextChanged(const QString &arg1);

    void on_comboBox_rect_damper_air_volume_currentTextChanged(const QString &arg1);

    void on_comboBox_air_distributor_model_currentTextChanged(const QString &arg1);

    void on_comboBox_air_diff_terminal_type_currentTextChanged(const QString &arg1);

    void on_comboBox_air_diff_size_currentTextChanged(const QString &arg1);

    void on_comboBox_fan_noise_locate_currentIndexChanged(int index);

    void on_comboBox_fanCoil_locate_currentIndexChanged(int index);

    void on_comboBox_aircondition_noise_locate_currentIndexChanged(int index);

    void on_comboBox_aircondition_fan_type_currentIndexChanged(int index);

private:
    Ui::RoomCalTable *ui;
    void clearPage(QWidget *widget);
    void connectLineEditsToCalSlot(const QVector<QLineEdit*>& lineEdits);
    void disconnectLineEditsToCalSlot(const QVector<QLineEdit*>& lineEdits);
    bool isCollapsed;
    QString systemName;       //所属系统名称
    QTimer debounceTimer;  // 定时器,用于处理短时间内有多个绑定的lineedit文本改变的信号
    bool updateModelComboBox = false;   //用来更新model，on_comboBox_unit_name_currentTextChanged利用这个函数而不清空当前界面

private:
    void clearLineEditsVectors();
    QVector<QLineEdit*> noi_after_cal_lineEdits;
    QVector<QLineEdit*> noi_lineEdits;       //噪音
    QVector<QLineEdit*> terminal_atten_lineEdits;       //末端衰减
    QVector<QLineEdit*> terminal_refl_lineEdits;       //末端反射衰减
    QVector<QLineEdit*> terminal_noi_lineEdits;       //末端气流噪音
    QVector<QLineEdit*> each_atten_lineEdits;       //1米直管或单个弯头衰减
    QVector<QLineEdit*> sum_atten_lineEdits;       //衰减汇总
    QVector<QLineEdit*> atten_lineEdits;       //衰减
    QVector<QLineEdit*> currentConnectedLineEdits;  //用来保存当前连接的lineEdit
    QList<QSharedPointer<ComponentBase>> currentComponentList;
    QList<QSharedPointer<ComponentBase>> currentAirconditionList_exhaust;   //排风机专用容器
    QSharedPointer<ComponentBase> currentComponent;
};

#endif // ROOMCALTABLE_H
