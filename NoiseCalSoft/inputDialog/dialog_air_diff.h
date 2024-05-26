#ifndef DIALOG_AIR_DIFF_H
#define DIALOG_AIR_DIFF_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_air_diff;
}

class Dialog_air_diff : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_air_diff(QWidget *parent = nullptr, int editRow = -1, const AirDiff& data = AirDiff());
    Dialog_air_diff(const QString& name, QWidget *parent = nullptr, int editRow = -1, const AirDiff& data = AirDiff()){}
    ~Dialog_air_diff();

private slots:

    void calTotalNoise();

    void calReflAtten();

    void on_pushButton_confirm_clicked();

    bool check_lineedit();

    void on_close_clicked();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

    void on_comboBox_refl_data_source_currentIndexChanged(int index);

private:
    Ui::Dialog_air_diff *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    AirDiff* component;     //噪音结构体
    QString table_id{-1};
    QString UUID{QString()};

    array<QLineEdit*,9> noi_lineEdits;
    array<QLineEdit*,8> atten_lineEdits;
    array<QLineEdit*,8> refl_lineEdits;
    QString size;
    QString shape;

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;

    // InputBaseDialog interface
public:
    void switchToCompontDB(bool inDB) override {};
};

#endif // DIALOG_AIR_DIFF_H
