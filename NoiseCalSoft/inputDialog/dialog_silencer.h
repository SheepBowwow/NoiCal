#ifndef DIALOG_SILENCER_H
#define DIALOG_SILENCER_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"
namespace Ui {
class Dialog_silencer;
}

namespace silencer_type_name
{
const char* const CIRCLE = "圆形消音器";
const char* const RECT = "矩形消音器";
const char* const CIRCLE_ELBOW = "圆形消音弯头";
const char* const RECT_ELBOW = "矩形消音弯头";
}

class Dialog_silencer : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_silencer(QString type, QWidget *parent = nullptr, int editRow = -1, const Silencer& data = Silencer());
    Dialog_silencer(QWidget *parent = nullptr, int editRow = -1, const Silencer& data = Silencer()){}
    ~Dialog_silencer();

private slots:
    void on_close_clicked();

    void on_pushButton_confirm_clicked();
    bool check_lineedit();

private:
    Ui::Dialog_silencer *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    Silencer* component;     //噪音结构体
    array<QLineEdit*,8> atten_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;

    // InputBaseDialog interface
public:
    void switchToCompontDB(bool inDB) override {};
};

#endif // DIALOG_SILENCER_H
