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

struct Aircondition_in_calTable : public Aircondition
{
    QString send_or_exhaust;

    Aircondition_in_calTable(const Aircondition& ac)
        : Aircondition(ac) // 调用基类的拷贝构造函数来复制属性
    {
    }

    Aircondition_in_calTable() = default;
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

    void circular_damper_noise_cal();

    void rect_damper_noise_cal();

    void set_Noise_after_cal_Vector();

    void static_box_atten_cal();
    void multi_ranc_atten_cal();
    void tee_atten_cal();
    void pipe_atten_cal();
    void elbow_atten_cal();
    void reducer_atten_cal();

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

    void on_comboBox_air_distributor_model_currentTextChanged(const QString &arg1);

    void on_comboBox_air_diff_terminal_type_currentTextChanged(const QString &arg1);

    void on_comboBox_fan_noise_locate_currentIndexChanged(int index);

    void on_comboBox_fanCoil_noise_locate_currentIndexChanged(int index);

    void on_comboBox_aircondition_noise_locate_currentIndexChanged(int index);

    void on_comboBox_aircondition_fan_type_currentIndexChanged(int index);

    void on_comboBox_fanCoil_data_source_currentTextChanged(const QString &arg1);

    void on_comboBox_aircondition_data_source_currentTextChanged(const QString &arg1);

    void on_comboBox_VAV_terminal_data_source_currentTextChanged(const QString &arg1);

    void on_comboBox_circular_damper_model_currentTextChanged(const QString &arg1);

    void on_comboBox_rect_damper_data_source_currentTextChanged(const QString &arg1);

    void on_comboBox_rect_damper_model_currentTextChanged(const QString &arg1);

    void air_diff_data_source_changed();
    void pump_data_source_changed();
    void send_data_source_changed();
    void staticBox_grille_data_source_changed();
    void disp_vent_terminal_data_source_changed();
    void other_send_terminal_data_source_changed();

    void on_comboBox_diffuser_model_currentTextChanged(const QString &arg1);

    void on_comboBox_pump_model_currentTextChanged(const QString &arg1);

    void on_comboBox_pump_terminal_type_currentTextChanged(const QString &arg1);

    void on_comboBox_send_model_currentTextChanged(const QString &arg1);

    void on_comboBox_send_terminal_type_currentTextChanged(const QString &arg1);

    void on_comboBox_staticBox_model_currentTextChanged(const QString &arg1);

    void on_comboBox_grille_model_currentTextChanged(const QString &arg1);

    void on_comboBox_staticBox_grille_terminal_type_currentTextChanged(const QString &arg1);

    void on_comboBox_disp_vent_terminal_model_currentTextChanged(const QString &arg1);

    void on_comboBox_disp_vent_terminal_type_currentTextChanged(const QString &arg1);

    void on_comboBox_other_send_terminal_model_currentTextChanged(const QString &arg1);

    void on_comboBox_other_send_terminal_type_currentTextChanged(const QString &arg1);

    void on_comboBox_static_box_data_source_currentTextChanged(const QString &arg1);

    void on_comboBox_static_box_model_currentTextChanged(const QString &arg1);

    void on_comboBox_tee_data_source_currentTextChanged(const QString &arg1);

    void on_comboBox_tee_model_currentTextChanged(const QString &arg1);

    void on_comboBox_multi_ranc_data_source_currentTextChanged(const QString &arg1);

    void on_comboBox_multi_ranc_model_currentTextChanged(const QString &arg1);

    void on_comboBox_pipe_data_source_currentTextChanged(const QString &arg1);

    void on_lineEdit_pipe_size_textChanged(const QString &arg1);

    void on_lineEdit_pipe_length_textChanged(const QString &arg1);

    void on_comboBox_pipe_model_currentTextChanged(const QString &arg1);

    void on_comboBox_reducer_data_source_currentTextChanged(const QString &arg1);

    void on_comboBox_reducer_model_currentTextChanged(const QString &arg1);

    void on_comboBox_silencer_data_source_currentTextChanged(const QString &arg1);

    void on_comboBox_silencer_type_currentTextChanged(const QString &arg1);

    void on_comboBox_silencer_model_currentTextChanged(const QString &arg1);

    void on_comboBox_elbow_data_source_currentTextChanged(const QString &arg1);

    void on_lineEdit_elbow_count_textChanged(const QString &arg1);

    void on_comboBox_elbow_model_currentTextChanged(const QString &arg1);

    void on_comboBox_elbow_type_currentTextChanged(const QString &arg1);

private:
    Ui::RoomCalTable *ui;
    void clearPage(QWidget *widget, bool isPageChanged);
    void connectLineEditsToCalSlot(const QVector<QLineEdit*>& lineEdits);
    void disconnectLineEditsToCalSlot(const QVector<QLineEdit*>& lineEdits);
    bool isCollapsed;
    QString systemName;       //所属系统名称
    QTimer debounceTimer;  // 定时器,用于处理短时间内有多个绑定的lineedit文本改变的信号
    bool updateModelComboBox = false;   //用来更新model，on_comboBox_unit_name_currentTextChanged利用这个函数而不清空当前界面
    std::pair<int, int> splitDimension(const QString &size);    //用来分割矩形的size
    void initTerminalDataSourceConn();  //用来初始化所有末端的来源与槽函数连接

private:
    void clearPageControlVectors();
    QVector<QLineEdit*> noi_after_cal_lineEdits;
    QVector<QLineEdit*> noi_lineEdits;       //噪音
    QVector<QLineEdit*> terminal_atten_lineEdits;       //末端衰减
    QVector<QLineEdit*> terminal_refl_lineEdits;       //末端反射衰减
    QVector<QLineEdit*> terminal_noi_lineEdits;       //末端气流噪音
    QVector<QLineEdit*> each_atten_lineEdits;       //1米直管或单个弯头衰减
    QVector<QLineEdit*> sum_atten_lineEdits;       //衰减汇总
    QVector<QLineEdit*> atten_lineEdits;       //衰减
    QVector<QLineEdit*> currentConnectedLineEdits;  //用来保存当前连接的lineEdit
    /**
     * @brief currentAllComponentList
     * 一级列表，用来存放所有该部件的列表
     */
    QList<QSharedPointer<ComponentBase>> currentAllComponentList;
    /**
     * @brief currentComponentListByDataSource
     * 二级列表，选择来源之后确定哪些在列表里
     */
    QList<QSharedPointer<ComponentBase>> currentComponentListByDataSource;

    QSharedPointer<ComponentBase> currentComponent;
};

#endif // ROOMCALTABLE_H
