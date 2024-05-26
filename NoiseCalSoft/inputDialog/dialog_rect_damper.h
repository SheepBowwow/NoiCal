#ifndef DIALOG_RECT_DAMPER_H
#define DIALOG_RECT_DAMPER_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_rect_damper;
}

class Dialog_rect_damper : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_rect_damper(QWidget *parent = nullptr, int editRow = -1, const Rect_damper& data = Rect_damper());
    Dialog_rect_damper(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Rect_damper& data = Rect_damper()){}
    ~Dialog_rect_damper();

private slots:

    void on_close_clicked();

    void on_pushButton_confirm_clicked();
    bool check_lineedit();

    void calTotalNoise();

private:
    Ui::Dialog_rect_damper *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    Rect_damper* component;     //噪音结构体
    array<QLineEdit*,9> noi_lineEdits;
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

#endif // DIALOG_RECT_DAMPER_H
