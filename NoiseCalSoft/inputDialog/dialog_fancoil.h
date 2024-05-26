#ifndef DIALOG_FANCOIL_H
#define DIALOG_FANCOIL_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_fanCoil;
}

class Dialog_fanCoil : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_fanCoil(QWidget *parent = nullptr, int editRow = -1, const FanCoil& data = FanCoil());
    Dialog_fanCoil(const QString& name, QWidget *parent = nullptr, int editRow = -1, const FanCoil& data = FanCoil()){}
    ~Dialog_fanCoil();

private:
    Ui::Dialog_fanCoil *ui;
    FanCoil* component;
    int editRow;    //当前修改行，如果是新建就为-1
    array<QLineEdit*,9> noi_in_lineEdits;
    array<QLineEdit*,9> noi_out_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

private slots:
    void on_pushButton_confirm_clicked();
    bool check_lineedit();
    void calTotalNoise();
    void on_close_clicked();


    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;

    // InputBaseDialog interface
public:
    void switchToCompontDB(bool inDB) override {};
};

#endif // DIALOG_FANCOIL_H
