#ifndef DIALOG_DISP_VENT_TERMINAL_H
#define DIALOG_DISP_VENT_TERMINAL_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_disp_vent_terminal;
}

class Dialog_disp_vent_terminal : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_disp_vent_terminal(QWidget *parent = nullptr, int editRow = -1, const Disp_vent_terminal& data = Disp_vent_terminal());
    Dialog_disp_vent_terminal(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Disp_vent_terminal& data = Disp_vent_terminal()){}
    ~Dialog_disp_vent_terminal();

private:
    Ui::Dialog_disp_vent_terminal *ui;
    Disp_vent_terminal* component;
    int editRow;    //当前修改行，如果是新建就为-1
    QString table_id{-1};
    QString UUID{QString()};

    array<QLineEdit*,9> noi_lineEdits;
    array<QLineEdit*,8> atten_lineEdits;
    array<QLineEdit*,8> refl_lineEdits;
    QString size;
    QString shape;

private slots:
    void on_pushButton_confirm_clicked();
    bool check_lineedit();

    void calTotalNoise();

    void on_close_clicked();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;


    // InputBaseDialog interface
public:
    void switchToCompontDB(bool inDB) override {};
};

#endif // DIALOG_DISP_VENT_TERMINAL_H
