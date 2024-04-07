#ifndef DIALOG_ELBOW_H
#define DIALOG_ELBOW_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_elbow;
}

class Dialog_elbow : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_elbow(QWidget *parent = nullptr, int editRow = -1, const Elbow& data = Elbow());
    Dialog_elbow(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Elbow& data = Elbow()){}
    ~Dialog_elbow();

private slots:
    void on_close_clicked();

    void on_pushButton_confirm_clicked();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

    void on_pushButton_clicked();

private:
    Ui::Dialog_elbow *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    Elbow* component;     //噪音结构体
    array<QLineEdit*,8> atten_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;
};

#endif // DIALOG_ELBOW_H
