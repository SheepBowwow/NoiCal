#ifndef DIALOG_REDUCER_H
#define DIALOG_REDUCER_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_reducer;
}

class Dialog_reducer : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_reducer(QWidget *parent = nullptr, int editRow = -1, const Reducer& data = Reducer());
    Dialog_reducer(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Reducer& data = Reducer()){}
    ~Dialog_reducer();
    void total_noi();

private slots:
    void on_close_clicked();

    void on_pushButton_confirm_clicked();

    void on_radioButton_circle_to_circle_clicked();

    void on_radioButton_rect_to_rect_clicked();

    void on_radioButton_circle_to_rect_clicked();

    void on_radioButton_rect_to_circle_clicked();

private:
    Ui::Dialog_reducer *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    Reducer* component;     //噪音结构体
    array<QLineEdit*,8> atten_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;
};

#endif // DIALOG_REDUCER_H
