#ifndef DIALOG_DISP_VENT_TERMINAL_H
#define DIALOG_DISP_VENT_TERMINAL_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_disp_vent_terminal;
}

class Dialog_disp_vent_terminal : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_disp_vent_terminal(QWidget *parent = nullptr, int editRow = -1, const Disp_vent_terminal_noise& data = Disp_vent_terminal_noise());
    Dialog_disp_vent_terminal(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Disp_vent_terminal_noise& data = Disp_vent_terminal_noise()){}
    ~Dialog_disp_vent_terminal();
    void* getNoi() override;
    void total_noi();

private:
    Ui::Dialog_disp_vent_terminal *ui;
    Disp_vent_terminal_noise* noi;
    int editRow;    //当前修改行，如果是新建就为-1

private slots:
    void on_pushButton_confirm_clicked();

    void calReflNoi();

    void calTotalNoise();

    void on_close_clicked();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

    void on_radioButton_known_clicked();

    void on_radioButton_formula_clicked();
    void on_pushButton_clicked();
};

#endif // DIALOG_DISP_VENT_TERMINAL_H
