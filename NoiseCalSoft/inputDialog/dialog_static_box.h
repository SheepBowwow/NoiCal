#ifndef DIALOG_STATIC_BOX_H
#define DIALOG_STATIC_BOX_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_static_box;
}

class Dialog_static_box : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_static_box(QWidget *parent = nullptr, int editRow = -1, const Static_box& data = Static_box());
    Dialog_static_box(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Static_box& data = Static_box()){}
    ~Dialog_static_box();

private:
    Ui::Dialog_static_box *ui;
    Static_box* component;
    int editRow;    //当前修改行，如果是新建就为-1
    array<QLineEdit*,8> atten_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

private slots:
    void on_pushButton_confirm_clicked();

    void on_close_clicked();

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;
};

#endif // DIALOG_STATIC_BOX_H
