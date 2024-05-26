#ifndef DIALOG_AIRCONDITION_H
#define DIALOG_AIRCONDITION_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_aircondition;
}

class Dialog_aircondition : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_aircondition(QWidget *parent = nullptr, int editRow = -1, int fan_counts = 0, const Aircondition& data = Aircondition());
    Dialog_aircondition(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Aircondition& data = Aircondition()){}
    ~Dialog_aircondition();


private slots:
    void on_pushButton_confirm_clicked();
    bool check_lineedit(int counts);
    void calTotalNoise();
    void on_close_clicked();
private:
    int editRow;    //当前修改行，如果是新建就为-1
    Ui::Dialog_aircondition *ui;
    Aircondition* component;
    int fan_counts;
    QString table_id{-1};
    QString UUID{QString()};

    array<QLineEdit*,9> noi_send_in_lineEdits;
    array<QLineEdit*,9> noi_send_out_lineEdits;
    array<QLineEdit*,9> noi_exhaust_in_lineEdits;
    array<QLineEdit*,9> noi_exhaust_out_lineEdits;

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;

    // InputBaseDialog interface
public:
    void switchToCompontDB(bool inDB) override;
};

#endif // DIALOG_AIRCONDITION_H
